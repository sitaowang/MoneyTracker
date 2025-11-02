#include "statisticscalculator.h"
#include <QDate>

StatisticsCalculator::StatisticsCalculator(QObject* parent)
    : QObject(parent)
{
}

double StatisticsCalculator::calculateTotalAmount(const QList<Transaction>& transactions)
{
    double total = 0.0;
    for (const auto& transaction : transactions) {
        total += transaction.getAmount();
    }
    return total;
}

MonthlyStats StatisticsCalculator::calculateMonthlyStats(int month, int year, const QList<Transaction>& transactions)
{
    MonthlyStats stats;

    for (const auto& transaction : transactions) {
        if (isTransactionInMonth(transaction, month, year)) {
            if (transaction.getType() == TransactionType::INCOME) {
                stats.totalIncome += transaction.getAmount();
            } else {
                stats.totalExpense += transaction.getAmount();
            }
        }
    }

    stats.netAmount = stats.totalIncome - stats.totalExpense;
    return stats;
}

YearlyStats StatisticsCalculator::calculateYearlyStats(int year, const QList<Transaction>& transactions)
{
    YearlyStats yearlyStats;

    // Initialize monthly data
    for (int month = 1; month <= 12; ++month) {
        yearlyStats.monthlyData[month] = calculateMonthlyStats(month, year, transactions);
    }

    // Calculate yearly totals
    for (const auto& transaction : transactions) {
        if (isTransactionInYear(transaction, year)) {
            if (transaction.getType() == TransactionType::INCOME) {
                yearlyStats.totalIncome += transaction.getAmount();
            } else {
                yearlyStats.totalExpense += transaction.getAmount();
            }
        }
    }

    yearlyStats.netAmount = yearlyStats.totalIncome - yearlyStats.totalExpense;
    return yearlyStats;
}

QMap<QString, double> StatisticsCalculator::calculateCategoryBreakdown(const QList<Transaction>& transactions)
{
    QMap<QString, double> breakdown;

    for (const auto& transaction : transactions) {
        QString category = transaction.getCategory();
        breakdown[category] += transaction.getAmount();
    }

    return breakdown;
}

QMap<QString, double> StatisticsCalculator::calculateExpenseByCategory(const QList<Transaction>& transactions)
{
    QMap<QString, double> expenseBreakdown;

    for (const auto& transaction : transactions) {
        if (transaction.getType() == TransactionType::EXPENSE) {
            QString category = transaction.getCategory();
            expenseBreakdown[category] += transaction.getAmount();
        }
    }

    return expenseBreakdown;
}

QMap<QString, double> StatisticsCalculator::calculateIncomeByCategory(const QList<Transaction>& transactions)
{
    QMap<QString, double> incomeBreakdown;

    for (const auto& transaction : transactions) {
        if (transaction.getType() == TransactionType::INCOME) {
            QString category = transaction.getCategory();
            incomeBreakdown[category] += transaction.getAmount();
        }
    }

    return incomeBreakdown;
}

QMap<QDate, double> StatisticsCalculator::calculateDailyTrend(const QDateTime& startDate,
                                                              const QDateTime& endDate,
                                                              const QList<Transaction>& transactions)
{
    QMap<QDate, double> dailyTrend;

    for (const auto& transaction : transactions) {
        QDateTime timestamp = transaction.getTimestamp();
        if (timestamp >= startDate && timestamp <= endDate) {
            QDate date = timestamp.date();
            if (transaction.getType() == TransactionType::INCOME) {
                dailyTrend[date] += transaction.getAmount();
            } else {
                dailyTrend[date] -= transaction.getAmount();
            }
        }
    }

    return dailyTrend;
}

bool StatisticsCalculator::isTransactionInMonth(const Transaction& transaction, int month, int year)
{
    QDateTime timestamp = transaction.getTimestamp();
    return timestamp.date().month() == month && timestamp.date().year() == year;
}

bool StatisticsCalculator::isTransactionInYear(const Transaction& transaction, int year)
{
    return transaction.getTimestamp().date().year() == year;
}
