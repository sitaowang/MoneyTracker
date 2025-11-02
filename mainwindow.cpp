#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QTabWidget>
#include <QGroupBox>
#include <QTableWidget>
#include <QHeaderView>
#include <QFileDialog>
#include <QScrollArea>
#include <QFrame>
#include <QSpacerItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_transactionManager(new TransactionManager(this))
    , m_statsCalculator(new StatisticsCalculator(this))
    , m_tabWidget(nullptr)
    , m_balanceLabel(nullptr)
    , m_incomeLabel(nullptr)
    , m_expenseLabel(nullptr)
    , m_transactionList(nullptr)
    , m_statsList(nullptr)
    , m_categoryList(nullptr)
    , m_billsTable(nullptr)
    , m_startDateEdit(nullptr)
    , m_endDateEdit(nullptr)
    , m_quickAddBtn(nullptr)
{
    ui->setupUi(this);
    setupUI();

    setStyleSheet(
        // 主窗口和基础部件
        "QMainWindow { background-color: #f5f6fa; }"
        "QWidget { background-color: #f5f6fa; color: #333333; }"

        // 标签页
        "QTabWidget::pane { border: 1px solid #C2C7CB; background-color: white; }"
        "QTabWidget::tab-bar { alignment: center; }"
        "QTabBar::tab { background-color: #E1E5E9; color: #333333; padding: 8px 16px; margin: 2px; border: 1px solid #C2C7CB; border-radius: 4px; }"
        "QTabBar::tab:selected { background-color: #667eea; color: white; }"
        "QTabBar::tab:hover { background-color: #D6DBDF; }"

        // 分组框
        "QGroupBox { font-weight: bold; border: 1px solid #C2C7CB; border-radius: 5px; margin-top: 10px; padding-top: 10px; background-color: white; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px 0 5px; color: #2c3e50; }"

        // 列表和表格
        "QListWidget { background-color: white; border: 1px solid #C2C7CB; border-radius: 3px; alternate-background-color: #f8f9fa; }"
        "QTableWidget { background-color: white; border: 1px solid #C2C7CB; border-radius: 3px; alternate-background-color: #f8f9fa; gridline-color: #E1E5E9; }"
        "QHeaderView::section { background-color: #667eea; color: white; padding: 5px; border: 0px; }"

        // 按钮
        "QPushButton { background-color: #667eea; color: white; border: none; padding: 8px 16px; border-radius: 4px; font-weight: bold; }"
        "QPushButton:hover { background-color: #5a6fd8; }"
        "QPushButton:pressed { background-color: #4a5fc8; }"

        // 输入控件
        "QLineEdit, QComboBox, QDateEdit, QTimeEdit, QDoubleSpinBox { border: 1px solid #C2C7CB; border-radius: 3px; padding: 5px; background-color: white; color: #333333; }"
        "QLineEdit:focus, QComboBox:focus, QDateEdit:focus, QTimeEdit:focus, QDoubleSpinBox:focus { border-color: #667eea; }"

        // 对话框
        "QDialog { background-color: white; }"
        );

    setupConnections();
    loadSampleData();
    updateQuickStats();
    updateTransactionList();
    updateStatistics();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    setWindowTitle("记账本系统");
    setMinimumSize(800, 600);

    // Create central widget and main layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Create tab widget for different views
    m_tabWidget = new QTabWidget(centralWidget);

    // ==================== Home Tab ====================
    QWidget *homeTab = new QWidget();
    QVBoxLayout *homeLayout = new QVBoxLayout(homeTab);

    // Quick stats
    QGroupBox *statsGroup = new QGroupBox("财务概览");
    QHBoxLayout *statsLayout = new QHBoxLayout(statsGroup);

    m_balanceLabel = new QLabel("总资产: ¥ 0.00");
    m_balanceLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; padding: 10px;");

    m_incomeLabel = new QLabel("总收入: ¥ 0.00");
    m_incomeLabel->setStyleSheet("font-size: 14px; color: #27ae60; padding: 10px;");

    m_expenseLabel = new QLabel("总支出: ¥ 0.00");
    m_expenseLabel->setStyleSheet("font-size: 14px; color: #e74c3c; padding: 10px;");

    statsLayout->addWidget(m_balanceLabel);
    statsLayout->addWidget(m_incomeLabel);
    statsLayout->addWidget(m_expenseLabel);

    // Recent transactions
    QGroupBox *recentGroup = new QGroupBox("最近交易");
    QVBoxLayout *recentLayout = new QVBoxLayout(recentGroup);

    m_transactionList = new QListWidget();
    recentLayout->addWidget(m_transactionList);

    // Delete button
    QPushButton *deleteButton = new QPushButton("删除选中交易");
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteTransactionClicked);
    recentLayout->addWidget(deleteButton);

    homeLayout->addWidget(statsGroup);
    homeLayout->addWidget(recentGroup);

    // ==================== Statistics Tab ====================
    QWidget *statsTab = new QWidget();
    QVBoxLayout *statsTabLayout = new QVBoxLayout(statsTab);

    // Statistics overview
    QGroupBox *statsOverviewGroup = new QGroupBox("统计概览");
    QVBoxLayout *statsOverviewLayout = new QVBoxLayout(statsOverviewGroup);

    m_statsList = new QListWidget();
    statsOverviewLayout->addWidget(m_statsList);

    // Category breakdown
    QGroupBox *categoryGroup = new QGroupBox("支出分类统计");
    QVBoxLayout *categoryLayout = new QVBoxLayout(categoryGroup);

    m_categoryList = new QListWidget();
    categoryLayout->addWidget(m_categoryList);

    statsTabLayout->addWidget(statsOverviewGroup);
    statsTabLayout->addWidget(categoryGroup);

    // ==================== Bills Tab ====================
    QWidget *billsTab = new QWidget();
    QVBoxLayout *billsLayout = new QVBoxLayout(billsTab);

    // Filter controls
    QGroupBox *filterGroup = new QGroupBox("筛选条件");
    QHBoxLayout *filterLayout = new QHBoxLayout(filterGroup);

    m_startDateEdit = new QDateEdit(QDate::currentDate().addDays(-30));
    m_endDateEdit = new QDateEdit(QDate::currentDate());

    QPushButton *filterButton = new QPushButton("筛选");
    connect(filterButton, &QPushButton::clicked, this, &MainWindow::onFilterApplied);

    filterLayout->addWidget(new QLabel("开始日期:"));
    filterLayout->addWidget(m_startDateEdit);
    filterLayout->addWidget(new QLabel("结束日期:"));
    filterLayout->addWidget(m_endDateEdit);
    filterLayout->addWidget(filterButton);
    filterLayout->addStretch();

    // Bills table
    QGroupBox *billsTableGroup = new QGroupBox("账单明细");
    QVBoxLayout *billsTableLayout = new QVBoxLayout(billsTableGroup);

    m_billsTable = new QTableWidget();
    m_billsTable->setColumnCount(6);
    m_billsTable->setHorizontalHeaderLabels({"类型", "金额", "对方账户", "类别", "方式", "时间"});
    m_billsTable->horizontalHeader()->setStretchLastSection(true);
    m_billsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_billsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    billsTableLayout->addWidget(m_billsTable);

    billsLayout->addWidget(filterGroup);
    billsLayout->addWidget(billsTableGroup);

    // Add tabs
    m_tabWidget->addTab(homeTab, "🏠 首页");
    m_tabWidget->addTab(statsTab, "📊 统计");
    m_tabWidget->addTab(billsTab, "📝 账单");

    mainLayout->addWidget(m_tabWidget);

    // Quick add button
    m_quickAddBtn = new QPushButton("+", centralWidget);
    m_quickAddBtn->setFixedSize(60, 60);
    m_quickAddBtn->setStyleSheet(
        "QPushButton {"
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #667eea, stop:1 #764ba2);"
        "color: white; border-radius: 30px; font-size: 24px; font-weight: bold;"
        "}"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #5a6fd8, stop:1 #6a4190); }"
        );

    // Create overlay layout for floating button
    QHBoxLayout *overlayLayout = new QHBoxLayout();
    overlayLayout->addStretch();
    overlayLayout->addWidget(m_quickAddBtn);
    mainLayout->addLayout(overlayLayout);

    setCentralWidget(centralWidget);
}

void MainWindow::setupConnections()
{
    // Connect quick add button
    connect(m_quickAddBtn, &QPushButton::clicked, this, &MainWindow::showAddTransactionDialog);

    // Connect transaction list
    connect(m_transactionList, &QListWidget::itemClicked, this, &MainWindow::onTransactionSelected);

    // Connect transaction manager signals
    connect(m_transactionManager, &TransactionManager::transactionsChanged,
            this, &MainWindow::onTransactionsChanged);
}

void MainWindow::loadSampleData()
{
    // Add some sample transactions
    QDateTime now = QDateTime::currentDateTime();

    // 修复：创建 QDateTime 对象时使用正确的构造函数
    Transaction t1(TransactionType::INCOME, 8500.0, "公司", "我的账户", "工资", "银行卡",
                   QDateTime(now.date().addDays(-5), QTime(9, 0, 0)));
    Transaction t2(TransactionType::EXPENSE, 2500.0, "我的账户", "房东", "住房", "支付宝",
                   QDateTime(now.date().addDays(-4), QTime(10, 30, 0)));
    Transaction t3(TransactionType::EXPENSE, 38.5, "我的账户", "星巴克", "餐饮", "微信支付",
                   QDateTime(now.date().addDays(-3), QTime(15, 15, 0)));
    Transaction t4(TransactionType::EXPENSE, 25.0, "我的账户", "滴滴出行", "交通", "支付宝",
                   QDateTime(now.date().addDays(-2), QTime(8, 45, 0)));
    Transaction t5(TransactionType::INCOME, 2000.0, "张三", "我的账户", "转账", "微信支付",
                   QDateTime(now.date().addDays(-1), QTime(14, 20, 0)));
    Transaction t6(TransactionType::EXPENSE, 156.8, "我的账户", "超市", "购物", "微信支付",
                   QDateTime(now.date().addDays(-1), QTime(18, 30, 0)));
    Transaction t7(TransactionType::EXPENSE, 89.0, "我的账户", "电影院", "娱乐", "支付宝",
                   QDateTime(now.date(), QTime(20, 0, 0)));

    m_transactionManager->addTransaction(t1);
    m_transactionManager->addTransaction(t2);
    m_transactionManager->addTransaction(t3);
    m_transactionManager->addTransaction(t4);
    m_transactionManager->addTransaction(t5);
    m_transactionManager->addTransaction(t6);
    m_transactionManager->addTransaction(t7);
}

void MainWindow::updateQuickStats()
{
    double balance = m_transactionManager->calculateBalance();

    // Calculate total income and expense
    double totalIncome = 0.0;
    double totalExpense = 0.0;

    auto transactions = m_transactionManager->getTransactions();
    for (const auto& transaction : transactions) {
        if (transaction.getType() == TransactionType::INCOME) {
            totalIncome += transaction.getAmount();
        } else {
            totalExpense += transaction.getAmount();
        }
    }

    if (m_balanceLabel) {
        m_balanceLabel->setText(QString("总资产: ¥ %1").arg(balance, 0, 'f', 2));
    }

    if (m_incomeLabel) {
        m_incomeLabel->setText(QString("总收入: ¥ %1").arg(totalIncome, 0, 'f', 2));
    }

    if (m_expenseLabel) {
        m_expenseLabel->setText(QString("总支出: ¥ %1").arg(totalExpense, 0, 'f', 2));
    }
}

void MainWindow::updateTransactionList()
{
    if (!m_transactionList) return;

    m_transactionList->clear();

    auto transactions = m_transactionManager->getTransactions();

    // Sort transactions by timestamp (newest first)
    std::sort(transactions.begin(), transactions.end(),
              [](const Transaction& a, const Transaction& b) {
                  return a.getTimestamp() > b.getTimestamp();
              });

    // Show only recent transactions (last 10)
    int count = qMin(10, transactions.size());
    for (int i = 0; i < count; ++i) {
        const auto& transaction = transactions[i];
        QString displayText = QString("%1 %2 - %3 - %4")
                                  .arg(transaction.getDisplayAmount())
                                  .arg(transaction.getToAccount())
                                  .arg(transaction.getCategory())
                                  .arg(transaction.getTimestamp().toString("MM/dd hh:mm"));

        QListWidgetItem *item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, transaction.getId());

        // Color code based on type
        if (transaction.getType() == TransactionType::INCOME) {
            item->setForeground(QColor(39, 174, 96)); // Green
        } else {
            item->setForeground(QColor(231, 76, 60)); // Red
        }

        m_transactionList->addItem(item);
    }
}

void MainWindow::updateStatistics()
{
    refreshStatisticsDisplay();
}

void MainWindow::refreshStatisticsDisplay()
{
    if (!m_statsList || !m_categoryList) return;

    m_statsList->clear();
    m_categoryList->clear();

    auto transactions = m_transactionManager->getTransactions();

    // Calculate monthly stats for current month
    QDate currentDate = QDate::currentDate();
    MonthlyStats monthlyStats = m_statsCalculator->calculateMonthlyStats(
        currentDate.month(), currentDate.year(), transactions);

    // Add statistics to list
    m_statsList->addItem(QString("本月收入: ¥ %1").arg(monthlyStats.totalIncome, 0, 'f', 2));
    m_statsList->addItem(QString("本月支出: ¥ %1").arg(monthlyStats.totalExpense, 0, 'f', 2));
    m_statsList->addItem(QString("本月结余: ¥ %1").arg(monthlyStats.netAmount, 0, 'f', 2));

    // Calculate category breakdown
    auto expenseBreakdown = m_statsCalculator->calculateExpenseByCategory(transactions);
    double totalExpense = monthlyStats.totalExpense;

    // Add category breakdown to list
    for (auto it = expenseBreakdown.begin(); it != expenseBreakdown.end(); ++it) {
        if (it.value() > 0) {
            double percentage = (it.value() / totalExpense) * 100;
            m_categoryList->addItem(
                QString("%1: ¥ %2 (%3%)")
                    .arg(it.key())
                    .arg(it.value(), 0, 'f', 2)
                    .arg(percentage, 0, 'f', 1)
                );
        }
    }
}

void MainWindow::updateBillsTable()
{
    if (!m_billsTable) return;

    // 修复：将 QDate 转换为 QDateTime
    QDateTime startDate = QDateTime(m_startDateEdit->date(), QTime(0, 0, 0));
    QDateTime endDate = QDateTime(m_endDateEdit->date().addDays(1), QTime(0, 0, 0)); // Include the end date

    auto filteredTransactions = m_transactionManager->filterByDate(startDate, endDate);

    // Sort by timestamp (newest first)
    std::sort(filteredTransactions.begin(), filteredTransactions.end(),
              [](const Transaction& a, const Transaction& b) {
                  return a.getTimestamp() > b.getTimestamp();
              });

    m_billsTable->setRowCount(filteredTransactions.size());

    for (int i = 0; i < filteredTransactions.size(); ++i) {
        const auto& transaction = filteredTransactions[i];

        m_billsTable->setItem(i, 0, new QTableWidgetItem(transaction.getTypeString()));

        QTableWidgetItem *amountItem = new QTableWidgetItem(transaction.getDisplayAmount());
        if (transaction.getType() == TransactionType::INCOME) {
            amountItem->setForeground(QColor(39, 174, 96)); // Green
        } else {
            amountItem->setForeground(QColor(231, 76, 60)); // Red
        }
        m_billsTable->setItem(i, 1, amountItem);

        m_billsTable->setItem(i, 2, new QTableWidgetItem(transaction.getToAccount()));
        m_billsTable->setItem(i, 3, new QTableWidgetItem(transaction.getCategory()));
        m_billsTable->setItem(i, 4, new QTableWidgetItem(transaction.getMethod()));
        m_billsTable->setItem(i, 5, new QTableWidgetItem(
                                        transaction.getTimestamp().toString("yyyy-MM-dd hh:mm")));
    }

    // Resize columns to content
    m_billsTable->resizeColumnsToContents();
}

void MainWindow::onTransactionsChanged()
{
    updateTransactionList();
    updateQuickStats();
    updateStatistics();
    updateBillsTable();
}

void MainWindow::showAddTransactionDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("添加交易记录");
    dialog.setMinimumWidth(400);

    QFormLayout form(&dialog);

    // Transaction type
    QComboBox *typeCombo = new QComboBox();
    typeCombo->addItem("支出", static_cast<int>(TransactionType::EXPENSE));
    typeCombo->addItem("收入", static_cast<int>(TransactionType::INCOME));

    // Amount
    QDoubleSpinBox *amountSpin = new QDoubleSpinBox();
    amountSpin->setRange(0.01, 1000000.0);
    amountSpin->setDecimals(2);
    amountSpin->setPrefix("¥ ");

    // Accounts
    QLineEdit *fromAccountEdit = new QLineEdit();
    fromAccountEdit->setPlaceholderText("例如：我的钱包");

    QLineEdit *toAccountEdit = new QLineEdit();
    toAccountEdit->setPlaceholderText("例如：超市");

    // Category
    QComboBox *categoryCombo = new QComboBox();
    categoryCombo->addItems({"餐饮", "交通", "购物", "娱乐", "医疗", "住房", "工资", "转账", "其他"});

    // Method
    QComboBox *methodCombo = new QComboBox();
    methodCombo->addItems({"微信支付", "支付宝", "现金", "银行卡"});

    // Date
    QDateEdit *dateEdit = new QDateEdit(QDate::currentDate());
    dateEdit->setCalendarPopup(true);

    // Time
    QTimeEdit *timeEdit = new QTimeEdit(QTime::currentTime());

    // Note
    QLineEdit *noteEdit = new QLineEdit();
    noteEdit->setPlaceholderText("可选备注信息");

    form.addRow("类型:", typeCombo);
    form.addRow("金额:", amountSpin);
    form.addRow("付款方:", fromAccountEdit);
    form.addRow("收款方:", toAccountEdit);
    form.addRow("类别:", categoryCombo);
    form.addRow("方式:", methodCombo);
    form.addRow("日期:", dateEdit);
    form.addRow("时间:", timeEdit);
    form.addRow("备注:", noteEdit);

    QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form.addRow(&buttons);

    connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        TransactionType type = static_cast<TransactionType>(typeCombo->currentData().toInt());

        // Set default accounts based on type
        QString fromAccount = fromAccountEdit->text();
        QString toAccount = toAccountEdit->text();

        if (fromAccount.isEmpty()) {
            fromAccount = (type == TransactionType::INCOME) ? "对方账户" : "我的账户";
        }
        if (toAccount.isEmpty()) {
            toAccount = (type == TransactionType::INCOME) ? "我的账户" : "商家";
        }

        // 修复：将 QDate 和 QTime 组合成 QDateTime
        QDate selectedDate = dateEdit->date();
        QTime selectedTime = timeEdit->time();
        QDateTime transactionDateTime = QDateTime(selectedDate, selectedTime);

        Transaction transaction(type, amountSpin->value(), fromAccount,
                                toAccount, categoryCombo->currentText(),
                                methodCombo->currentText(), transactionDateTime);

        m_transactionManager->addTransaction(transaction);

        QMessageBox::information(this, "成功", "交易记录已添加！");
    }
}

void MainWindow::onAddTransactionClicked()
{
    showAddTransactionDialog();
}

void MainWindow::onDeleteTransactionClicked()
{
    QListWidgetItem *currentItem = m_transactionList->currentItem();

    if (currentItem) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "确认删除",
                                      "确定要删除这条交易记录吗？",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            QString transactionId = currentItem->data(Qt::UserRole).toString();
            bool success = m_transactionManager->deleteTransaction(transactionId);

            if (success) {
                QMessageBox::information(this, "成功", "交易记录已删除！");
            } else {
                QMessageBox::warning(this, "错误", "删除交易记录失败！");
            }
        }
    } else {
        QMessageBox::warning(this, "警告", "请先选择要删除的交易记录！");
    }
}

void MainWindow::onTransactionSelected(QListWidgetItem *item)
{
    // Show transaction details
    QString transactionId = item->data(Qt::UserRole).toString();
    Transaction transaction = m_transactionManager->getTransactionById(transactionId);

    QString details = QString(
                          "交易详情:\n\n"
                          "类型: %1\n"
                          "金额: %2\n"
                          "付款方: %3\n"
                          "收款方: %4\n"
                          "类别: %5\n"
                          "方式: %6\n"
                          "时间: %7"
                          ).arg(transaction.getTypeString())
                          .arg(transaction.getDisplayAmount())
                          .arg(transaction.getFromAccount())
                          .arg(transaction.getToAccount())
                          .arg(transaction.getCategory())
                          .arg(transaction.getMethod())
                          .arg(transaction.getTimestamp().toString("yyyy-MM-dd hh:mm"));

    QMessageBox::information(this, "交易详情", details);
}

void MainWindow::onFilterApplied()
{
    updateBillsTable();
}

void MainWindow::onShowStatistics()
{
    if (m_tabWidget) {
        m_tabWidget->setCurrentIndex(1); // Statistics tab
    }
}

void MainWindow::onShowBills()
{
    if (m_tabWidget) {
        m_tabWidget->setCurrentIndex(2); // Bills tab
    }
}

void MainWindow::onShowProfile()
{
    // For future implementation
    QMessageBox::information(this, "功能提示", "个人资料页面正在开发中...");
}

void MainWindow::onShowHome()
{
    if (m_tabWidget) {
        m_tabWidget->setCurrentIndex(0); // Home tab
    }
}
