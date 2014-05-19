#ifndef _BYTE_SINK_H
#define _BYTE_SINK_H

#include <assert.h>

class ByteSink
{
public:
    ByteSink(size_t size)
        : size_(size)
    {
        buf_.reserve(size);
    }

    ssize_t space()
    {
        return size_ - buf_.size();
    }
    
    template <typename T>
    void put(const T &in)
    {
        union coercion { T value; char data[ sizeof ( T ) ]; };
        coercion c;
        c.value = in;
        put(&c.data[0], sizeof(T));
    }

    void put(void *in, size_t count)
    {
        std::copy((uint8_t *) in, (uint8_t *) in + count, std::back_inserter(buf_));
    }

    const std::vector<uint8_t> & bytes()
    {
        return buf_;
    }

    void clear()
    {
        buf_.clear();
    }

    size_t size()
    {
        return buf_.size();
    }
    
private:
    size_t size_;
    std::vector<uint8_t> buf_;
};

#endif
