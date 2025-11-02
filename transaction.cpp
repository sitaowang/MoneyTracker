#include "transaction.h"
#include <QUuid>
#include <QJsonObject>
#include <QJsonValue>

Transaction::Transaction()
    : m_id(QUuid::createUuid().toString())
    , m_type(TransactionType::EXPENSE)
    , m_amount(0.0)
{
}

Transaction::Transaction(TransactionType type, double amount, const QString& fromAccount,
                         const QString& toAccount, const QString& category, const QString& method,
                         const QDateTime& timestamp)
    : m_id(QUuid::createUuid().toString())
    , m_type(type)
    , m_amount(amount)
    , m_fromAccount(fromAccount)
    , m_toAccount(toAccount)
    , m_category(category)
    , m_method(method)
    , m_timestamp(timestamp)
{
}

QString Transaction::getId() const { return m_id; }
TransactionType Transaction::getType() const { return m_type; }
double Transaction::getAmount() const { return m_amount; }
QString Transaction::getFromAccount() const { return m_fromAccount; }
QString Transaction::getToAccount() const { return m_toAccount; }
QString Transaction::getCategory() const { return m_category; }
QString Transaction::getMethod() const { return m_method; }
QDateTime Transaction::getTimestamp() const { return m_timestamp; }

void Transaction::setType(TransactionType type) { m_type = type; }
void Transaction::setAmount(double amount) { m_amount = amount; }
void Transaction::setFromAccount(const QString& fromAccount) { m_fromAccount = fromAccount; }
void Transaction::setToAccount(const QString& toAccount) { m_toAccount = toAccount; }
void Transaction::setCategory(const QString& category) { m_category = category; }
void Transaction::setMethod(const QString& method) { m_method = method; }
void Transaction::setTimestamp(const QDateTime& timestamp) { m_timestamp = timestamp; }

QJsonObject Transaction::toJson() const
{
    QJsonObject json;
    json["id"] = m_id;
    json["type"] = static_cast<int>(m_type);
    json["amount"] = m_amount;
    json["fromAccount"] = m_fromAccount;
    json["toAccount"] = m_toAccount;
    json["category"] = m_category;
    json["method"] = m_method;
    json["timestamp"] = m_timestamp.toString(Qt::ISODate);
    return json;
}

Transaction Transaction::fromJson(const QJsonObject& json)
{
    Transaction transaction;
    transaction.m_id = json["id"].toString();
    transaction.m_type = static_cast<TransactionType>(json["type"].toInt());
    transaction.m_amount = json["amount"].toDouble();
    transaction.m_fromAccount = json["fromAccount"].toString();
    transaction.m_toAccount = json["toAccount"].toString();
    transaction.m_category = json["category"].toString();
    transaction.m_method = json["method"].toString();
    transaction.m_timestamp = QDateTime::fromString(json["timestamp"].toString(), Qt::ISODate);
    return transaction;
}

QString Transaction::getTypeString() const
{
    return m_type == TransactionType::INCOME ? "收入" : "支出";
}

QString Transaction::getDisplayAmount() const
{
    QString sign = m_type == TransactionType::INCOME ? "+ " : "- ";
    return sign + QString("¥ %1").arg(m_amount, 0, 'f', 2);
}
