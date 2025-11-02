#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "transaction.h"
#include "transactionmanager.h"
#include "statisticscalculator.h"

#include <QMainWindow>
#include <QListWidgetItem>

// 前向声明
class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QListWidget;
class QTableWidget;
class QDateEdit;
class QPushButton;
class QComboBox;
class QGroupBox;
class QTabWidget;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddTransactionClicked();
    void onDeleteTransactionClicked();
    void onTransactionSelected(QListWidgetItem *item);
    void onFilterApplied();
    void onShowStatistics();
    void onShowBills();
    void onShowProfile();
    void onShowHome();

    void onTransactionsChanged();
    void updateTransactionList();
    void updateStatistics();
    void updateQuickStats();
    void updateBillsTable();

private:
    Ui::MainWindow *ui;
    TransactionManager *m_transactionManager;
    StatisticsCalculator *m_statsCalculator;

    // UI components
    QTabWidget *m_tabWidget;

    // Home tab
    QLabel *m_balanceLabel;
    QLabel *m_incomeLabel;
    QLabel *m_expenseLabel;
    QListWidget *m_transactionList;

    // Statistics tab
    QListWidget *m_statsList;
    QListWidget *m_categoryList;

    // Bills tab
    QTableWidget *m_billsTable;
    QDateEdit *m_startDateEdit;
    QDateEdit *m_endDateEdit;

    // Common
    QPushButton *m_quickAddBtn;

    void setupUI();
    void setupConnections();
    void loadSampleData();
    void showAddTransactionDialog();
    void refreshStatisticsDisplay();

};

#endif // MAINWINDOW_H
