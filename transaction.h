#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

enum class TransactionType {
    INCOME,
    EXPENSE
};

class Transaction
{
public:
    Transaction();
    Transaction(TransactionType type, double amount, const QString& fromAccount,
                const QString& toAccount, const QString& category, const QString& method,
                const QDateTime& timestamp = QDateTime::currentDateTime());

    // Getters
    QString getId() const;
    TransactionType getType() const;
    double getAmount() const;
    QString getFromAccount() const;
    QString getToAccount() const;
    QString getCategory() const;
    QString getMethod() const;
    QDateTime getTimestamp() const;

    // Setters
    void setType(TransactionType type);
    void setAmount(double amount);
    void setFromAccount(const QString& fromAccount);
    void setToAccount(const QString& toAccount);
    void setCategory(const QString& category);
    void setMethod(const QString& method);
    void setTimestamp(const QDateTime& timestamp);

    // Serialization
    QJsonObject toJson() const;
    static Transaction fromJson(const QJsonObject& json);

    // Utility
    QString getTypeString() const;
    QString getDisplayAmount() const;

private:
    QString m_id;
    TransactionType m_type;
    double m_amount;
    QString m_fromAccount;
    QString m_toAccount;
    QString m_category;
    QString m_method;
    QDateTime m_timestamp;
};

#endif // TRANSACTION_H
