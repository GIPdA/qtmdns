#include <qtmdns/bitmap.h>

namespace QtMdns {

class BitmapPrivate
{
public:
    BitmapPrivate()
    {}

    void fromData(std::vector<quint8> newData)
    {
        data = std::move(newData);
    }

    std::vector<quint8> data;
};


Bitmap::Bitmap() :
    dd_ptr(new BitmapPrivate)
{}

Bitmap::Bitmap(Bitmap const& other) :
    dd_ptr(new BitmapPrivate)
{
    Q_D(Bitmap);
    d->fromData(other.dd_ptr->data);
}

Bitmap& Bitmap::operator=(Bitmap const& other)
{
    Q_D(Bitmap);
    d->fromData(other.dd_ptr->data);
    return *this;
}

bool Bitmap::operator==(Bitmap const& other) const
{
    Q_D(const Bitmap);
    return d->data == other.dd_ptr->data;
}

Bitmap::~Bitmap()
{
}


size_t Bitmap::length() const
{
    Q_D(const Bitmap);
    return d->data.size();
}

quint8 const* Bitmap::data() const
{
    Q_D(const Bitmap);
    return d->data.data();
}

void Bitmap::setData(quint8 length, quint8 const* data)
{
    Q_D(Bitmap);
    d->fromData(std::vector<quint8>(data, data+length));
}

void Bitmap::setData(std::vector<quint8> data)
{
    Q_D(Bitmap);
    d->fromData(std::move(data));
}

} // namespace QtMdns
