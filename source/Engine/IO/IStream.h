#ifndef ISTREAM_H
#define ISTREAM_H

#include "../IStandardHeaders.h"
#include <zlib.h>

class IStream {
private:
    unsigned char* ptr;
    unsigned char* ptr_start;
    long  distance = 0;
public:
    IStream(void* pt) {
        this->ptr = (unsigned char*)pt;
        ptr_start = (unsigned char*)pt;
    }

    unsigned char ReadByte() {
        unsigned char data = *ptr;
        ptr++;
        distance++;
        return data;
    }
    unsigned char* ReadByte4() {
        unsigned char* data = ptr;
        ptr += 4;
        distance += 4;
        return data;
    }
    unsigned char* ReadBytes(int n) {
        unsigned char* data = ptr;
        ptr += n;
        distance += n;
        return data;
    }
    unsigned int ReadUInt16() {
        unsigned int data = 0;
        data += (unsigned char)(*ptr); ptr++;
        data += (unsigned char)(*ptr) * 0x100; ptr++;
        distance += 2;
        return (unsigned int)data;
    }
    unsigned int ReadUInt16E() {
        unsigned int data = 0;
        data += (unsigned char)(*ptr) * 0x100; ptr++;
        data += (unsigned char)(*ptr); ptr++;
        distance += 2;
        return (unsigned int)data;
    }
    unsigned int ReadUInt32() {
        unsigned int data = 0;
        data += (*ptr) << 0; ptr++;
        data += (*ptr) << 8; ptr++;
        data += (*ptr) << 16; ptr++;
        data += (*ptr) << 24; ptr++;
        distance += 4;
        return (unsigned int)data;
    }
    unsigned int ReadUInt32BE() {
        unsigned int data = 0;
        data += (*ptr) << 24; ptr++;
        data += (*ptr) << 16; ptr++;
        data += (*ptr) << 8; ptr++;
        data += (*ptr) << 0; ptr++;
        distance += 4;
        return (unsigned int)data;
    }
    char* ReadRSDKString() {
        unsigned char count = *ptr;
        ptr++;
        char* data = (char*)ptr;
        ptr += count;
        distance += count + 1;
        return data;
    }

    unsigned long Decompress(void* dst, int dstLen, void* src, int srcLen) {
        z_stream strm  = {0};
        strm.total_in  = strm.avail_in  = srcLen;
        strm.total_out = strm.avail_out = dstLen;
        strm.next_in   = (Bytef *) src;
        strm.next_out  = (Bytef *) dst;

        strm.zalloc = Z_NULL;
        strm.zfree  = Z_NULL;
        strm.opaque = Z_NULL;

        int err = -1;
        unsigned long ret = -1;

        err = inflateInit2(&strm, (15 + 32)); //15 window bits, and the +32 tells zlib to to detect if using gzip or zlib
        if (err == Z_OK) {
            err = inflate(&strm, Z_FINISH);
            if (err == Z_STREAM_END) {
                ret = strm.total_out;
            }
            else {
                 inflateEnd(&strm);
                 return err;
            }
        }
        else {
            inflateEnd(&strm);
            return err;
        }

        inflateEnd(&strm);
        return ret;
    }

    unsigned char* ReadCompressed() {
        unsigned int compressed_size = ReadUInt32() - 4; // 0x47 = 71 - 4 = 67
        unsigned int uncompressed_size = ReadUInt32BE(); // 0x200 = 512

        unsigned char* out = (unsigned char*)malloc(uncompressed_size);
        Decompress(out, uncompressed_size, ptr, compressed_size);

        ptr += compressed_size;
        distance += compressed_size;

        return out;
    }

    unsigned long Distance() {
        return distance;
    }

    IStream GetCompressedStream() {
        return IStream(ReadCompressed());
    }
};

#endif // ISTREAM_H
