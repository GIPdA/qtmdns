#pragma once

#include "qtmdns_export.hpp"

#include <QHostAddress>
#include <QObject>
#include <QScopedPointer>

namespace QtMdns {

class AbstractServer;
class Cache;

class QTMDNS_EXPORT ResolverPrivate;

/**
 * @brief %Resolver for services
 *
 * When [Browser](@ref QtMdns::Browser) indicates that a new service has
 * been found, it becomes necessary to resolve the service in order to connect
 * to it. This class serves that role. A [Cache](@ref QtMdns::Cache) can
 * optionally be provided to speed up the resolving process.
 *
 * For example, assuming that `record` is a SRV record:
 *
 * @code
 * QtMdns::Resolver resolver(&server, record.target());
 * connect(&resolver, &QtMdns::Resolver::resolved, [](const QHostAddress &address) {
 *     qDebug() << "Address:" << address;
 * });
 * @endcode
 */
class QTMDNS_EXPORT Resolver : public QObject
{
    Q_OBJECT
public:
    Resolver(AbstractServer* server, const QByteArray &name, std::shared_ptr<Cache> cache, QObject* parent = nullptr);
    Resolver(AbstractServer* server, const QByteArray &name, QObject* parent = nullptr);
    ~Resolver() override;

Q_SIGNALS:
    /**
     * @brief Indicate that the host resolved to an address
     * @param address service address
     *
     * This signal will be emitted once for each resolved address. For
     * example, if a host provides both A and AAAA records, this signal will
     * be emitted twice.
     */
    void resolved(const QHostAddress &address);

private:
    Q_DECLARE_PRIVATE_D(dd_ptr, Resolver)
    QScopedPointer<ResolverPrivate> dd_ptr;
};

} // namespace QtMdns
