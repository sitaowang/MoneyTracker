#ifndef TRANSACTIONMANAGER_H
#define TRANSACTIONMANAGER_H

#include "transaction.h"
#include <QObject>
#include <QList>
#include <QDateTime>

class TransactionManager : public QObject
{
    Q_OBJECT

public:
    explicit TransactionManager(QObject* parent = nullptr);

    // Core operations
    void addTransaction(const Transaction& transaction);
    bool deleteTransaction(const QString& id);
    QList<Transaction> getTransactions() const;
    Transaction getTransactionById(const QString& id) const;

    // Filtering operations
    QList<Transaction> filterByDate(const QDateTime& startDate, const QDateTime& endDate) const;
    QList<Transaction> filterByAmount(double minAmount, double maxAmount) const;
    QList<Transaction> filterByCategory(const QString& category) const;

    // Statistics
    double calculateTotalAmount() const;
    double calculateBalance() const;

    // Data persistence
    bool saveToFile(const QString& filename);
    bool loadFromFile(const QString& filename);

    // Utility
    void clearAll();
    int getTransactionCount() const;

signals:
    void transactionsChanged();
    void transactionAdded(const Transaction& transaction);
    void transactionDeleted(const QString& id);

private:
    QList<Transaction> m_transactions;
};

#endif // TRANSACTIONMANAGER_H
