#ifndef BELL_BYTE_READER_H
#define BELL_BYTE_READER_H

#include <stdlib.h>
#include <stdint.h>

/**
 * A class for reading bytes from a stream. Further implemented in HTTPStream.h
 */
namespace bell
{
    class ByteStream
    {
    public:
        ByteStream(){};
        ~ByteStream(){};

        virtual size_t read(uint8_t *buf, size_t nbytes) = 0;
        virtual size_t skip(size_t nbytes) = 0;

        virtual size_t position() = 0;
        virtual size_t size() = 0;
        virtual void close() = 0;
    };
}

#endif
