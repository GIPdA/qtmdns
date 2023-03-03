#include <qtmdns/mdns.hpp>
#include <qtmdns/message.hpp>
#include <qtmdns/query.hpp>
#include <qtmdns/record.hpp>


namespace QtMdns {

class MessagePrivate
{
public:
    QHostAddress address;
    quint16 port {0};
    quint16 transactionId {0};
    bool isResponse {false};
    bool isTruncated {false};
    QList<Query> queries;
    QList<Record> records;
};


Message::Message() :
    dd_ptr(new MessagePrivate)
{
}

Message::Message(const Message &other) :
    dd_ptr(new MessagePrivate)
{
    *this = other;
}

Message& Message::operator=(const Message &other)
{
    *dd_ptr = *other.dd_ptr;
    return *this;
}

Message::~Message()
{
}


QHostAddress Message::address() const
{
    Q_D(const Message);
    return d->address;
}

void Message::setAddress(const QHostAddress &address)
{
    Q_D(Message);
    d->address = address;
}

quint16 Message::port() const
{
    Q_D(const Message);
    return d->port;
}

void Message::setPort(quint16 port)
{
    Q_D(Message);
    d->port = port;
}

quint16 Message::transactionId() const
{
    Q_D(const Message);
    return d->transactionId;
}

void Message::setTransactionId(quint16 transactionId)
{
    Q_D(Message);
    d->transactionId = transactionId;
}

bool Message::isResponse() const
{
    Q_D(const Message);
    return d->isResponse;
}

void Message::setResponse(bool isResponse)
{
    Q_D(Message);
    d->isResponse = isResponse;
}

bool Message::isTruncated() const
{
    Q_D(const Message);
    return d->isTruncated;
}

void Message::setTruncated(bool isTruncated)
{
    Q_D(Message);
    d->isTruncated = isTruncated;
}

QList<Query> Message::queries() const
{
    Q_D(const Message);
    return d->queries;
}

void Message::addQuery(Query const& query)
{
    Q_D(Message);
    d->queries.append(query);
}

void Message::addQueries(QList<Query> const& queries)
{
    Q_D(Message);
    d->queries.append(queries);
}

QList<Record> Message::records() const
{
    Q_D(const Message);
    return d->records;
}

void Message::addRecord(const Record &record)
{
    Q_D(Message);
    d->records.append(record);
}

void Message::reply(const Message &other)
{
    if (other.port() == mdnsDefaults().MdnsPort) {
        if (other.address().protocol() == QAbstractSocket::IPv4Protocol)
            setAddress(mdnsDefaults().MdnsIpv4Address);
        else
            setAddress(mdnsDefaults().MdnsIpv6Address);
    } else {
        setAddress(other.address());
    }

    setPort(other.port());
    setTransactionId(other.transactionId());
    setResponse(true);
}

} // namespace QtMdns
