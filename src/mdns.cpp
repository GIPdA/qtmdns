#include <qtmdns/mdns.h>

namespace QtMdns {

const quint16 MdnsPort = 5353;
const QHostAddress MdnsIpv4Address("224.0.0.251");
const QHostAddress MdnsIpv6Address("ff02::fb");
const QByteArray MdnsBrowseType("_services._dns-sd._udp.local.");

} // namespace QtMdns
