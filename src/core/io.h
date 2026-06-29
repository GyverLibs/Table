#pragma once
#include <Arduino.h>

namespace tbl {

class Reader {
   public:
    Reader(Stream& stream, size_t len) : _stream(&stream), _len(len) {}
    Reader(const uint8_t* buffer, size_t len) : _buffer(buffer), _len(len) {}

    bool read(void* dest, size_t size) {
        if (size > _len) return false;
        _len -= size;

        if (_stream) return _stream->readBytes((uint8_t*)dest, size) == size;
        if (_buffer) {
            memcpy(dest, _buffer, size);
            _buffer += size;
            return true;
        }
        return false;
    }

    template <typename T>
    bool read(T& value) {
        return read(&value, sizeof(value));
    }

   private:
    Stream* _stream = nullptr;
    const uint8_t* _buffer = nullptr;
    size_t _len;
};

class BufferWriter {
   public:
    BufferWriter(uint8_t* buffer) : _buffer(buffer) {}

    size_t write(const uint8_t* data, size_t size) {
        memcpy(_buffer, data, size);
        _buffer += size;
        return size;
    }

   private:
    uint8_t* _buffer;
};

}  // namespace tbl
