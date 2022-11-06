#pragma once

#include "qtmdns_export.hpp"

#include <qtmdns/abstractserver.h>

#include <QObject>
#include <QScopedPointer>

namespace QtMdns {

class Message;

class QTMDNS_EXPORT ServerPrivate;

/**
 * @brief mDNS server
 *
 * This class provides an implementation of
 * [AbstractServer](@ref QtMdns::AbstractServer) that uses all available
 * local network adapters to send and receive mDNS messages.
 *
 * The class takes care of watching for the addition and removal of network
 * interfaces, automatically joining multicast groups when new interfaces are
 * available.
 */
class QTMDNS_EXPORT Server : public AbstractServer
{
    Q_OBJECT
public:
    explicit Server(QObject* parent = nullptr);
    ~Server() override;

    void sendMessage(const Message &message) override;
    void sendMessageToAll(const Message &message) override;

private:
    Q_DECLARE_PRIVATE_D(dd_ptr, Server)
    QScopedPointer<ServerPrivate> dd_ptr;
};

} // namespace QtMdns
