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
        "include/qtmdns/abstractserver.h",
        "include/qtmdns/bitmap.h",
        "include/qtmdns/browser.h",
        "include/qtmdns/cache.h",
        "include/qtmdns/dns.h",
        "include/qtmdns/hostname.h",
        "include/qtmdns/mdns.h",
        "include/qtmdns/message.h",
        "include/qtmdns/prober.h",
        "include/qtmdns/provider.h",
        "include/qtmdns/query.h",
        "include/qtmdns/record.h",
        "include/qtmdns/resolver.h",
        "include/qtmdns/server.h",
        "include/qtmdns/service.h",
        "include/qtmdns/qtmdns_export.hpp",
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
