#pragma once

#include "qtmdns_export.hpp"

#include <QByteArray>
#include <QHostAddress>

namespace QtMdns {

/**
 * @brief Standard port for mDNS
 */
QTMDNS_EXPORT extern const quint16 MdnsPort;

/**
 * @brief Standard IPv4 address for mDNS
 */
QTMDNS_EXPORT extern const QHostAddress MdnsIpv4Address;

/**
 * @brief Standard IPv6 address for mDNS
 */
QTMDNS_EXPORT extern const QHostAddress MdnsIpv6Address;

/**
 * @brief Service type for browsing service types
 */
QTMDNS_EXPORT extern const QByteArray MdnsBrowseType;

} // namespace QtMdns
