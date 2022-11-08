/*
 * mDNS library for Qt.
 *
 * Based on https://github.com/nitroshare/qmdnsengine
 */

StaticLibrary {
    name: "qtmdns"

    Depends { name: 'cpp' }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.network" }

    Depends { name: "bundle" }
    bundle.isBundle: false

    files: [
        "include/qtmdns/abstractserver.hpp",
        "include/qtmdns/bitmap.hpp",
        "include/qtmdns/browser.hpp",
        "include/qtmdns/cache.hpp",
        "include/qtmdns/dns.hpp",
        "include/qtmdns/hostname.hpp",
        "include/qtmdns/mdns.hpp",
        "include/qtmdns/message.hpp",
        "include/qtmdns/prober.hpp",
        "include/qtmdns/provider.hpp",
        "include/qtmdns/query.hpp",
        "include/qtmdns/record.hpp",
        "include/qtmdns/resolver.hpp",
        "include/qtmdns/server.hpp",
        "include/qtmdns/qtmdns_export.hpp",
        "include/qtmdns/service.hpp",
        "src/abstractserver.cpp",
        "src/bitmap.cpp",
        "src/browser.cpp",
        "src/cache.cpp",
        "src/dns.cpp",
        "src/hostname.cpp",
        "src/mdns.cpp",
        "src/message.cpp",
        "src/prober.cpp",
        "src/provider.cpp",
        "src/query.cpp",
        "src/record.cpp",
        "src/resolver.cpp",
        "src/server.cpp",
        "src/service.cpp",
    ]

    cpp.includePaths: ["include"]

    Export {
        Depends { name: "cpp" }
        Depends { name: "Qt.core" }
        Depends { name: "Qt.network" }
        cpp.includePaths: [exportingProduct.sourceDirectory+"/include"]
        cpp.defines: ["QTMDNS_LIBRARY"]
    }
}
