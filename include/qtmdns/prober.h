#pragma once

#include "qtmdns_export.hpp"

#include <QObject>
#include <QScopedPointer>

namespace QtMdns {

class AbstractServer;
class Record;

class QTMDNS_EXPORT ProberPrivate;

/**
 * @brief %Prober to confirm that a record is unique
 *
 * Before responding to queries for a record, its uniqueness on the network
 * must be confirmed. This class takes care of probing for existing records
 * that match and adjusts the record's name until a unique one is found.
 *
 * For example, to probe for a SRV record:
 *
 * @code
 * QtMdns::Record record;
 * record.setName("My Service._http._tcp.local.");
 * record.setType(QtMdns::SRV);
 * record.setPort(1234);
 * record.setTarget(hostname.hostname());
 *
 * QtMdns::Prober prober(&server, record);
 * connect(&prober, &QtMdns::Prober::nameConfirmed, [](const QByteArray &name) {
 *     qDebug() << "Name confirmed:" << name;
 * });
 * @endcode
 */
class QTMDNS_EXPORT Prober : public QObject
{
    Q_OBJECT
public:
    Prober(AbstractServer* server, QtMdns::Record record, QObject* parent = nullptr);
    ~Prober() override;

Q_SIGNALS:
    /**
     * @brief Indicate that the name has been confirmed unique
     * @param name that was confirmed to be unique
     */
    void nameConfirmed(const QByteArray &name);

private:
    Q_DECLARE_PRIVATE_D(dd_ptr, Prober)
    QScopedPointer<ProberPrivate> dd_ptr;
};

} // namespace QtMdns
