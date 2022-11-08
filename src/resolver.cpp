#include <qtmdns/abstractserver.hpp>
#include <qtmdns/dns.hpp>
#include <qtmdns/cache.hpp>
#include <qtmdns/message.hpp>
#include <qtmdns/query.hpp>
#include <qtmdns/record.hpp>
#include <qtmdns/resolver.hpp>

#include <QHostAddress>
#include <QObject>
#include <QPointer>
#include <QSet>
#include <QTimer>

namespace QtMdns {

class ResolverPrivate : public QObject
{
    Q_DISABLE_COPY_MOVE(ResolverPrivate)
    Q_DECLARE_PUBLIC(Resolver)
    Resolver * const q_ptr {nullptr};

public:
    ResolverPrivate(Resolver* resolver, AbstractServer* server, const QByteArray &name, std::shared_ptr<Cache> cache) :
        QObject(resolver),
        q_ptr(resolver),
        server(server),
        name(name),
        cache(cache ? std::move(cache) : std::make_shared<Cache>())
    {
        connect(server, &AbstractServer::messageReceived, this, &ResolverPrivate::onMessageReceived);
        connect(&timer, &QTimer::timeout, this, &ResolverPrivate::onTimeout);

        // Query for new records
        query();

        // Pull the existing records from the cache
        timer.setSingleShot(true);
        timer.start(0);
    }

    QList<Record> existing() const
    {
        QList<Record> records;
        cache->lookupRecords(name, A, records);
        cache->lookupRecords(name, AAAA, records);
        return records;
    }

    void query() const
    {
        // Add a query for A and AAAA records
        Query query;
            query.setName(name);
            query.setType(A);

        Message message;
            message.addQuery(query);
                query.setType(AAAA);
            message.addQuery(query);

        // Add existing (known) records to the query
        const auto records = existing();
        for (const Record &record : records) {
            message.addRecord(record);
        }

        // Send the query
        server->sendMessageToAll(message);
    }


    void onMessageReceived(const Message &message)
    {
        if ( ! message.isResponse())
            return;

        const auto records = message.records();
        for (const Record &record : records) {
            if (record.name() == name && (record.type() == A || record.type() == AAAA)) {
                cache->addRecord(record);
                if ( ! addresses.contains(record.address())) {
                    emit q_ptr->resolved(record.address());
                    addresses.insert(record.address());
                }
            }
        }
    }

    void onTimeout()
    {
        const auto records = existing();
        for (const Record &record : records) {
            emit q_ptr->resolved(record.address());
        }
    }

private:
    QPointer<AbstractServer> server;
    QByteArray name;
    std::shared_ptr<Cache> cache;
    QSet<QHostAddress> addresses;
    QTimer timer;
};


Resolver::Resolver(AbstractServer* server, const QByteArray &name, std::shared_ptr<Cache> cache, QObject* parent) :
    QObject(parent),
    dd_ptr(new ResolverPrivate(this, server, name, std::move(cache)))
{
}

Resolver::Resolver(AbstractServer* server, const QByteArray &name, QObject* parent) :
    QObject(parent),
    dd_ptr(new ResolverPrivate(this, server, name, nullptr))
{
}

Resolver::~Resolver()
{
}

} // namespace QtMdns
