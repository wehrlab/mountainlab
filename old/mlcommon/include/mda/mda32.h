/******************************************************
** See the accompanying README and LICENSE files
** Author(s): Jeremy Magland
*******************************************************/

#ifndef MDA32_H
#define MDA32_H

#ifdef QT_CORE_LIB
#include <QString>
#include <QDebug>
#endif

#include "mlcommon.h"

typedef float dtype32;

extern void* allocate(const bigint nbytes);

class MdaDataFloat;

/** \class Mda32 - a multi-dimensional array corresponding to the .mda file format
 * @brief The Mda32 class
 *
 * An object of type Mda32 is a multi-dimensional array, with up to 6 dimensions. All indexing is 0-based.
 */
class Mda32 {
public:
    ///Construct an array of size N1xN2x...xN6
    Mda32(bigint N1 = 1, bigint N2 = 1, bigint N3 = 1, bigint N4 = 1, bigint N5 = 1, bigint N6 = 1);
    ///Construct an array and read the .mda file
    Mda32(const QString mda_filename);
    ///Copy constructor
    Mda32(const Mda32& other);
    ///Assignment operator
    void operator=(const Mda32& other);
    ///Destructor
    virtual ~Mda32();
    ///Allocate an array of size N1xN2x...xN6
    bool allocate(bigint N1, bigint N2, bigint N3 = 1, bigint N4 = 1, bigint N5 = 1, bigint N6 = 1);
#ifdef QT_CORE_LIB
    ///Create an array with content read from the .mda file specified by path
    bool read(const QString& path);
    ///Write the array to the .mda file specified by path, with file format 8-bit integer (numbers should be integers between 0 and 255)
    bool write8(const QString& path) const;
    ///Write the array to the .mda file specified by path, with file format 32-bit float
    bool write32(const QString& path) const;
    ///Write the array to the .mda file specified by path, with file format 64-bit float
    bool write64(const QString& path) const;
    bool writeCsv(const QString& path) const;
#endif
    ///Create an array with content read from the .mda file specified by path
    bool read(const char* path);
    bool readCsv(const QString& path);
    ///Write the array to the .mda file specified by path, with file format 8-bit integer (numbers should be integers between 0 and 255)
    bool write8(const char* path) const;
    ///Write the array to the .mda file specified by path, with file format 32-bit float
    bool write32(const char* path) const;
    ///Write the array to the .mda file specified by path, with file format 64-bit float
    bool write64(const char* path) const;

    QByteArray toByteArray8() const;
    QByteArray toByteArray32() const;
    QByteArray toByteArray64() const;
    bool fromByteArray(const QByteArray& X);

    ///The number of dimensions. This will be between 2 and 6. It will be 3 if N3()>1 and N4()...N6() are all 1. And so on.
    int ndims() const;
    ///The size of the first dimension
    bigint N1() const;
    ///The size of the second dimension
    bigint N2() const;
    ///The size of the third dimension
    bigint N3() const;
    ///The size of the fourth dimension
    bigint N4() const;
    ///The size of the fifth dimension
    bigint N5() const;
    ///The size of the sixth dimension
    bigint N6() const;
    ///The product of N1() through N6()
    bigint totalSize() const;
    bigint size(int dimension_index) const; //zero-based

    ///The value of the ith entry of the vectorized array. For example get(3+N1()*4)==get(3,4). Use the slower value(i) to safely return 0 when i is out of bounds.
    dtype32 get(bigint i) const;
    ///The value of the (i1,i2) entry of the array. Use the slower value(i1,i2) when either of the indices are out of bounds.
    dtype32 get(bigint i1, bigint i2) const;
    ///The value of the (i1,i2,i3) entry of the array. Use the slower value(i1,i2,i3) when any of the indices are out of bounds.
    dtype32 get(bigint i1, bigint i2, bigint i3) const;
    ///The value of the (i1,i2,...,i6) entry of the array. Use the slower value(i1,i2,...,i6) when any of the indices are out of bounds.
    dtype32 get(bigint i1, bigint i2, bigint i3, bigint i4, bigint i5 = 0, bigint i6 = 0) const;

    ///Set the value of the ith entry of the vectorized array to val. For example set(0.4,3+N1()*4) is the same as set(0.4,3,4). Use the slower setValue(val,i) to safely handle the case when i is out of bounds.
    void set(dtype32 val, bigint i);
    ///Set the value of the (i1,i2) entry of the array to val. Use the slower setValue(val,i1,i2) to safely handle the case when either of the indices are out of bounds.
    void set(dtype32 val, bigint i1, bigint i2);
    ///Set the value of the (i1,i2,i3) entry of the array to val. Use the slower setValue(val,i1,i2,i3) to safely handle the case when any of the indices are out of bounds.
    void set(dtype32 val, bigint i1, bigint i2, bigint i3);
    ///Set the value of the (i1,i2,...,i6) entry of the array to val. Use the slower setValue(val,i1,i2,...,i6) to safely handle the case when any of the indices are out of bounds.
    void set(dtype32 val, bigint i1, bigint i2, bigint i3, bigint i4, bigint i5 = 0, bigint i6 = 0);

    ///Slower version of get(i), safely returning 0 when i is out of bounds.
    dtype32 value(bigint i) const;
    ///Slower version of get(i1,i2), safely returning 0 when either of the indices are out of bounds.
    dtype32 value(bigint i1, bigint i2) const;
    ///Slower version of get(i1,i2,i3), safely returning 0 when any of the indices are out of bounds.
    dtype32 value(bigint i1, bigint i2, bigint i3) const;
    ///Slower version of get(i1,i2,...,i6), safely returning 0 when any of the indices are out of bounds.
    dtype32 value(bigint i1, bigint i2, bigint i3, bigint i4, bigint i5 = 0, bigint i6 = 0) const;

    ///Slower version of set(val,i), safely doing nothing when i is out of bounds.
    void setValue(dtype32 val, bigint i);
    ///Slower version of set(val,i1,i2), safely doing nothing when either of the indices are out of bounds.
    void setValue(dtype32 val, bigint i1, bigint i2);
    ///Slower version of set(val,i1,i2,i3), safely doing nothing when any of the indices are out of bounds.
    void setValue(dtype32 val, bigint i1, bigint i2, bigint i3);
    ///Slower version of set(val,i1,i2,...,i6), safely doing nothing when any of the indices are out of bounds.
    void setValue(dtype32 val, bigint i1, bigint i2, bigint i3, bigint i4, bigint i5 = 0, bigint i6 = 0);

    ///Return a pointer to the 1D raw data. The internal data may be efficiently read/written.
    dtype32* dataPtr();
    const dtype32* constDataPtr() const;
    ///Return a pointer to the 1D raw data at the vectorized location i. The internal data may be efficiently read/written.
    dtype32* dataPtr(bigint i);
    ///Return a pointer to the 1D raw data at the the location (i1,i2). The internal data may be efficiently read/written.
    dtype32* dataPtr(bigint i1, bigint i2);
    ///Return a pointer to the 1D raw data at the the location (i1,i2,i3). The internal data may be efficiently read/written.
    dtype32* dataPtr(bigint i1, bigint i2, bigint i3);
    ///Return a pointer to the 1D raw data at the the location (i1,i2,...,i6). The internal data may be efficiently read/written.
    dtype32* dataPtr(bigint i1, bigint i2, bigint i3, bigint i4, bigint i5 = 0, bigint i6 = 0);

    ///Retrieve a chunk of the vectorized data of size 1xN starting at position i
    void getChunk(Mda32& ret, bigint i, bigint N) const;
    ///Retrieve a chunk of the vectorized data of size N1xN2 starting at position (i1,i2)
    void getChunk(Mda32& ret, bigint i1, bigint i2, bigint N1, bigint N2) const;
    ///Retrieve a chunk of the vectorized data of size N1xN2xN3 starting at position (i1,i2,i3)
    void getChunk(Mda32& ret, bigint i1, bigint i2, bigint i3, bigint size1, bigint size2, bigint size3) const;

    ///Set a chunk of the vectorized data starting at position i
    void setChunk(Mda32& X, bigint i);
    ///Set a chunk of the vectorized data starting at position (i1,i2)
    void setChunk(Mda32& X, bigint i1, bigint i2);
    ///Set a chunk of the vectorized data of size N1xN2xN3 starting at position (i1,i2,i3)
    void setChunk(Mda32& X, bigint i1, bigint i2, bigint i3);

    dtype32 minimum() const;
    dtype32 maximum() const;

    bool reshape(bigint N1b, bigint N2b, bigint N3b = 1, bigint N4b = 1, bigint N5b = 1, bigint N6b = 1);

private:
    QSharedDataPointer<MdaDataFloat> d;
};

#endif // MDA_H
