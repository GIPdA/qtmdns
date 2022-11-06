#pragma once

#include "qtmdns_export.hpp"

#include <QObject>
#include <QScopedPointer>

namespace QtMdns {

class AbstractServer;

class QTMDNS_EXPORT HostnamePrivate;

/**
 * @brief %Hostname reserved for exclusive use
 *
 * In order to provide services on the local network, a unique hostname must
 * be used. This class asserts a hostname (by first confirming that it is not
 * in use) and then responds to A and AAAA queries for the hostname.
 *
 * @code
 * QtMdns::Hostname hostname(&server);
 *
 * connect(&hostname, &QtMdns::Hostname::hostnameChanged, [](const QByteArray &hostname) {
 *     qDebug() << "New hostname:" << hostname;
 * });
 * @endcode
 */
class QTMDNS_EXPORT Hostname : public QObject
{
    Q_OBJECT
public:
    Hostname(AbstractServer* server, QObject* parent = nullptr);
    ~Hostname() override;

    /**
     * @brief Determine if a hostname has been registered
     *
     * A hostname is not considered registered until a probe for the desired
     * name has been completed and no matching records were received.
     */
    bool isRegistered() const;

    /**
     * @brief Retrieve the current hostname
     *
     * This value is only valid when isRegistered() returns true.
     */
    QByteArray hostname() const;

Q_SIGNALS:

    /**
     * @brief Indicate that the current hostname has changed
     * @param hostname new hostname
     */
    void hostnameChanged(const QByteArray &hostname);

private:
    Q_DECLARE_PRIVATE_D(dd_ptr, Hostname)
    QScopedPointer<HostnamePrivate> dd_ptr;
};

} // namespace QtMdns
