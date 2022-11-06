#include <qtmdns/abstractserver.h>
#include <qtmdns/dns.h>
#include <qtmdns/hostname.h>
#include <qtmdns/mdns.h>
#include <qtmdns/message.h>
#include <qtmdns/prober.h>
#include <qtmdns/provider.h>
#include <qtmdns/query.h>
#include <qtmdns/record.h>
#include <qtmdns/service.h>

#include <QPointer>

namespace QtMdns {

class ProviderPrivate : public QObject
{
    Q_DISABLE_COPY_MOVE(ProviderPrivate)
    Q_DECLARE_PUBLIC(Provider)
    Provider * const q_ptr {nullptr};

public:
    ProviderPrivate(Provider* q, AbstractServer* server, Hostname* hostname) :
        QObject(q),
        q_ptr(q),
        server(server),
        hostname(hostname)
    {
        connect(server, &AbstractServer::messageReceived, this, &ProviderPrivate::onMessageReceived);
        connect(hostname, &Hostname::hostnameChanged, this, &ProviderPrivate::onHostnameChanged);

        browsePtrProposed.setName(MdnsBrowseType);
        browsePtrProposed.setType(PTR);
        ptrProposed.setType(PTR);
        srvProposed.setType(SRV);
        txtProposed.setType(TXT);
    }

    virtual ~ProviderPrivate()
    {
        if (confirmed)
            farewell();
    }

    void announce()
    {
        // Broadcast a message with each of the records
        Message message;
            message.setResponse(true);
            message.addRecord(ptrRecord);
            message.addRecord(srvRecord);
            message.addRecord(txtRecord);

        server->sendMessageToAll(message);
    }

    void confirm()
    {
        // Confirm that the desired name is unique through probing
        prober.reset(new Prober(server, srvProposed, this));

        connect(prober.get(), &Prober::nameConfirmed, prober.get(),
                [this](const QByteArray &name) {

            // If existing records were confirmed, indicate that they are no
            // longer valid
            if (confirmed)
                farewell();
            else
                confirmed = true;

            // Update the proposed records
            ptrProposed.setTarget(name);
            srvProposed.setName(name);
            txtProposed.setName(name);

            // Publish the proposed records and announce them
            publish();

            prober.reset();
        });
    }

    void farewell()
    {
        // Send a message indicating that the existing records are no longer valid
        // by setting their TTL to 0
        ptrRecord.setTtl(0);
        srvRecord.setTtl(0);
        txtRecord.setTtl(0);

        announce();
    }

    void publish()
    {
        // Copy the proposed records over and announce them
        browsePtrRecord = browsePtrProposed;
        ptrRecord = ptrProposed;
        srvRecord = srvProposed;
        txtRecord = txtProposed;

        announce();
    }


    void onMessageReceived(const Message &message)
    {
        if ( ! confirmed || message.isResponse())
            return;

        bool sendBrowsePtr = false;
        bool sendPtr = false;
        bool sendSrv = false;
        bool sendTxt = false;

        // Determine which records to send based on the queries
        const auto queries = message.queries();
        for (const Query &query : queries) {
            if (query.type() == PTR && query.name() == MdnsBrowseType) {
                sendBrowsePtr = true;
            } else if (query.type() == PTR && query.name() == ptrRecord.name()) {
                sendPtr = true;
            } else if (query.type() == SRV && query.name() == srvRecord.name()) {
                sendSrv = true;
            } else if (query.type() == TXT && query.name() == txtRecord.name()) {
                sendTxt = true;
            }
        }

        // Remove records to send if they are already known
        const auto records = message.records();
        for (const Record &record : records) {
            if (record == ptrRecord) {
                sendPtr = false;
            } else if (record == srvRecord) {
                sendSrv = false;
            } else if (record == txtRecord) {
                sendTxt = false;
            }
        }

        // Include the SRV and TXT if the PTR is being sent
        if (sendPtr)
            sendSrv = sendTxt = true;

        // If any records should be sent, compose a message reply
        if (sendBrowsePtr || sendPtr || sendSrv || sendTxt) {
            Message reply;
            reply.reply(message);
            if (sendBrowsePtr)
                reply.addRecord(browsePtrRecord);

            if (sendPtr)
                reply.addRecord(ptrRecord);

            if (sendSrv)
                reply.addRecord(srvRecord);

            if (sendTxt)
                reply.addRecord(txtRecord);

            server->sendMessage(reply);
        }
    }

    void onHostnameChanged(QByteArray const& newHostname)
    {
        // Update the proposed SRV record
        srvProposed.setTarget(newHostname);

        // If initialized, confirm the record
        if (initialized)
            confirm();
    }

private:
    QPointer<AbstractServer> server;
    QPointer<Hostname> hostname;
    std::unique_ptr<Prober> prober;

    Service service;
    bool initialized {false};
    bool confirmed {false};

    Record browsePtrRecord;
    Record ptrRecord;
    Record srvRecord;
    Record txtRecord;

    Record browsePtrProposed;
    Record ptrProposed;
    Record srvProposed;
    Record txtProposed;
};


Provider::Provider(AbstractServer* server, Hostname* hostname, QObject* parent) :
    QObject(parent),
    dd_ptr(new ProviderPrivate(this, server, hostname))
{
}

Provider::~Provider()
{
}


void Provider::update(const Service &service)
{
    Q_D(Provider);
    d->initialized = true;

    // Clean the service name
    QByteArray const serviceName = service.name().replace('.', '-');

    // Update the proposed records
    QByteArray const fqName = serviceName + "." + service.type();
    d->browsePtrProposed.setTarget(service.type());
    d->ptrProposed.setName(service.type());
    d->ptrProposed.setTarget(fqName);
    d->srvProposed.setName(fqName);
    d->srvProposed.setPort(service.port());
    d->srvProposed.setTarget(d->hostname->hostname());
    d->txtProposed.setName(fqName);
    d->txtProposed.setAttributes(service.attributes());

    // Assuming a valid hostname exists, check to see if the new service uses
    // a different name - if so, it must first be confirmed
    if (d->hostname->isRegistered()) {
        if ( ! d->confirmed || (fqName != d->srvRecord.name()) ) {
            d->confirm();
        } else {
            d->publish();
        }
    }
}

} // namespace QtMdns
