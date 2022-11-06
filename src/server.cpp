#include <qtmdns/dns.h>
#include <qtmdns/mdns.h>
#include <qtmdns/message.h>
#include <qtmdns/server.h>

#include <QtGlobal>

#include <QHostAddress>
#include <QLoggingCategory>
#include <QNetworkInterface>
#include <QTimer>
#include <QUdpSocket>

#ifdef Q_OS_UNIX
#  include <cerrno>
#  include <cstring>
#  include <sys/socket.h>
#endif

static Q_LOGGING_CATEGORY(log, "qtmdns.server");

namespace QtMdns {

class ServerPrivate : public QObject
{
    Q_DISABLE_COPY_MOVE(ServerPrivate)
    Q_DECLARE_PUBLIC(Server)
    Server * const q_ptr {nullptr};

    Q_OBJECT
public:
    explicit ServerPrivate(Server *server) :
        QObject(server),
        q_ptr(server)
    {
        connect(&timer, &QTimer::timeout, this, &ServerPrivate::onTimeout);
        connect(&ipv4Socket, &QUdpSocket::readyRead, this, &ServerPrivate::onReadyRead);
        connect(&ipv6Socket, &QUdpSocket::readyRead, this, &ServerPrivate::onReadyRead);

        timer.setInterval(60 * 1000);
        timer.setSingleShot(true);
        onTimeout();
    }

    bool bindSocket(QUdpSocket &socket, const QHostAddress &address)
    {
        // Exit early if the socket is already bound
        if (socket.state() == QAbstractSocket::BoundState)
            return true;

        // I cannot find the correct combination of flags that allows the socket
        // to bind properly on Linux, so on that platform, we must manually create
        // the socket and initialize the QUdpSocket with it

    #ifdef Q_OS_UNIX
        if (!socket.bind(address, MdnsPort, QAbstractSocket::ShareAddress)) {
            int arg = 1;
            if (setsockopt(socket.socketDescriptor(), SOL_SOCKET, SO_REUSEADDR,
                    reinterpret_cast<char*>(&arg), sizeof(int))) {
                emit q_ptr->error(strerror(errno));
                return false;
            }
    #endif
            if (!socket.bind(address, MdnsPort, QAbstractSocket::ReuseAddressHint)) {
                emit q_ptr->error(socket.errorString());
                return false;
            }

    #ifdef Q_OS_UNIX
        }
    #endif

        return true;
    }


    void onTimeout()
    {
        // A timer is used to run a set of operations once per minute; first, the
        // two sockets are bound - if this fails, another attempt is made once per
        // timeout; secondly, all network interfaces are enumerated; if the
        // interface supports multicast, the socket will join the mDNS multicast
        // groups

        bool const ipv4Bound = bindSocket(ipv4Socket, QHostAddress::AnyIPv4);
        bool const ipv6Bound = bindSocket(ipv6Socket, QHostAddress::AnyIPv6);

        if (ipv4Bound || ipv6Bound) {
            const auto interfaces = QNetworkInterface::allInterfaces();
            for (const QNetworkInterface &networkInterface : interfaces) {
                if (networkInterface.flags() & (QNetworkInterface::IsLoopBack | QNetworkInterface::IsPointToPoint))
                    continue;
                if ( ! (networkInterface.flags() & (QNetworkInterface::IsUp | QNetworkInterface::IsRunning | QNetworkInterface::CanMulticast)))
                    continue;

                if (ipv4Bound)
                    ipv4Socket.joinMulticastGroup(MdnsIpv4Address, networkInterface);

                if (ipv6Bound)
                    ipv6Socket.joinMulticastGroup(MdnsIpv6Address, networkInterface);
            }
        } else {
            qCWarning(log, "Socket bind failed.");
        }

        timer.start();
    }

    void onReadyRead()
    {
        // Read the packet from the socket
        QUdpSocket* socket = qobject_cast<QUdpSocket*>(sender());
        QByteArray packet;
        packet.resize(socket->pendingDatagramSize());

        QHostAddress address;
        quint16 port;
        socket->readDatagram(packet.data(), packet.size(), &address, &port);

        // Attempt to decode the packet
        Message message;
        if (fromPacket(packet, message)) {
            message.setAddress(address);
            message.setPort(port);

            emit q_ptr->messageReceived(message);
        }
    }

private:
    QTimer timer;
    QUdpSocket ipv4Socket;
    QUdpSocket ipv6Socket;
};



Server::Server(QObject* parent) :
    AbstractServer(parent),
    dd_ptr(new ServerPrivate(this))
{
}

Server::~Server()
{
}


void Server::sendMessage(const Message &message)
{
    Q_D(Server);
    QByteArray const packet = toPacket(message);

    if (message.address().protocol() == QAbstractSocket::IPv4Protocol) {
        d->ipv4Socket.writeDatagram(packet, message.address(), message.port());
    } else {
        d->ipv6Socket.writeDatagram(packet, message.address(), message.port());
    }
}

void Server::sendMessageToAll(const Message &message)
{
    Q_D(Server);
    QByteArray const packet = toPacket(message);

    foreach (QNetworkInterface interface, QNetworkInterface::allInterfaces()) {
        if (interface.flags() & (QNetworkInterface::IsLoopBack | QNetworkInterface::IsPointToPoint))
            continue;
        if ( ! (interface.flags() & (QNetworkInterface::IsUp | QNetworkInterface::IsRunning | QNetworkInterface::CanMulticast)))
            continue;

        d->ipv4Socket.setMulticastInterface(interface);
        d->ipv4Socket.writeDatagram(packet, MdnsIpv4Address, MdnsPort);

        d->ipv6Socket.setMulticastInterface(interface);
        d->ipv6Socket.writeDatagram(packet, MdnsIpv6Address, MdnsPort);
    }
}

} // namespace QtMdns

#include "server.moc"
