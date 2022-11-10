#include <qtmdns/service.hpp>

namespace QtMdns {

class ServicePrivate
{
public:
    QByteArray type;
    QByteArray name;
    QByteArray hostname;
    quint16 port;
    QHostAddress hostAddress;
    QHostAddress hostAddressIPv6;
    QMap<QByteArray, QByteArray> attributes;
};


Service::Service() :
    dd_ptr(new ServicePrivate)
{
}

Service::Service(Service const& other) :
    dd_ptr(new ServicePrivate)
{
    *this = other;
}

Service& Service::operator=(const Service &other)
{
    *dd_ptr = *other.dd_ptr;
    return *this;
}

bool Service::operator==(const Service &other) const
{
    Q_D(const Service);
    return d->type == other.dd_ptr->type &&
           d->name == other.dd_ptr->name &&
           d->port == other.dd_ptr->port &&
           d->attributes == other.dd_ptr->attributes;
}

bool Service::operator!=(const Service &other) const
{
    return !(*this == other);
}

Service::~Service()
{
}


QByteArray Service::type() const
{
    Q_D(const Service);
    return d->type;
}

void Service::setType(const QByteArray &type)
{
    Q_D(Service);
    d->type = type;
}

QByteArray Service::name() const
{
    Q_D(const Service);
    return d->name;
}

void Service::setName(const QByteArray &name)
{
    Q_D(Service);
    d->name = name;
}

QByteArray Service::hostname() const
{
    Q_D(const Service);
    return d->hostname;
}

void Service::setHostname(const QByteArray &hostname)
{
    Q_D(Service);
    d->hostname = hostname;
}

quint16 Service::port() const
{
    Q_D(const Service);
    return d->port;
}

void Service::setPort(quint16 port)
{
    Q_D(Service);
    d->port = port;
}

QHostAddress const& Service::hostAddress() const
{
    Q_D(const Service);
    return d->hostAddress;
}

void Service::setHostAddress(QHostAddress const& address)
{
    Q_D(Service);
    d->hostAddress = address;
}

QHostAddress const& Service::hostAddressIPv6() const
{
    Q_D(const Service);
    return d->hostAddressIPv6;
}

void Service::setHostAddressIPv6(QHostAddress const& address)
{
    Q_D(Service);
    d->hostAddressIPv6 = address;
}

QMap<QByteArray, QByteArray> Service::attributes() const
{
    Q_D(const Service);
    return d->attributes;
}

void Service::setAttributes(const QMap<QByteArray, QByteArray> &attributes)
{
    Q_D(Service);
    d->attributes = attributes;
}

void Service::addAttribute(const QByteArray &key, const QByteArray &value)
{
    Q_D(Service);
    d->attributes.insert(key, value);
}


QDebug operator<<(QDebug debug, const Service &service)
{
    QDebugStateSaver saver(debug);
    Q_UNUSED(saver)

    debug.noquote().nospace()
        << "Service(name: " << service.name()
        << ", type: " << service.type()
        << ", hostname: " << service.hostname()
        << ", port: " << service.port()
        << ", host address: " << service.hostAddress()
        << ", attributes: " << service.attributes()
        << ")";

    return debug;
}

} // namespace QtMdns
