#pragma once

#include "qtmdns_export.hpp"

#include <QByteArray>
#include <QHostAddress>

namespace QtMdns {

struct Defaults
{
    quint16 const MdnsPort; //! Standard port for mDNS
    QHostAddress const MdnsIpv4Address; //! Standard IPv4 address for mDNS
    QHostAddress const MdnsIpv6Address; //! Standard IPv6 address for mDNS
    QByteArray const MdnsBrowseType; //! Service type for browsing service types
};

Defaults const& mdnsDefaults();

} // namespace QtMdns
