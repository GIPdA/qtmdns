#pragma once

#include "qtmdns_export.hpp"

#include <QByteArray>
#include <QHostAddress>
#include <QMap>
#include <QScopedPointer>

#include <qtmdns/bitmap.hpp>

namespace QtMdns {

class QTMDNS_EXPORT RecordPrivate;

/**
 * @brief DNS record
 *
 * This class maintains information for an individual record. Not all record
 * types use every field.
 *
 * For example, to create a TXT record:
 *
 * @code
 * QtMdns::Record record;
 * record.setName("My Service._http._tcp.local.");
 * record.setType(QtMdns::TXT);
 * record.addAttribute("a", "value1");
 * record.addAttribute("b", "value2");
 *
 * message.addRecord(record);
 * @endcode
 */
class QTMDNS_EXPORT Record
{
public:
    Record();
    Record(const Record &other);
    Record &operator=(const Record &other);
    ~Record();

    bool operator==(const Record &other) const;
    bool operator!=(const Record &other) const;

    /**
     * @brief Retrieve the name of the record
     */
    QByteArray name() const;

    /**
     * @brief Set the name of the record
     */
    void setName(const QByteArray &name);

    /**
     * @brief Retrieve the type of the record
     */
    quint16 type() const;

    /**
     * @brief Set the type of the record
     *
     * For convenience, constants for types used by mDNS, such as
     * QtMdns::A or QtMdns::PTR, may be used here.
     */
    void setType(quint16 type);

    /**
     * @brief Determine whether to replace or append to existing records
     *
     * If true, this record replaces all previous records of the same name and
     * type rather than appending to them.
     */
    bool flushCache() const;

    /**
     * @brief Set whether to replace or append to existing records
     */
    void setFlushCache(bool flushCache);

    /**
     * @brief Retrieve the TTL (time to live) for the record
     */
    quint32 ttl() const;

    /**
     * @brief Set the TTL (time to live) for the record
     */
    void setTtl(quint32 ttl);

    /**
     * @brief Retrieve the address for the record
     *
     * This field is used by QtMdns::A and QtMdns::AAAA records.
     */
    QHostAddress address() const;

    /**
     * @brief Set the address for the record
     */
    void setAddress(const QHostAddress &address);

    /**
     * @brief Retrieve the target for the record
     *
     * This field is used by QtMdns::PTR and QtMdns::SRV records.
     */
    QByteArray target() const;

    /**
     * @brief Set the target for the record
     */
    void setTarget(const QByteArray &target);

    /**
     * @brief Retrieve the next domain name
     *
     * This field is used by QtMdns::NSEC records.
     */
    QByteArray nextDomainName() const;

    /**
     * @brief Set the next domain name
     */
    void setNextDomainName(const QByteArray &nextDomainName);

    /**
     * @brief Retrieve the priority for the record
     *
     * This field is used by QtMdns::SRV records.
     */
    quint16 priority() const;

    /**
     * @brief Set the priority for the record
     *
     * Unless more than one QtMdns::SRV record is being sent, this field
     * should be set to 0.
     */
    void setPriority(quint16 priority);

    /**
     * @brief Retrieve the weight of the record
     *
     * This field is used by QtMdns::SRV records.
     */
    quint16 weight() const;

    /**
     * @brief Set the weight of the record
     *
     * Unless more than one QtMdns::SRV record is being sent, this field
     * should be set to 0.
     */
    void setWeight(quint16 weight);

    /**
     * @brief Retrieve the port for the record
     *
     * This field is used by QtMdns::SRV records.
     */
    quint16 port() const;

    /**
     * @brief Set the port for the record
     */
    void setPort(quint16 port);

    /**
     * @brief Retrieve attributes for the record
     *
     * This field is used by QtMdns::TXT records.
     */
    QMap<QByteArray, QByteArray> attributes() const;

    /**
     * @brief Set attributes for the record
     */
    void setAttributes(const QMap<QByteArray, QByteArray> &attributes);

    /**
     * @brief Add an attribute to the record
     */
    void addAttribute(const QByteArray &key, const QByteArray &value);

    /**
     * @brief Retrieve the bitmap for the record
     *
     * This field is used by QtMdns::NSEC records.
     */
    Bitmap bitmap() const;

    /**
     * @brief Set the bitmap for the record
     */
    void setBitmap(const Bitmap &bitmap);

private:
    Q_DECLARE_PRIVATE_D(dd_ptr, Record)
    QScopedPointer<RecordPrivate> dd_ptr;
};

QTMDNS_EXPORT QDebug operator<<(QDebug dbg, const Record &record);

} // namespace QtMdns
