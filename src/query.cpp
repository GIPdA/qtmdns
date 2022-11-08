#include <qtmdns/dns.hpp>
#include <qtmdns/query.hpp>

#include <QByteArray>
#include <QDebug>

namespace QtMdns {

class QueryPrivate
{
public:
    QByteArray name;
    quint16 type {0};
    bool unicastResponse {true};
};


Query::Query() :
    dd_ptr(new QueryPrivate)
{
}

Query::Query(const Query &other) :
    dd_ptr(new QueryPrivate)
{
    *this = other;
}

Query &Query::operator=(const Query &other)
{
    *dd_ptr = *other.dd_ptr;
    return *this;
}

Query::~Query()
{
}


QByteArray Query::name() const
{
    Q_D(const Query);
    return d->name;
}

void Query::setName(const QByteArray &name)
{
    Q_D(Query);
    d->name = name;
}

quint16 Query::type() const
{
    Q_D(const Query);
    return d->type;
}

void Query::setType(quint16 type)
{
    Q_D(Query);
    d->type = type;
}

bool Query::unicastResponse() const
{
    Q_D(const Query);
    return d->unicastResponse;
}

void Query::setUnicastResponse(bool unicastResponse)
{
    Q_D(Query);
    d->unicastResponse = unicastResponse;
}


QDebug operator<<(QDebug dbg, const Query &query)
{
    QDebugStateSaver saver(dbg);
    Q_UNUSED(saver)

    dbg.noquote().nospace() << "Query(" << typeName(query.type()) << " " << query.name() << ")";

    return dbg;
}

} // namespace QtMdns
