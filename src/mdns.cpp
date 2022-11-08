#include <qtmdns/mdns.h>

namespace QtMdns {

Defaults const& mdnsDefaults()
{
    static Defaults const def {
        .MdnsPort = 5353,
        .MdnsIpv4Address = QHostAddress{"224.0.0.251"},
        .MdnsIpv6Address = QHostAddress{"ff02::fb"},
        .MdnsBrowseType = QByteArray{"_services._dns-sd._udp.local."}
    };
    return def;
}

} // namespace QtMdns
