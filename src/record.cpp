
#include <qtmdns/bitmap.h>
#include <qtmdns/dns.h>
#include <qtmdns/record.h>

#include <QDebug>

namespace QtMdns {

class RecordPrivate
{
public:
    QByteArray name;
    quint16 type {0};
    bool flushCache {false};
    quint32 ttl {3600};

    QHostAddress address;
    QByteArray target;
    QByteArray nextDomainName;
    quint16 priority {0};
    quint16 weight {0};
    quint16 port {0};
    QMap<QByteArray, QByteArray> attributes;
    Bitmap bitmap;
};


Record::Record() :
    dd_ptr(new RecordPrivate)
{
}

Record::Record(const Record &other) :
    dd_ptr(new RecordPrivate)
{
    *this = other;
}

Record& Record::operator=(const Record &other)
{
    *dd_ptr = *other.dd_ptr;
    return *this;
}

Record::~Record()
{
}


bool Record::operator==(const Record &other) const
{
    Q_D(const Record);
    return d->name == other.dd_ptr->name &&
        d->type == other.dd_ptr->type &&
        d->address == other.dd_ptr->address &&
        d->target == other.dd_ptr->target &&
        d->nextDomainName == other.dd_ptr->nextDomainName &&
        d->priority == other.dd_ptr->priority &&
        d->weight == other.dd_ptr->weight &&
        d->port == other.dd_ptr->port &&
        d->attributes == other.dd_ptr->attributes &&
        d->bitmap == other.dd_ptr->bitmap;
}

bool Record::operator!=(const Record &other) const
{
    return !(*this == other);
}


QByteArray Record::name() const
{
    Q_D(const Record);
    return d->name;
}

void Record::setName(const QByteArray &name)
{
    Q_D(Record);
    d->name = name;
}

quint16 Record::type() const
{
    Q_D(const Record);
    return d->type;
}

void Record::setType(quint16 type)
{
    Q_D(Record);
    d->type = type;
}

bool Record::flushCache() const
{
    Q_D(const Record);
    return d->flushCache;
}

void Record::setFlushCache(bool flushCache)
{
    Q_D(Record);
    d->flushCache = flushCache;
}

quint32 Record::ttl() const
{
    Q_D(const Record);
    return d->ttl;
}

void Record::setTtl(quint32 ttl)
{
    Q_D(Record);
    d->ttl = ttl;
}

QHostAddress Record::address() const
{
    Q_D(const Record);
    return d->address;
}

void Record::setAddress(const QHostAddress &address)
{
    Q_D(Record);
    d->address = address;
}

QByteArray Record::target() const
{
    Q_D(const Record);
    return d->target;
}

void Record::setTarget(const QByteArray &target)
{
    Q_D(Record);
    d->target = target;
}

QByteArray Record::nextDomainName() const
{
    Q_D(const Record);
    return d->nextDomainName;
}

void Record::setNextDomainName(const QByteArray &nextDomainName)
{
    Q_D(Record);
    d->nextDomainName = nextDomainName;
}

quint16 Record::priority() const
{
    Q_D(const Record);
    return d->priority;
}

void Record::setPriority(quint16 priority)
{
    Q_D(Record);
    d->priority = priority;
}

quint16 Record::weight() const
{
    Q_D(const Record);
    return d->weight;
}

void Record::setWeight(quint16 weight)
{
    Q_D(Record);
    d->weight = weight;
}

quint16 Record::port() const
{
    Q_D(const Record);
    return d->port;
}

void Record::setPort(quint16 port)
{
    Q_D(Record);
    d->port = port;
}

QMap<QByteArray, QByteArray> Record::attributes() const
{
    Q_D(const Record);
    return d->attributes;
}

void Record::setAttributes(const QMap<QByteArray, QByteArray> &attributes)
{
    Q_D(Record);
    d->attributes = attributes;
}

void Record::addAttribute(const QByteArray &key, const QByteArray &value)
{
    Q_D(Record);
    d->attributes.insert(key, value);
}

Bitmap Record::bitmap() const
{
    Q_D(const Record);
    return d->bitmap;
}

void Record::setBitmap(const Bitmap &bitmap)
{
    Q_D(Record);
    d->bitmap = bitmap;
}


QDebug operator<<(QDebug dbg, const Record &record)
{
    QDebugStateSaver saver(dbg);
    Q_UNUSED(saver)

    dbg.noquote().nospace() << "Record(" << typeName(record.type()) << " " << record.name() << ")";

    return dbg;
}

} // namespace QtMdns
