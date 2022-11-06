#pragma once

#include "qtmdns_export.hpp"

#include <QByteArray>
#include <QScopedPointer>

namespace QtMdns {

class QTMDNS_EXPORT QueryPrivate;

/**
 * @brief DNS query
 *
 * This class represents a query for a DNS record. For example, to query for
 * the IPv4 address of a local host:
 *
 * @code
 * QtMdns::Query query;
 * query.setName("myserver.local.");
 * query.setType(QtMdns::A);
 *
 * message.addQuery(query);
 * @endcode
 */
class QTMDNS_EXPORT Query
{
public:
    Query();
    Query(const Query &other);
    Query& operator=(const Query &other);
    ~Query();

    /**
     * @brief Retrieve the name being queried
     */
    QByteArray name() const;

    /**
     * @brief Set the name to query
     */
    void setName(const QByteArray &name);

    /**
     * @brief Retrieve the type of record being queried
     */
    quint16 type() const;

    /**
     * @brief Set the type of record to query
     *
     * Constants, such as QtMdns::SRV are provided for convenience.
     */
    void setType(quint16 type);

    /**
     * @brief Determine if a unicast response is desired
     */
    bool unicastResponse() const;

    /**
     * @brief Set whether a unicast response is desired
     */
    void setUnicastResponse(bool unicastResponse);

private:
    Q_DECLARE_PRIVATE_D(dd_ptr, Query)
    QScopedPointer<QueryPrivate> dd_ptr;
};

QTMDNS_EXPORT QDebug operator<<(QDebug dbg, const Query &query);

} // namespace QtMdns
