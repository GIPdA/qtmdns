#pragma once

#include "qtmdns_export.hpp"

#include <QHostAddress>
#include <QList>
#include <QScopedPointer>

namespace QtMdns {

class Query;
class Record;

class QTMDNS_EXPORT MessagePrivate;

/**
 * @brief DNS message
 *
 * A DNS message consists of a header and zero or more queries and records.
 * Instances of this class are created and initialized by
 * [AbstractServer](@ref QtMdns::AbstractServer) when messages are
 * received from the network.
 *
 * If a message is being constructed in reply to one received from the
 * network, the reply() method can be used to simplify initialization:
 *
 * @code
 * connect(&server, &QtMdns::Server::messageReceived, [](const QtMdns::Message &message) {
 *     QtMdns::Message reply;
 *     reply.reply(message);
 *     server.sendMessage(reply);
 * });
 * @endcode
 */
class QTMDNS_EXPORT Message
{
public:
    Message();
    Message(const Message &other);
    Message& operator=(const Message &other);
    ~Message();

    /**
     * @brief Retrieve the address for the message
     *
     * When receiving messages, this is the address that the message was
     * received from.
     */
    QHostAddress address() const;

    /**
     * @brief Set the address for the message
     *
     * When sending messages, this is the address that the message will be
     * sent to. QtMdns::MdnsIpv4Address and QtMdns::MdnsIpv6Address
     * can be used for mDNS messages.
     */
    void setAddress(const QHostAddress &address);

    /**
     * @brief Retrieve the port for the message
     *
     * When receiving messages, this is the port that the message was received
     * from. For traditional queries, this will be an ephemeral port. For mDNS
     * queries, this will always equal QtMdns::MdnsPort.
     */
    quint16 port() const;

    /**
     * @brief Set the port for the message
     *
     * When sending messages, this is the port that the message will be sent
     * to. This should be set to QtMdns::MdnsPort unless the message is a
     * reply to a traditional DNS query.
     */
    void setPort(quint16 port);

    /**
     * @brief Retrieve the transaction ID for the message
     *
     * This is always set to 1 for mDNS messages. Traditional DNS queries may
     * set this to an arbitrary integer.
     */
    quint16 transactionId() const;

    /**
     * @brief Set the transaction ID for the message
     *
     * The default transaction ID is 0. This value should not be changed
     * unless responding to a traditional DNS query.
     */
    void setTransactionId(quint16 transactionId);

    /**
     * @brief Determine if the message is a response
     */
    bool isResponse() const;

    /**
     * @brief Set whether the message is a response
     */
    void setResponse(bool isResponse);

    /**
     * @brief Determine if the message is truncated
     */
    bool isTruncated() const;

    /**
     * @brief Set whether the message is truncated
     */
    void setTruncated(bool isTruncated);

    /**
     * @brief Retrieve a list of queries in the message
     */
    QList<Query> queries() const;

    /**
     * @brief Add a query to the message
     */
    void addQuery(const Query &query);

    /**
     * @brief Retrieve a list of records in the message
     */
    QList<Record> records() const;

    /**
     * @brief Add a record to the message
     */
    void addRecord(const Record &record);

    /**
     * @brief Reply to another message
     *
     * The message will be correctly initialized to respond to the other
     * message. This includes setting the target address, port, and
     * transaction ID.
     */
    void reply(const Message &other);

private:
    Q_DECLARE_PRIVATE_D(dd_ptr, Message)
    QScopedPointer<MessagePrivate> dd_ptr;
};

} // namespace QtMdns
