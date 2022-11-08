#pragma once

#include "qtmdns_export.hpp"

#include <QByteArray>
#include <QMap>

namespace QtMdns {

class Message;
class Record;

enum {
    /// IPv4 address record
    A = 1,
    /// IPv6 address record
    AAAA = 28,
    /// Wildcard for cache lookups
    ANY = 255,
    /// List of records
    NSEC = 47,
    /// Pointer to hostname
    PTR = 12,
    /// %Service information
    SRV = 33,
    /// Arbitrary metadata
    TXT = 16
};

/**
 * @brief Parse a name from a raw DNS packet
 * @param packet raw DNS packet data
 * @param offset offset into the packet where the name begins
 * @param name reference to QByteArray to store the name in
 * @return true if no errors occurred
 *
 * The offset will be incremented by the number of bytes read. Name
 * compression requires access to the contents of the packet.
 */
QTMDNS_EXPORT bool parseName(const QByteArray &packet, quint16 &offset, QByteArray &name);

/**
 * @brief Write a name to a raw DNS packet
 * @param packet raw DNS packet to write to
 * @param offset offset to update with the number of bytes written
 * @param name name to write to the packet
 * @param nameMap map of names already written to their offsets
 *
 * The offset will be incremented by the number of bytes read. The name map
 * will be updated with offsets of any names written so that it can be passed
 * to future invocations of this function.
 */
QTMDNS_EXPORT void writeName(QByteArray &packet, quint16 &offset, const QByteArray &name, QMap<QByteArray, quint16> &nameMap);

/**
 * @brief Parse a record from a raw DNS packet
 * @param packet raw DNS packet data
 * @param offset offset into the packet where the record begins
 * @param record reference to Record to populate
 * @return true if no errors occurred
 */
QTMDNS_EXPORT bool parseRecord(const QByteArray &packet, quint16 &offset, Record &record);

/**
 * @brief Write a record to a raw DNS packet
 * @param packet raw DNS packet to write to
 * @param offset offset to update with the number of bytes written
 * @param record record to write to the packet
 * @param nameMap map of names already written to their offsets
 */
QTMDNS_EXPORT void writeRecord(QByteArray &packet, quint16 &offset, const Record& record, QMap<QByteArray, quint16> &nameMap);

/**
 * @brief Populate a Message with data from a raw DNS packet
 * @param packet raw DNS packet data
 * @param message reference to Message to populate
 * @return true if no errors occurred
 */
QTMDNS_EXPORT bool fromPacket(const QByteArray &packet, Message &message);

/**
 * @brief Create a raw DNS packet from a Message
 * @param message Message to create the packet from
 * @param packet storage for raw DNS packet
 */
QTMDNS_EXPORT QByteArray toPacket(const Message &message);

/**
 * @brief Retrieve the string representation of a DNS type
 * @param type integer type
 * @return human-readable name for the type
 */
QTMDNS_EXPORT QString typeName(quint16 type);

} // namespace QtMdns
