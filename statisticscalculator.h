#ifndef STATISTICSCALCULATOR_H
#define STATISTICSCALCULATOR_H

#include "transaction.h"
#include <QObject>
#include <QMap>
#include <QDateTime>

struct MonthlyStats {
    double totalIncome;
    double totalExpense;
    double netAmount;

    MonthlyStats() : totalIncome(0.0), totalExpense(0.0), netAmount(0.0) {}
};

struct YearlyStats {
    QMap<int, MonthlyStats> monthlyData; // month (1-12) -> stats
    double totalIncome;
    double totalExpense;
    double netAmount;

    YearlyStats() : totalIncome(0.0), totalExpense(0.0), netAmount(0.0) {}
};

class StatisticsCalculator : public QObject
{
    Q_OBJECT

public:
    explicit StatisticsCalculator(QObject* parent = nullptr);

    // Core statistics
    double calculateTotalAmount(const QList<Transaction>& transactions);
    MonthlyStats calculateMonthlyStats(int month, int year, const QList<Transaction>& transactions);
    YearlyStats calculateYearlyStats(int year, const QList<Transaction>& transactions);

    // Category analysis
    QMap<QString, double> calculateCategoryBreakdown(const QList<Transaction>& transactions);
    QMap<QString, double> calculateExpenseByCategory(const QList<Transaction>& transactions);
    QMap<QString, double> calculateIncomeByCategory(const QList<Transaction>& transactions);

    // Time-based analysis
    QMap<QDate, double> calculateDailyTrend(const QDateTime& startDate, const QDateTime& endDate,
                                            const QList<Transaction>& transactions);

private:
    bool isTransactionInMonth(const Transaction& transaction, int month, int year);
    bool isTransactionInYear(const Transaction& transaction, int year);
};

#endif // STATISTICSCALCULATOR_H
