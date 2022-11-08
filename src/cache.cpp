#include <qtmdns/cache.hpp>
#include <qtmdns/dns.hpp>
#include <qtmdns/record.hpp>

#include <QtGlobal>
#if(QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
#include <QRandomGenerator>
#define USE_QRANDOMGENERATOR
#endif

#include <QDateTime>
#include <QList>
#include <QObject>
#include <QTimer>


namespace QtMdns {

class CachePrivate : public QObject
{
    Q_DISABLE_COPY_MOVE(CachePrivate)
    Q_DECLARE_PUBLIC(Cache)
    Cache * const q_ptr {nullptr};

public:
    struct Entry
    {
        Record record;
        QList<QDateTime> triggers;
    };

    CachePrivate(Cache* cache) :
        QObject(cache),
        q_ptr(cache)
    {
        connect(&timer, &QTimer::timeout, this, &CachePrivate::onTimeout);

        timer.setSingleShot(true);
    }

    void onTimeout()
    {
        // Loop through all of the records in the cache, emitting the appropriate
        // signal when a trigger has passed, determining when the next trigger
        // will occur, and removing records that have expired
        QDateTime const now = QDateTime::currentDateTime();
        QDateTime newNextTrigger;

        for (auto it = entries.begin(); it != entries.end();) {
            // Loop through the triggers and remove ones that have already
            // passed
            bool shouldQuery = false;
            for (auto j = it->triggers.begin(); j != it->triggers.end();) {
                if ((*j) <= now) {
                    shouldQuery = true;
                    j = it->triggers.erase(j);
                } else {
                    break;
                }
            }

            // If triggers remain, determine the next earliest one; if none
            // remain, the record has expired and should be removed
            if (it->triggers.length()) {
                if (newNextTrigger.isNull() || it->triggers.at(0) < newNextTrigger)
                    newNextTrigger = it->triggers.at(0);

                if (shouldQuery)
                    emit q_ptr->shouldQuery(it->record);

                ++it;
            } else {
                emit q_ptr->recordExpired(it->record);
                it = entries.erase(it);
            }
        }

        // If newNextTrigger contains a value, it will be the time for the next
        // trigger and the timer should be started again
        nextTrigger = newNextTrigger;
        if ( ! nextTrigger.isNull())
            timer.start(now.msecsTo(nextTrigger));
    }

private:
    QTimer timer;
    QList<Entry> entries;
    QDateTime nextTrigger;
};


Cache::Cache(QObject *parent)
    : QObject(parent),
      dd_ptr(new CachePrivate(this))
{
}

Cache::~Cache()
{
}


void Cache::addRecord(const Record &record)
{
    Q_D(Cache);
    // If a record exists that matches, remove it from the cache; if the TTL
    // is nonzero, it will be added back to the cache with updated times
    for (auto i = d->entries.begin(); i != d->entries.end();) {
        if ( (record.flushCache()
              && (*i).record.name() == record.name()
              && (*i).record.type() == record.type()
             )
            || (*i).record == record )
        {

            // If the TTL is set to 0, indicate that the record was removed
            if (record.ttl() == 0)
                emit recordExpired((*i).record);

            i = d->entries.erase(i);

            // No need to continue further if the TTL was set to 0
            if (record.ttl() == 0)
                return;

        } else {
            ++i;
        }
    }

    // Use the current time to calculate the triggers and add a random offset
    QDateTime now = QDateTime::currentDateTime();
#ifdef USE_QRANDOMGENERATOR
    qint64 const random = QRandomGenerator::global()->bounded(20);
#else
    qint64 const random = qrand() % 20;
#endif

    QList<QDateTime> triggers {
        now.addMSecs(record.ttl() * 500 + random),  // 50%
        now.addMSecs(record.ttl() * 850 + random),  // 85%
        now.addMSecs(record.ttl() * 900 + random),  // 90%
        now.addMSecs(record.ttl() * 950 + random),  // 95%
        now.addSecs(record.ttl())
    };

    // Append the record and its triggers
    d->entries.append({record, triggers});

    // Check if the new record's first trigger is earlier than the next
    // scheduled trigger; if so, restart the timer
    if (d->nextTrigger.isNull() || (triggers.at(0) < d->nextTrigger)) {
        d->nextTrigger = triggers.at(0);
        d->timer.start(now.msecsTo(d->nextTrigger));
    }
}

bool Cache::lookupRecord(const QByteArray &name, quint16 type, Record &record) const
{
    QList<Record> records;
    if (lookupRecords(name, type, records)) {
        record = records.at(0);
        return true;
    }
    return false;
}

bool Cache::lookupRecords(const QByteArray &name, quint16 type, QList<Record> &records) const
{
    Q_D(const Cache);
    bool recordsAdded = false;
    for (CachePrivate::Entry const& entry : d->entries) {
        if (   (name.isNull() || entry.record.name().endsWith(name))
            && (type == ANY || entry.record.type() == type) )
        {
            records.append(entry.record);
            recordsAdded = true;
        }
    }
    return recordsAdded;
}

} // namespace QtMdns
