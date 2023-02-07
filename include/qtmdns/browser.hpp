#pragma once

#include "qtmdns_export.hpp"

#include <QByteArray>
#include <QObject>
#include <QScopedPointer>

namespace QtMdns {

class AbstractServer;
class Cache;
class Service;

class QTMDNS_EXPORT BrowserPrivate;

/**
 * @brief %Browser for local services
 *
 * This class provides a simple way to discover services on the local network.
 * A cache may be provided in the constructor to store records for future
 * queries.
 *
 * To browse for services of any type:
 *
 * @code
 * QtMdns::Browser browser(&server, QtMdns::MdnsBrowseType);
 * @endcode
 *
 * To browse for services of a specific type:
 *
 * @code
 * QtMdns::Browser browser(&server, "_http._tcp.local.");
 * @endcode
 *
 * When a service is found, the serviceAdded() signal is emitted:
 *
 * @code
 * connect(&browser, &QtMdns::Browser::serviceAdded, [](const QtMdns::Service &service) {
 *     qDebug() << "Service added:" << service.name();
 * });
 * @endcode
 *
 * The serviceUpdated() and serviceRemoved() signals are emitted when services
 * are updated (their properties change) or are removed, respectively.
 */
class QTMDNS_EXPORT Browser : public QObject
{
    Q_OBJECT
public:

    /**
     * @brief Create a new browser instance
     * @param server server to use for receiving and sending mDNS messages
     * @param type service type to browse for
     * @param cache DNS cache to use or null to create one
     * @param parent QObject
     */
    Browser(AbstractServer* server, QByteArray type, std::shared_ptr<Cache> cache, QObject* parent = nullptr);
    Browser(AbstractServer* server, QByteArray type, QObject* parent = nullptr);
    Browser(AbstractServer* server, std::shared_ptr<Cache> cache, QObject* parent = nullptr);
    Browser(AbstractServer* server, QObject* parent = nullptr);
    ~Browser() override;

    void startLookup(QByteArray type);

Q_SIGNALS:
    /**
     * @brief Indicate that a new service has been added
     *
     * This signal is emitted when the PTR and SRV records for a service are
     * received. If TXT records are received later, the serviceUpdated()
     * signal will be emitted.
     */
    void serviceAdded(const Service &service);

    /**
     * @brief Indicate that the specified service was updated
     *
     * This signal is emitted when the SRV record for a service (identified by
     * its name and type) or a TXT record has changed.
     */
    void serviceUpdated(const Service &service);

    /**
     * @brief Indicate that the specified service was removed
     *
     * This signal is emitted when an essential record (PTR or SRV) is
     * expiring from the cache. This will also occur when an updated PTR or
     * SRV record is received with a TTL of 0.
     */
    void serviceRemoved(const Service &service);

private:
    Q_DECLARE_PRIVATE_D(dd_ptr, Browser)
    QScopedPointer<BrowserPrivate> dd_ptr;
};

} // namespace QtMdns
