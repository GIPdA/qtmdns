## Qt mDNS

[![MIT License](http://img.shields.io/badge/license-MIT-blue.svg?style=flat)](http://opensource.org/licenses/MIT)


This library provides an implementation of multicast DNS as per [RFC 6762](https://tools.ietf.org/html/rfc6762).

It's a fork of QMdnsEngine, with some bugfixes.

### TODOs

- Add tests back with automated testing using Github actions


### Documentation

Import the library with your project. Currently, it only has QBS support.


To add a simple mDNS server, you need a Server, a Provider and a Hostname:

```
#include <qtmdns/hostname.hpp>
#include <qtmdns/provider.hpp>
#include <qtmdns/server.hpp>
#include <qtmdns/service.hpp>

QtMdns::Server server;
QtMdns::Hostname hostname(&server, "my-server"); // We want to use my-server.local.
QtMdns::Provider provider(&server, &hostname);

QtMdns::Service service;
service.setName("my-webserver");
service.setType("_http._tcp.local.");
service.setPort(80);

provider.update(service);

// Listen for the actual hostname we got. If my-server isn't available,
// it will use my-server-2.local or my-server-3, etc.
QObject::connect(&hostname, &QtMdns::Hostname::hostnameChanged, this,
                 [](QByteArray const& hostname) {
    qDebug("HOSTNAME: %s", qPrintable(QString::fromLatin1(hostname)));
});
```


To browse for services of a certain type, use a Server and a Browser:

```
#include <qtmdns/browser.hpp>
#include <qtmdns/cache.hpp>
#include <qtmdns/server.hpp>
#include <qtmdns/service.hpp>

QtMdns::Server server;
QtMdns::Browser browser(&server, "_http._tcp.local.");
// See Browser CTOR for external cache uses.

QObject::connect(&browser, &QtMdns::Browser::serviceAdded, this,
                 [](QtMdns::Service const& service) {
    qDebug() << "Service added:" << service.name() << "from" << service.hostname();
});

QObject::connect(&browser, &QtMdns::Browser::serviceUpdated, this,
                 [](QtMdns::Service const& service) {
    qDebug() << "Service updated:" << service.name();
});

QObject::connect(&browser, &QtMdns::Browser::serviceRemoved, this,
                 [](QtMdns::Service const& service) {
    qDebug() << "Service removed:" << service.name();
});
```

The server will bind to all compatible interfaces on standard mDNS port & IPs, IPv4 and IPv6.


Example projects can be found here: https://github.com/GIPdA/qtmdns_examples.git


### Credits

Forked from QMdnsEngine: https://github.com/nitroshare/qmdnsengine
