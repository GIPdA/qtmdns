#pragma once

#include "qtmdns_export.hpp"

#include <QObject>
#include <QScopedPointer>

namespace QtMdns {

class AbstractServer;
class Hostname;
class Service;

class QTMDNS_EXPORT ProviderPrivate;

/**
 * @brief %Provider for a single mDNS service
 *
 * This class provide a [Service](@ref QtMdns::Service) on the local
 * network by responding to the appropriate DNS queries. A hostname is
 * required for creating the SRV record.
 *
 * The provider needs to be given a reference to the service through the
 * update() method so that it can construct DNS records:
 *
 * @code
 * QtMdns::Service service;
 * service.setType("_http._tcp.local.");
 * service.setName("My Service");
 * service.setPort(1234);
 *
 * QtMdns::Provider provider;
 * provider.update(service);
 * @endcode
 *
 * This method can also be used to update the provider's records.
 */
class QTMDNS_EXPORT Provider : public QObject
{
    Q_OBJECT
public:
    Provider(AbstractServer* server, Hostname* hostname, QObject* parent = nullptr);
    ~Provider() override;

    /**
     * @brief Update the service with the provided information
     * @param service updated service description
     *
     * This class will not respond to any DNS queries until the hostname is
     * confirmed and this method is called.
     */
    void update(const Service &service);

private:
    Q_DECLARE_PRIVATE_D(dd_ptr, Provider)
    QScopedPointer<ProviderPrivate> dd_ptr;
};

} // namespace QtMdns
