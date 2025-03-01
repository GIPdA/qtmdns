#include <qtmdns/abstractserver.hpp>
#include <qtmdns/dns.hpp>
#include <qtmdns/hostname.hpp>
#include <qtmdns/message.hpp>
#include <qtmdns/query.hpp>
#include <qtmdns/record.hpp>

#include <QHostAddress>
#include <QHostInfo>
#include <QNetworkAddressEntry>
#include <QNetworkInterface>
#include <QObject>
#include <QPointer>
#include <QTimer>

namespace QtMdns {

class HostnamePrivate
{
    Q_DISABLE_COPY_MOVE(HostnamePrivate)
    Q_DECLARE_PUBLIC(Hostname)
    Hostname * const q_ptr {nullptr};

public:
    HostnamePrivate(Hostname* hostname, AbstractServer* server, QByteArray wantedName) :
        q_ptr(hostname),
        server(server),
        wantedHostname(std::move(wantedName))
    {
        QObject::connect(server, &AbstractServer::messageReceived, hostname,
                         [&](Message const& message) { onMessageReceived(message); });
        QObject::connect(&registrationTimer, &QTimer::timeout, hostname,
                         [&]() { onRegistrationTimeout(); });
        QObject::connect(&rebroadcastTimer, &QTimer::timeout, hostname,
                         [&]() { onRebroadcastTimeout(); });

        if (qsizetype const idx = wantedHostname.lastIndexOf(".local"); idx > 0)
            wantedHostname.truncate(idx);

        registrationTimer.setInterval(2 * 1000);
        registrationTimer.setSingleShot(true);

        rebroadcastTimer.setInterval(30 * 60 * 1000);
        rebroadcastTimer.setSingleShot(true);

        // Immediately assert the hostname
        onRebroadcastTimeout();
    }

    void assertHostname()
    {
        // Begin with the local hostname and replace any "." with "-" (I'm looking
        // at you, macOS)
        QByteArray localHostname = wantedHostname;
        localHostname = localHostname.replace('.', '-');

        Q_ASSERT( ! localHostname.isEmpty());

        // If the suffix > 1, then append a "-2", "-3", etc. to the hostname to
        // aid in finding one that is unique and not in use
        hostname = (hostnameSuffix == 1
                    ? localHostname
                    : localHostname + "-" + QByteArray::number(hostnameSuffix)) + ".local.";

        // Compose a query for A and AAAA records matching the hostname
        Query ipv4Query;
            ipv4Query.setName(hostname);
            ipv4Query.setType(A);

        Query ipv6Query;
            ipv6Query.setName(hostname);
            ipv6Query.setType(AAAA);

        Message message;
            message.addQuery(ipv4Query);
            message.addQuery(ipv6Query);

        server->sendMessageToAll(message);

        // If no reply is received after two seconds, the hostname is available
        registrationTimer.start();
    }

    std::optional<Record> generateRecord(QHostAddress const& srcAddress, quint16 type)
    {
        // Attempt to find the interface that corresponds with the provided
        // address and determine this device's address from the interface

        const auto interfaces = QNetworkInterface::allInterfaces();
        for (QNetworkInterface const& networkInterface : interfaces) {
            const auto entries = networkInterface.addressEntries();
            for (QNetworkAddressEntry const& entry : entries) {
                if (srcAddress.isInSubnet(entry.ip(), entry.prefixLength())) {
                    for (QNetworkAddressEntry const& newEntry : entries) {
                        QHostAddress address = newEntry.ip();
                        if ((address.protocol() == QAbstractSocket::IPv4Protocol && type == A) ||
                            (address.protocol() == QAbstractSocket::IPv6Protocol && type == AAAA))
                        {
                            Record record;
                            record.setName(hostname);
                            record.setType(type);
                            record.setAddress(address);
                            return record;
                        }
                    }
                }
            }
        }
        return std::nullopt;
    }


    void onMessageReceived(Message const& message)
    {
        if (message.isResponse()) {
            if (hostnameRegistered)
                return;

            const auto records = message.records();
            for (const Record &record : records) {
                if ((record.type() == A || record.type() == AAAA) && record.name() == hostname) {
                    ++hostnameSuffix;
                    assertHostname();
                }
            }
        } else {
            if ( ! hostnameRegistered)
                return;

            Message reply;
            reply.reply(message);

            const auto queries = message.queries();
            reply.addQueries(queries);

            for (Query const& query : queries) {
                if ((query.type() == A || query.type() == AAAA) && query.name() == hostname) {
                    if (auto record = generateRecord(message.address(), query.type()); record)
                        reply.addRecord(*record);
                }
            }

            if (reply.records().count())
                server->sendMessage(reply);
        }
    }

    void onRegistrationTimeout()
    {
        hostnameRegistered = true;
        if (hostname != hostnamePrev)
            emit q_ptr->hostnameChanged(hostname);

        // Re-assert the hostname in half an hour
        rebroadcastTimer.start();
    }

    void onRebroadcastTimeout()
    {
        hostnamePrev = hostname;
        hostnameRegistered = false;
        hostnameSuffix = 1;

        assertHostname();
    }

private:
    QPointer<AbstractServer> server;

    QByteArray wantedHostname;
    QByteArray hostnamePrev;
    QByteArray hostname;
    bool hostnameRegistered {false};
    int hostnameSuffix {0};

    QTimer registrationTimer;
    QTimer rebroadcastTimer;
};


Hostname::Hostname(AbstractServer* server, QObject* parent) :
    QObject(parent),
    dd_ptr(new HostnamePrivate(this, server, QHostInfo::localHostName().toUtf8()))
{}

Hostname::Hostname(AbstractServer* server, QByteArray hostname, QObject* parent) :
    QObject(parent),
    dd_ptr(new HostnamePrivate(this, server, std::move(hostname)))
{}

Hostname::~Hostname()
{}


bool Hostname::isRegistered() const
{
    Q_D(const Hostname);
    return d->hostnameRegistered;
}

QByteArray Hostname::hostname() const
{
    Q_D(const Hostname);
    return d->hostname;
}

} // namespace QtMdns
