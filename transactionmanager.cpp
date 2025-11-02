#include "transactionmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

TransactionManager::TransactionManager(QObject* parent)
    : QObject(parent)
{
}

void TransactionManager::addTransaction(const Transaction& transaction)
{
    m_transactions.append(transaction);
    emit transactionsChanged();
    emit transactionAdded(transaction);
}

bool TransactionManager::deleteTransaction(const QString& id)
{
    for (int i = 0; i < m_transactions.size(); ++i) {
        if (m_transactions[i].getId() == id) {
            m_transactions.removeAt(i);
            emit transactionsChanged();
            emit transactionDeleted(id);
            return true;
        }
    }
    return false;
}

QList<Transaction> TransactionManager::getTransactions() const
{
    return m_transactions;
}

Transaction TransactionManager::getTransactionById(const QString& id) const
{
    for (const auto& transaction : m_transactions) {
        if (transaction.getId() == id) {
            return transaction;
        }
    }
    return Transaction();
}

QList<Transaction> TransactionManager::filterByDate(const QDateTime& startDate, const QDateTime& endDate) const
{
    QList<Transaction> result;
    for (const auto& transaction : m_transactions) {
        QDateTime timestamp = transaction.getTimestamp();
        if (timestamp >= startDate && timestamp <= endDate) {
            result.append(transaction);
        }
    }
    return result;
}

QList<Transaction> TransactionManager::filterByAmount(double minAmount, double maxAmount) const
{
    QList<Transaction> result;
    for (const auto& transaction : m_transactions) {
        double amount = transaction.getAmount();
        if (amount >= minAmount && amount <= maxAmount) {
            result.append(transaction);
        }
    }
    return result;
}

QList<Transaction> TransactionManager::filterByCategory(const QString& category) const
{
    QList<Transaction> result;
    for (const auto& transaction : m_transactions) {
        if (transaction.getCategory() == category) {
            result.append(transaction);
        }
    }
    return result;
}

double TransactionManager::calculateTotalAmount() const
{
    double total = 0.0;
    for (const auto& transaction : m_transactions) {
        total += transaction.getAmount();
    }
    return total;
}

double TransactionManager::calculateBalance() const
{
    double balance = 0.0;
    for (const auto& transaction : m_transactions) {
        if (transaction.getType() == TransactionType::INCOME) {
            balance += transaction.getAmount();
        } else {
            balance -= transaction.getAmount();
        }
    }
    return balance;
}

bool TransactionManager::saveToFile(const QString& filename)
{
    QJsonArray jsonArray;
    for (const auto& transaction : m_transactions) {
        jsonArray.append(transaction.toJson());
    }

    QJsonDocument doc(jsonArray);
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    file.write(doc.toJson());
    file.close();
    return true;
}

bool TransactionManager::loadFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) {
        return false;
    }

    m_transactions.clear();
    QJsonArray jsonArray = doc.array();
    for (const auto& value : jsonArray) {
        if (value.isObject()) {
            Transaction transaction = Transaction::fromJson(value.toObject());
            m_transactions.append(transaction);
        }
    }

    emit transactionsChanged();
    return true;
}

void TransactionManager::clearAll()
{
    m_transactions.clear();
    emit transactionsChanged();
}

int TransactionManager::getTransactionCount() const
{
    return m_transactions.size();
}
