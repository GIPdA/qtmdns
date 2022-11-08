#pragma once

#include "qtmdns_export.hpp"

#include <QScopedPointer>

#include <vector>

namespace QtMdns {

class QTMDNS_EXPORT BitmapPrivate;

/**
 * @brief 256-bit bitmap
 *
 * Bitmaps are used in QtMdns::NSEC records to indicate which records are
 * available. Bitmaps in mDNS records use only the first block (block 0).
 */
class QTMDNS_EXPORT Bitmap
{
public:
    Bitmap();
    Bitmap(Bitmap const& other);
    Bitmap& operator=(Bitmap const& other);
    bool operator==(Bitmap const& other) const;
    ~Bitmap();

    /**
     * @brief Retrieve the length of the block in bytes
     *
     * This method indicates how many bytes are pointed to by the data()
     * method.
     */
    size_t length() const;

    /**
     * @brief Retrieve a pointer to the underlying data in the bitmap
     *
     * Use the length() method to determine how many bytes contain valid data.
     */
    quint8 const* data() const;

    /**
     * @brief Set the data to be stored in the bitmap
     *
     * The length parameter indicates how many bytes of data are valid. The
     * actual bytes are copied to the bitmap.
     */
    void setData(quint8 length, quint8 const* data);
    void setData(std::vector<quint8> data);

private:
    Q_DECLARE_PRIVATE_D(dd_ptr, Bitmap)
    QScopedPointer<BitmapPrivate> dd_ptr;
};

} // namespace QtMdns
