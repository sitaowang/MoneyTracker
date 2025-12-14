#include <QTest>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTemporaryFile>

// 包含你的项目头文件
#include "../MoneyTracker/transaction.h"
#include "../MoneyTracker/statisticscalculator.h"
#include "../MoneyTracker/transactionmanager.h"

class MoneyTrackerTests : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();    // 测试开始前初始化（可选）
    void cleanupTestCase(); // 测试结束后清理（可选）

    // StatisticsCalculator 测试
    void testCalculateMonthlyStats_empty();
    void testCalculateMonthlyStats_singleIncome();
    void testCalculateMonthlyStats_singleExpense();
    void testCalculateMonthlyStats_mixedSameMonth();
    void testCalculateMonthlyStats_crossMonth();
    void testCalculateYearlyStats();
    void testCalculateExpenseByCategory();
    void testCalculateDailyTrend();

    // TransactionManager 测试
    void testAddAndGetTransactions();
    void testDeleteTransaction();
    void testCalculateBalance();
    void testSaveAndLoadJson();

private:
    QList<Transaction> createSampleTransactions();
};

void MoneyTrackerTests::initTestCase()
{
    // 可选：全局初始化
}

void MoneyTrackerTests::cleanupTestCase()
{
    // 可选：全局清理
}

QList<Transaction> MoneyTrackerTests::createSampleTransactions()
{
    QList<Transaction> list;

    // 2025-12-01 收入 ¥5000 工资
    QDateTime dt1(QDate(2025, 12, 1), QTime(10, 0));
    list << Transaction(TransactionType::INCOME, 5000.0, "对方", "我", "工资", "支付宝", dt1);

    // 2025-12-15 支出 ¥300 餐饮
    QDateTime dt2(QDate(2025, 12, 15), QTime(12, 30));
    list << Transaction(TransactionType::EXPENSE, 300.0, "我", "超市", "餐饮", "微信", dt2);

    // 2025-11-20 支出 ¥1200 购物（跨月）
    QDateTime dt3(QDate(2025, 11, 20), QTime(18, 0));
    list << Transaction(TransactionType::EXPENSE, 1200.0, "我", "商场", "购物", "银行卡", dt3);

    // 2025-12-20 收入 ¥2000 转账
    QDateTime dt4(QDate(2025, 12, 20), QTime(9, 0));
    list << Transaction(TransactionType::INCOME, 2000.0, "朋友", "我", "转账", "微信", dt4);

    return list;
}

// ====================== StatisticsCalculator 测试 ======================

void MoneyTrackerTests::testCalculateMonthlyStats_empty()
{
    StatisticsCalculator calc;
    QList<Transaction> empty;
    MonthlyStats stats = calc.calculateMonthlyStats(12, 2025, empty);

    QCOMPARE(stats.totalIncome, 0.0);
    QCOMPARE(stats.totalExpense, 0.0);
    QCOMPARE(stats.netAmount, 0.0);
}

void MoneyTrackerTests::testCalculateMonthlyStats_singleIncome()
{
    StatisticsCalculator calc;
    QList<Transaction> list = createSampleTransactions();
    MonthlyStats stats = calc.calculateMonthlyStats(12, 2025, list);

    QVERIFY(qFuzzyCompare(stats.totalIncome, 7000.0));  // 5000 + 2000
    QVERIFY(qFuzzyCompare(stats.totalExpense, 300.0));
    QVERIFY(qFuzzyCompare(stats.netAmount, 6700.0));
}

void MoneyTrackerTests::testCalculateMonthlyStats_singleExpense()
{
    // 只取12月支出
    StatisticsCalculator calc;
    QList<Transaction> list = createSampleTransactions();
    MonthlyStats stats = calc.calculateMonthlyStats(12, 2025, list);
    QVERIFY(qFuzzyCompare(stats.totalExpense, 300.0));
}

void MoneyTrackerTests::testCalculateMonthlyStats_mixedSameMonth()
{
    // 同上，混合
    StatisticsCalculator calc;
    QList<Transaction> list = createSampleTransactions();
    MonthlyStats stats = calc.calculateMonthlyStats(12, 2025, list);
    QVERIFY(qFuzzyCompare(stats.netAmount, 6700.0));
}

void MoneyTrackerTests::testCalculateMonthlyStats_crossMonth()
{
    StatisticsCalculator calc;
    QList<Transaction> list = createSampleTransactions();
    MonthlyStats statsNov = calc.calculateMonthlyStats(11, 2025, list);
    QVERIFY(qFuzzyCompare(statsNov.totalExpense, 1200.0));
    QVERIFY(qFuzzyCompare(statsNov.totalIncome, 0.0));
}

void MoneyTrackerTests::testCalculateYearlyStats()
{
    StatisticsCalculator calc;
    QList<Transaction> list = createSampleTransactions();
    YearlyStats yearly = calc.calculateYearlyStats(2025, list);

    QVERIFY(qFuzzyCompare(yearly.totalIncome, 7000.0));
    QVERIFY(qFuzzyCompare(yearly.totalExpense, 1500.0)); // 300 + 1200
    QVERIFY(qFuzzyCompare(yearly.netAmount, 5500.0));

    // 检查11月和12月数据
    QVERIFY(qFuzzyCompare(yearly.monthlyData[12].netAmount, 6700.0));
    QVERIFY(qFuzzyCompare(yearly.monthlyData[11].netAmount, -1200.0));
}

void MoneyTrackerTests::testCalculateExpenseByCategory()
{
    StatisticsCalculator calc;
    QList<Transaction> list = createSampleTransactions();
    QMap<QString, double> breakdown = calc.calculateExpenseByCategory(list);

    QCOMPARE(breakdown.size(), 2);
    QVERIFY(qFuzzyCompare(breakdown["餐饮"], 300.0));
    QVERIFY(qFuzzyCompare(breakdown["购物"], 1200.0));
}

void MoneyTrackerTests::testCalculateDailyTrend()
{
    StatisticsCalculator calc;
    QList<Transaction> list = createSampleTransactions();

    QDateTime start(QDate(2025, 12, 1), QTime(0,0));
    QDateTime end(QDate(2025, 12, 31), QTime(23,59));

    QMap<QDate, double> trend = calc.calculateDailyTrend(start, end, list);

    QVERIFY(qFuzzyCompare(trend[QDate(2025,12,1)], 5000.0));   // +收入
    QVERIFY(qFuzzyCompare(trend[QDate(2025,12,15)], -300.0));  // -支出
    QVERIFY(qFuzzyCompare(trend[QDate(2025,12,20)], 2000.0));  // +收入
}

// ====================== TransactionManager 测试 ======================

void MoneyTrackerTests::testAddAndGetTransactions()
{
    TransactionManager manager;
    QList<Transaction> sample = createSampleTransactions();

    for (const auto& t : sample) {
        manager.addTransaction(t);
    }

    QCOMPARE(manager.getTransactions().size(), sample.size());
}

void MoneyTrackerTests::testDeleteTransaction()
{
    TransactionManager manager;
    QList<Transaction> sample = createSampleTransactions();
    manager.addTransaction(sample[0]);
    manager.addTransaction(sample[1]);

    QString idToDelete = sample[0].getId();
    QVERIFY(manager.deleteTransaction(idToDelete));

    QCOMPARE(manager.getTransactions().size(), 1);
    QVERIFY(manager.getTransactionById(idToDelete).getId().isEmpty()); // 已删除
}

void MoneyTrackerTests::testCalculateBalance()
{
    TransactionManager manager;
    QList<Transaction> sample = createSampleTransactions();

    for (const auto& t : sample) {
        manager.addTransaction(t);
    }

    QVERIFY(qFuzzyCompare(manager.calculateBalance(), 5500.0)); // 7000收入 - 1500支出
}

void MoneyTrackerTests::testSaveAndLoadJson()
{
    TransactionManager manager;
    QList<Transaction> sample = createSampleTransactions();

    for (const auto& t : sample) {
        manager.addTransaction(t);
    }

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QString filename = tempFile.fileName();
    tempFile.close();

    QVERIFY(manager.saveToFile(filename));

    TransactionManager newManager;
    QVERIFY(newManager.loadFromFile(filename));

    QCOMPARE(newManager.getTransactionCount(), sample.size());
    QVERIFY(qFuzzyCompare(newManager.calculateBalance(), 5500.0));
}

QTEST_APPLESS_MAIN(MoneyTrackerTests)

// #include "tst_moneytrackertests.moc"
