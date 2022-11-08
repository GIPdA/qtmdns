#include <qtmdns/abstractserver.hpp>
#include <qtmdns/dns.hpp>
#include <qtmdns/message.hpp>
#include <qtmdns/prober.hpp>
#include <qtmdns/query.hpp>
#include <qtmdns/record.hpp>

#include <QPointer>
#include <QTimer>

namespace QtMdns {

class ProberPrivate : public QObject
{
    Q_DISABLE_COPY_MOVE(ProberPrivate)
    Q_DECLARE_PUBLIC(Prober)
    Prober * const q_ptr {nullptr};

public:
    ProberPrivate(Prober* prober, AbstractServer* server, Record record) :
        QObject(prober),
        q_ptr(prober),
        server(server),
        confirmed(false),
        proposedRecord(std::move(record)),
        suffix(1)
    {
        // All records should contain at least one "."
        qsizetype const index = record.name().indexOf('.');
        name = record.name().left(index);
        type = record.name().mid(index);

        connect(server, &AbstractServer::messageReceived, this, &ProberPrivate::onMessageReceived);
        connect(&timer, &QTimer::timeout, this, &ProberPrivate::onTimeout);

        timer.setSingleShot(true);

        assertRecord();
    }

    void assertRecord()
    {
        // Use the current suffix to set the name of the proposed record
        QString const tmpName = suffix == 1
                                ? QString("%1%2").arg(name, type.constData())
                                : QString("%1-%2%3").arg(name.constData(), QByteArray::number(suffix), type);

        proposedRecord.setName(tmpName.toUtf8());

        // Broadcast a query for the proposed name (using an ANY query) and
        // include the proposed record in the query
        Query query;
            query.setName(proposedRecord.name());
            query.setType(ANY);

        Message message;
            message.addQuery(query);
            message.addRecord(proposedRecord);

        server->sendMessageToAll(message);

        // Wait two seconds to confirm it is unique
        timer.stop();
        timer.start(2 * 1000);
    }

    void onMessageReceived(const Message &message)
    {
        // If the response matches the proposed record, increment the suffix and
        // try with the new name

        if (confirmed || ! message.isResponse())
            return;

        QList<Record> const& records = message.records();
        for (const Record &record : records) {
            if (record.name() == proposedRecord.name() && record.type() == proposedRecord.type()) {
                ++suffix;
                assertRecord();
            }
        }
    }

    void onTimeout()
    {
        confirmed = true;
        emit q_ptr->nameConfirmed(proposedRecord.name());
    }

private:
    QPointer<AbstractServer> server;
    QTimer timer;

    bool confirmed {false};

    Record proposedRecord;
    QByteArray name;
    QByteArray type;
    int suffix {0};
};


Prober::Prober(AbstractServer* server, Record record, QObject* parent) :
    QObject(parent),
    dd_ptr(new ProberPrivate(this, server, std::move(record)))
{
}

Prober::~Prober()
{
}

} // namespace QtMdns
