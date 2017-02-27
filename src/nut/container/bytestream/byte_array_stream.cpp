
#include <assert.h>
#include <algorithm> // for std::min()

#include "byte_array_stream.h"

namespace nut
{

ByteArrayStream::ByteArrayStream()
{
    _data = rc_new<byte_rcarray_type>(16);
}

ByteArrayStream::ByteArrayStream(byte_rcarray_type *arr)
    : _data(arr)
{
    assert(nullptr != arr);
}

bool ByteArrayStream::is_little_endian() const
{
    return _little_endian;
}

void ByteArrayStream::set_little_endian(bool le)
{
    _little_endian = le;
}
    
ByteArrayStream::byte_rcarray_type* ByteArrayStream::byte_array() const
{
    return _data;
}
    
size_t ByteArrayStream::size() const
{
    return _data->size();
}

void ByteArrayStream::resize(size_t new_size)
{
    _data->resize(new_size);
    if (_index > new_size)
        _index = new_size;
}

size_t ByteArrayStream::tell() const
{
    return _index;
}

void ByteArrayStream::seek(size_t index)
{
    assert(index <= _data->size());
    _index = index;
}

size_t ByteArrayStream::readable_size() const
{
    return _data->size() - _index;
}

void ByteArrayStream::skip_read(size_t cb)
{
    assert(_index + cb <= _data->size());
    _index += cb;
}

size_t ByteArrayStream::read(void *buf, size_t cb)
{
    assert(nullptr != buf || 0 == cb);
    const byte_rcarray_type *data = _data;
    size_t ret = (std::min)(cb, readable_size());
    ::memcpy(buf, data->data() + _index, ret);
    _index += ret;
    return ret;
}

size_t ByteArrayStream::write(const void *buf, size_t cb)
{
    assert(nullptr != buf || 0 == cb);
    assert(_index <= _data->size());
    const size_t copy = (std::min)(cb, _data->size() - _index);
    ::memcpy(_data->data() + _index, buf, copy);
    _data->append(((const uint8_t*) buf) + copy, ((const uint8_t*) buf) + cb);
    _index += cb;
    return cb;
}

size_t ByteArrayStream::write(const byte_array_type& ba)
{
    return write(ba.data(), ba.size());
}

}
