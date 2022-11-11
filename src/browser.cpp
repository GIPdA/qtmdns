#include <qtmdns/abstractserver.hpp>
#include <qtmdns/browser.hpp>
#include <qtmdns/cache.hpp>
#include <qtmdns/dns.hpp>
#include <qtmdns/mdns.hpp>
#include <qtmdns/message.hpp>
#include <qtmdns/query.hpp>
#include <qtmdns/record.hpp>
#include <qtmdns/service.hpp>

#include <QByteArray>
#include <QMap>
#include <QObject>
#include <QPointer>
#include <QSet>
#include <QTimer>

namespace QtMdns {

class BrowserPrivate : public QObject
{
    Q_DISABLE_COPY_MOVE(BrowserPrivate)
    Q_DECLARE_PUBLIC(Browser)
    Browser * const q_ptr {nullptr};

public:
    explicit BrowserPrivate(Browser* browser, AbstractServer* server, QByteArray _type, std::shared_ptr<Cache> existingCache) :
        QObject(browser),
        q_ptr(browser),
        server(server),
        type(std::move(_type)),
        cache(existingCache ? std::move(existingCache) : std::make_shared<Cache>())
    {
        connect(server, &AbstractServer::messageReceived, this, &BrowserPrivate::onMessageReceived);
        connect(cache.get(), &Cache::shouldQuery, this, &BrowserPrivate::onShouldQuery);
        connect(cache.get(), &Cache::recordExpired, this, &BrowserPrivate::onRecordExpired);
        connect(&queryTimer, &QTimer::timeout, this, &BrowserPrivate::onQueryTimeout);
        connect(&serviceTimer, &QTimer::timeout, this, &BrowserPrivate::onServiceTimeout);

        queryTimer.setInterval(60 * 1000);
        queryTimer.setSingleShot(true);

        serviceTimer.setInterval(100);
        serviceTimer.setSingleShot(true);

        // Immediately begin browsing for services
        if ( ! type.isEmpty())
            start();
    }

    void start()
    {
        onQueryTimeout();
    }
    void stop()
    {
        queryTimer.stop();
        serviceTimer.stop();
    }

    // TODO: multiple SRV records not supported
    bool updateService(QByteArray const& fqName)
    {
        // Split the FQDN into service name and type
        qsizetype const index = fqName.indexOf("._");
        QByteArray const serviceName = fqName.left(index);
        QByteArray const serviceType = fqName.mid(index + 1);

        // Immediately return if a PTR record does not exist
        Record ptrRecord;
        if (!cache->lookupRecord(serviceType, PTR, ptrRecord))
            return false;

        // If a SRV record is missing, query for it (by returning true)
        Record srvRecord;
        if (!cache->lookupRecord(fqName, SRV, srvRecord))
            return true;

        Record aRecord;
        cache->lookupRecord(srvRecord.target(), A, aRecord);
        Record aaaaRecord;
        cache->lookupRecord(srvRecord.target(), AAAA, aaaaRecord);

        Service service;
        service.setName(serviceName);
        service.setType(serviceType);
        service.setHostname(srvRecord.target());
        service.setPort(srvRecord.port());
        service.setHostAddress(aRecord.address());
        service.setHostAddressIPv6(aaaaRecord.address());

        // If TXT records are available for the service, add their values
        QList<Record> txtRecords;
        if (cache->lookupRecords(fqName, TXT, txtRecords)) {
            QMap<QByteArray, QByteArray> attributes;
            for (const Record &record : qAsConst(txtRecords)) {
                for (auto i = record.attributes().constBegin();
                        i != record.attributes().constEnd(); ++i) {
                    attributes.insert(i.key(), i.value());
                }
            }
            service.setAttributes(attributes);
        }

        // If the service existed, this is an update; otherwise it is a new
        // addition; emit the appropriate signal
        if (!services.contains(fqName)) {
            emit q_ptr->serviceAdded(service);
        } else if (services.value(fqName) != service) {
            emit q_ptr->serviceUpdated(service);
        }

        services.insert(fqName, service);
        return false;
    }


    void onMessageReceived(const Message &message)
    {
        //! Remove sub domain
        auto getLocalDomain = [](QByteArray const& name) -> QByteArray {
            if (qsizetype const i = name.indexOf("._sub"); i >= 0)
                return name.mid(i+5/*strlen(._sub)*/+1);
            return name;
        };

        if ( ! message.isResponse())
            return;

        bool const any = (type == mdnsDefaults().MdnsBrowseType);
        QByteArray const localType = getLocalDomain(type);

        // Use a set to track all services that are updated in the message to
        // prevent unnecessary queries for SRV and TXT records
        QSet<QByteArray> updateNames;
        QList<Record> const records = message.records();
        for (Record const& record : records) {
            bool cacheRecord = false;

            switch (record.type()) {
            case PTR:
                if (any && record.name() == mdnsDefaults().MdnsBrowseType) {
                    ptrTargets.insert(record.target());
                    serviceTimer.start();
                    cacheRecord = true;
                } else if (any || record.name() == type) {
                    updateNames.insert(record.target());
                    cacheRecord = true;
                }
                break;
            case SRV:
            case TXT:
                if (any || record.name().endsWith("." + localType)) {
                    updateNames.insert(record.name());
                    if (record.type() == SRV)
                        hostnames.insert(record.target());
                    cacheRecord = true;
                }
                break;
            default:
                break;
            }

            if (cacheRecord)
                cache->addRecord(record);
        }

        // Cache A / AAAA records after services are processed to ensure hostnames are known
        for (const Record &record : records) {
            bool cacheRecord = false;

            switch (record.type()) {
            case A:
            case AAAA:
                cacheRecord = hostnames.contains(record.name());
                break;
            default:
                break;
            }

            if (cacheRecord)
                cache->addRecord(record);
        }

        // For each of the services marked to be updated, perform the update and
        // make a list of all missing SRV records
        QSet<QByteArray> queryNames;
        for (const QByteArray &name : qAsConst(updateNames)) {
            if (updateService(name)) {
                queryNames.insert(name);
            }
        }

        // Build and send a query for all of the SRV and TXT records
        if (queryNames.count()) {
            Message queryMessage;
            for (const QByteArray &name : qAsConst(queryNames)) {
                Query query;
                query.setName(name);
                query.setType(SRV);
                queryMessage.addQuery(query);
                query.setType(TXT);
                queryMessage.addQuery(query);
            }
            server->sendMessageToAll(queryMessage);
        }
    }

    void onShouldQuery(const Record &record)
    {
        // Assume that all messages in the cache are still in use (by the browser)
        // and attempt to renew them immediately

        Query query;
            query.setName(record.name());
            query.setType(record.type());

        Message message;
            message.addQuery(query);

        server->sendMessageToAll(message);
    }

    void onRecordExpired(const Record &record)
    {
        // If the SRV record has expired for a service, then it must be
        // removed - TXT records on the other hand, cause an update

        QByteArray serviceName;
        switch (record.type()) {
        case SRV:
            serviceName = record.name();
            break;
        case TXT:
            updateService(record.name());
            return;

        default:
            return;
        }

        Service const service = services.value(serviceName);
        if ( ! service.name().isNull()) {
            emit q_ptr->serviceRemoved(service);
            services.remove(serviceName);
            updateHostnames();
        }
    }

    void onQueryTimeout()
    {
        if (type.isEmpty())
            return;

        Query query;
            query.setName(type);
            query.setType(PTR);

        Message message;
            message.addQuery(query);

        // TODO: including too many records could cause problems

        // Include PTR records for the target that are already known
        QList<Record> records;
        if (cache->lookupRecords(query.name(), PTR, records)) {
            for (const Record &record : qAsConst(records)) {
                message.addRecord(record);
            }
        }

        server->sendMessageToAll(message);
        queryTimer.start();
    }

    void onServiceTimeout()
    {
        if (ptrTargets.count()) {
            Message message;
            for (const QByteArray &target : qAsConst(ptrTargets)) {
                // Add a query for PTR records
                Query query;
                    query.setName(target);
                    query.setType(PTR);

                message.addQuery(query);

                // Include PTR records for the target that are already known
                QList<Record> records;
                if (cache->lookupRecords(target, PTR, records)) {
                    for (const Record &record : qAsConst(records)) {
                        message.addRecord(record);
                    }
                }
            }

            server->sendMessageToAll(message);
            ptrTargets.clear();
        }
    }

private:
    void updateHostnames()
    {
        hostnames.clear();
        for (Service const& service : qAsConst(services)) {
            hostnames.insert(service.hostname());
        }
    }

    QPointer<AbstractServer> server;
    QByteArray type;

    std::shared_ptr<Cache> cache;
    QSet<QByteArray> ptrTargets;
    QMap<QByteArray, Service> services;
    QSet<QByteArray> hostnames;

    QTimer queryTimer;
    QTimer serviceTimer;
};


Browser::Browser(AbstractServer* server, QByteArray type, std::shared_ptr<Cache> cache, QObject* parent) :
    QObject(parent),
    dd_ptr(new BrowserPrivate(this, server, std::move(type), std::move(cache)))
{}

Browser::Browser(AbstractServer* server, QByteArray type, QObject* parent) :
    QObject(parent),
    dd_ptr(new BrowserPrivate(this, server, std::move(type), nullptr))
{}

Browser::Browser(AbstractServer* server, QObject* parent) :
    QObject(parent),
    dd_ptr(new BrowserPrivate(this, server, {}, nullptr))
{
}

Browser::~Browser()
{
}

void Browser::startLookup(QByteArray type)
{
    Q_D(Browser);
    if (d->type == type)
        return;

    bool const needStart = d->type.isEmpty();
    d->type = type;

    // TODO: cleanup?

    if (d->type.isEmpty())
        d->stop();
    else if (needStart)
        d->start();
}

} // namespace QtMdns
