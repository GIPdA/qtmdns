#pragma once

#include <QObject>

#include "qtmdns_export.hpp"

namespace QtMdns {

class Message;

/**
 * @brief Base class for sending and receiving DNS messages
 *
 * Many of the other classes in this library require the ability to send and
 * receive DNS messages. By having them use this base class, they become far
 * easier to test. Any class derived from this one that implements the pure
 * virtual methods can be used for sending and receiving DNS messages.
 */
class QTMDNS_EXPORT AbstractServer : public QObject
{
    Q_OBJECT
public:
    explicit AbstractServer(QObject* parent = nullptr);
    ~AbstractServer() override;

    /**
     * @brief Send a message to its provided destination
     *
     * The message should be sent over the IP protocol specified in the
     * message and to the target address and port specified in the message.
     */
    virtual void sendMessage(Message const& message) = 0;

    /**
     * @brief Send a message to the multicast address on each interface
     *
     * The message should be sent over both IPv4 and IPv6 on all interfaces.
     */
    virtual void sendMessageToAll(Message const& message) = 0;

Q_SIGNALS:

    /**
     * @brief Indicate that a DNS message was received
     * @param message newly received message
     */
    void messageReceived(Message const& message);

    /**
     * @brief Indicate that an error has occurred
     * @param message brief description of the error
     */
    void error(QString const& message);
};

} // namespace QtMdns
