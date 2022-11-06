#pragma once

#include "qtmdns_export.hpp"

#include <QByteArray>
#include <QHostAddress>
#include <QList>
#include <QMap>
#include <QScopedPointer>

namespace QtMdns {

class QTMDNS_EXPORT ServicePrivate;

/**
 * @brief %Service available on the local network
 *
 * This class contains the descriptive information necessary to represent an
 * individual service made available to the local network. Instances are
 * provided by [Browser](@ref QtMdns::Browser) as services are
 * discovered. Instances must be created and passed to
 * [Provider::update()](@ref QtMdns::Provider::update) to provide a
 * service.
 */
class QTMDNS_EXPORT Service
{
public:

    /**
     * @brief Create an uninitialized service
     */
    Service();
    Service(const Service &other);
    Service& operator=(const Service &other);
    bool operator==(const Service &other) const;
    bool operator!=(const Service &other) const;
    ~Service();

    QByteArray type() const;

    /**
     * @brief Set the service type
     *
     * For example, an HTTP service might use "_http._tcp".
     */
    void setType(const QByteArray &type);


    QByteArray name() const;

    /**
     * @brief Set the service name
     *
     * This is combined with the service type and domain to form the FQDN for
     * the service.
     */
    void setName(const QByteArray &name);

    /**
     * @brief Retrieve the hostname of the device providing the service
     */
    QByteArray hostname() const;

    /**
     * @brief Set the hostname of the device providing the service
     */
    void setHostname(const QByteArray &hostname);

    quint16 port() const;
    void setPort(quint16 port);

    /**
     * @brief Retrieve the attributes for the service
     *
     * Boolean attributes will have null values (invoking QByteArray::isNull()
     * on the value will return true).
     */
    QMap<QByteArray, QByteArray> attributes() const;

    void setAttributes(const QMap<QByteArray, QByteArray> &attributes);
    void addAttribute(const QByteArray &key, const QByteArray &value);

private:
    Q_DECLARE_PRIVATE_D(dd_ptr, Service)
    QScopedPointer<ServicePrivate> dd_ptr;
};

QTMDNS_EXPORT QDebug operator<<(QDebug debug, const Service &service);

} // namespace QtMdns
