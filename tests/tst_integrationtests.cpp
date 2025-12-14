#include <QTest>
#include <QDateTime>
#include <QTemporaryFile>
#include <QFile>

// 项目头文件
#include "../transaction.h"
#include "../transactionmanager.h"
#include "../statisticscalculator.h"

class IntegrationTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testAddTransactionsAndStatistics();
    void testSaveLoadAndStatisticsConsistency();
    void testDeleteTransactionAndStatisticsUpdate();

private:
    QList<Transaction> createSampleTransactions();
    TransactionManager *m_manager;
    StatisticsCalculator *m_calculator;
};

void IntegrationTests::initTestCase()
{
    m_manager = new TransactionManager(this);
    m_calculator = new StatisticsCalculator(this);
}

void IntegrationTests::cleanupTestCase()
{
    // 自动由 QObject 父子机制删除
}

QList<Transaction> IntegrationTests::createSampleTransactions()
{
    QList<Transaction> list;

    QDateTime dt1(QDate(2025, 12, 5), QTime(9, 30));
    list << Transaction(TransactionType::INCOME, 8000.0, "公司", "我", "工资", "银行转账", dt1);

    QDateTime dt2(QDate(2025, 12, 10), QTime(12, 0));
    list << Transaction(TransactionType::EXPENSE, 450.0, "我", "餐厅", "餐饮", "微信支付", dt2);

    QDateTime dt3(QDate(2025, 12, 15), QTime(18, 45));
    list << Transaction(TransactionType::EXPENSE, 1200.0, "我", "商场", "购物", "银行卡", dt3);

    QDateTime dt4(QDate(2025, 12, 20), QTime(14, 0));
    list << Transaction(TransactionType::INCOME, 3000.0, "朋友", "我", "转账", "支付宝", dt4);

    QDateTime dt5(QDate(2025, 11, 28), QTime(20, 0)); // 跨月，用于边界测试
    list << Transaction(TransactionType::EXPENSE, 800.0, "我", "超市", "餐饮", "现金", dt5);

    return list;
}

// 集成测试1：添加交易 → 使用 StatisticsCalculator 计算是否正确
void IntegrationTests::testAddTransactionsAndStatistics()
{
    QList<Transaction> samples = createSampleTransactions();

    // 通过 TransactionManager 添加
    for (const auto& t : samples) {
        m_manager->addTransaction(t);
    }

    const QList<Transaction>& all = m_manager->getTransactions();
    QCOMPARE(all.size(), samples.size());

    // 使用 StatisticsCalculator 直接计算（集成点）
    MonthlyStats decemberStats = m_calculator->calculateMonthlyStats(12, 2025, all);

    QVERIFY(qFuzzyCompare(decemberStats.totalIncome, 11000.0));   // 8000 + 3000
    QVERIFY(qFuzzyCompare(decemberStats.totalExpense, 1650.0));  // 450 + 1200
    QVERIFY(qFuzzyCompare(decemberStats.netAmount, 9350.0));

    // 余额计算（另一集成点）
    QVERIFY(qFuzzyCompare(m_manager->calculateBalance(), 9350.0 + (-800.0))); // 总余额包含11月
    QVERIFY(qFuzzyCompare(m_manager->calculateBalance(), 8550.0));
}

// 集成测试2：持久化 + 统计一致性（自底向上集成）
void IntegrationTests::testSaveLoadAndStatisticsConsistency()
{
    // 第一阶段：添加数据并计算统计
    {
        TransactionManager tempManager(this);
        QList<Transaction> samples = createSampleTransactions();
        for (const auto& t : samples) {
            tempManager.addTransaction(t);
        }

        QTemporaryFile tempFile;
        QVERIFY(tempFile.open());
        QString filename = tempFile.fileName();
        tempFile.close();

        QVERIFY(tempManager.saveToFile(filename));

        // 计算原始统计
        StatisticsCalculator calc;
        YearlyStats originalYearly = calc.calculateYearlyStats(2025, tempManager.getTransactions());
        QVERIFY(qFuzzyCompare(originalYearly.netAmount, 8550.0));
        QVERIFY(qFuzzyCompare(originalYearly.monthlyData[12].netAmount, 9350.0));

        // 第二阶段：清空后重新加载
        m_manager->clearAll();
        QCOMPARE(m_manager->getTransactionCount(), 0);

        QVERIFY(m_manager->loadFromFile(filename));

        // 重新计算统计并比较
        YearlyStats loadedYearly = m_calculator->calculateYearlyStats(2025, m_manager->getTransactions());
        QCOMPARE(loadedYearly.totalIncome, originalYearly.totalIncome);
        QCOMPARE(loadedYearly.totalExpense, originalYearly.totalExpense);
        QVERIFY(qFuzzyCompare(loadedYearly.netAmount, 8550.0));
        QVERIFY(qFuzzyCompare(loadedYearly.monthlyData[12].netAmount, 9350.0));

        // 清理临时文件
        QFile::remove(filename);
    }
}

// 集成测试3：删除交易后统计和余额更新
void IntegrationTests::testDeleteTransactionAndStatisticsUpdate()
{
    m_manager->clearAll();

    QList<Transaction> samples = createSampleTransactions();
    for (const auto& t : samples) {
        m_manager->addTransaction(t);
    }

    // 记录删除前状态
    double balanceBefore = m_manager->calculateBalance();
    QVERIFY(qFuzzyCompare(balanceBefore, 8550.0));

    // 删除一笔12月的收入（8000工资）
    QString incomeIdToDelete = m_manager->getTransactions().first().getId(); // 第一笔是工资收入
    QVERIFY(m_manager->deleteTransaction(incomeIdToDelete));

    // 删除后验证
    double balanceAfter = m_manager->calculateBalance();
    QVERIFY(qFuzzyCompare(balanceAfter, 8550.0 - 8000.0)); // 8550 - 8000 = 550

    // 统计更新验证
    MonthlyStats decAfterDelete = m_calculator->calculateMonthlyStats(12, 2025, m_manager->getTransactions());
    QVERIFY(qFuzzyCompare(decAfterDelete.totalIncome, 3000.0));   // 只剩转账3000
    QVERIFY(qFuzzyCompare(decAfterDelete.totalExpense, 1650.0));
    QVERIFY(qFuzzyCompare(decAfterDelete.netAmount, 1350.0));
}

QTEST_APPLESS_MAIN(IntegrationTests)

#include "tst_integrationtests.moc"