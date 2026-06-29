#pragma once
#include <Arduino.h>

#include "table_t.h"

namespace tbl {

class Cell : public Printable {
   public:
    Cell(uint16_t row, uint8_t col, table_t& t) : row(row), col(col), t(t) {}

    // тип ячейки
    cell_t type() const {
        return t.type(row, col);
    }

    // напечатать в print
    size_t printTo(Print& p) const {
        // TABLE_TYPES
        switch (type()) {
            case cell_t::Float: return p.print(_get<float>());
            case cell_t::Int8: return p.print(_get<int8_t>());
            case cell_t::Uint8: return p.print(_get<uint8_t>());
            case cell_t::Int16: return p.print(_get<int16_t>());
            case cell_t::Uint16: return p.print(_get<uint16_t>());
            case cell_t::Int32: return p.print(_get<int32_t>());
            case cell_t::Uint32: return p.print(_get<uint32_t>());
            case cell_t::Unix: return p.print(_get<uint32_t>());
#if defined(ESP32) || defined(ESP8266)
            case cell_t::Int64: return p.print(_get<int64_t>());
            case cell_t::Uint64: return p.print(_get<uint64_t>());
#else
            case cell_t::Int64: return p.print((int32_t)_get<int64_t>());
            case cell_t::Uint64: return p.print((uint32_t)_get<uint64_t>());
#endif
            case cell_t::Char: return p.print(_get<char>());
            case cell_t::Char8:
            case cell_t::Char16:
            case cell_t::Char32:
            case cell_t::Char64:
            case cell_t::Char128:
            case cell_t::Char254:
                return p.print(str());
            default: break;
        }
        return 0;
    }

    template <typename T>
    T operator=(T val) {
        // TABLE_TYPES
        switch (type()) {
            case cell_t::Char:
            case cell_t::Int8:
            case cell_t::Uint8: {
                uint8_t v = val;
                _write(&v, 1);
            } break;
            case cell_t::Int16:
            case cell_t::Uint16: {
                uint16_t v = val;
                _write(&v, 2);
            } break;
            case cell_t::Unix:
            case cell_t::Int32:
            case cell_t::Uint32: {
                uint32_t v = val;
                _write(&v, 4);
            } break;
            case cell_t::Int64:
            case cell_t::Uint64: {
                uint64_t v = val;
                _write(&v, 8);
            } break;
            case cell_t::Float: {
                float v = val;
                _write(&v, 4);
            } break;
            default:
                break;
        }
        return val;
    }

    void operator=(const String& val) {
        *this = val.c_str();
    }
    void operator=(char* val) {
        *this = (const char*)val;
    }
    void operator=(const char* val) {
        // TABLE_TYPES
        switch (type()) {
            case cell_t::Char8:
            case cell_t::Char16:
            case cell_t::Char32:
            case cell_t::Char64:
            case cell_t::Char128:
            case cell_t::Char254: {
                size_t len = typeSize(type());
                strncpy(str(), val, len - 1);
                str()[len - 1] = 0;
            } break;

            default:
                break;
        }
    }

    int32_t toInt() const {
        // TABLE_TYPES
        switch (type()) {
            case cell_t::Int8:
                return _get<int8_t>();

            case cell_t::Char:
            case cell_t::Uint8:
                return _get<uint8_t>();

            case cell_t::Int16:
                return _get<int16_t>();

            case cell_t::Uint16:
                return _get<uint16_t>();

            case cell_t::Int32:
                return _get<int32_t>();

            case cell_t::Uint32:
            case cell_t::Unix:
                return _get<uint32_t>();

            case cell_t::Int64:
                return _get<int64_t>();

            case cell_t::Uint64:
                return _get<uint64_t>();

            case cell_t::Float:
                return _get<float>();

            default:
                break;
        }
        return 0;
    }

    float toFloat() const {
        return (type() == cell_t::Float) ? _get<float>() : toInt();
    }

    int64_t toInt64() const {
        switch (type()) {
            case cell_t::Int64:
                return _get<int64_t>();

            case cell_t::Uint64:
                return _get<uint64_t>();

            default:
                break;
        }
        return toInt();
    }

    // TABLE_TYPES
    template <typename T>
    operator T() {
        return toInt();
    }
    operator float() {
        return toFloat();
    }
    operator double() {
        return toFloat();
    }
    operator int64_t() {
        return toInt64();
    }
    operator uint64_t() {
        return toInt64();
    }
    operator char*() {
        return str();
    }

    // compare
    bool operator==(const char* val) {
        switch (type()) {
            case cell_t::Char8:
            case cell_t::Char16:
            case cell_t::Char32:
            case cell_t::Char64:
            case cell_t::Char128:
            case cell_t::Char254:
                return !strcmp(str(), val);
            default: break;
        }
        return false;
    }
    bool operator!=(const char* val) {
        return !(*this == val);
    }
    bool operator==(char* val) {
        return !(*this == (const char*)val);
    }
    bool operator!=(char* val) {
        return !(*this == val);
    }

    template <typename T>
    bool operator==(T val) {
        return (T)(*this) == val;
    }
    template <typename T>
    bool operator!=(T val) {
        return (T)(*this) != val;
    }
    template <typename T>
    bool operator>=(T val) {
        return (T)(*this) >= val;
    }
    template <typename T>
    bool operator>(T val) {
        return (T)(*this) > val;
    }
    template <typename T>
    bool operator<=(T val) {
        return (T)(*this) <= val;
    }
    template <typename T>
    bool operator<(T val) {
        return (T)(*this) < val;
    }

    // change
    template <typename T>
    T operator++(T val) {
        return *this = (T)(*this) + 1;
    }
    template <typename T>
    T operator--(T val) {
        return *this = (T)(*this) - 1;
    }

    template <typename T>
    T operator+=(T val) {
        return *this = (T)(*this) + val;
    }
    template <typename T>
    T operator-=(T val) {
        return *this = (T)(*this) - val;
    }
    template <typename T>
    T operator*=(T val) {
        return *this = (T)(*this) * val;
    }
    template <typename T>
    T operator/=(T val) {
        return *this = (T)(*this) / val;
    }
    template <typename T>
    T operator%=(T val) {
        return *this = (T)(*this) % val;
    }
    template <typename T>
    T operator&=(T val) {
        return *this = (T)(*this) & val;
    }
    template <typename T>
    T operator|=(T val) {
        return *this = (T)(*this) | val;
    }
    template <typename T>
    T operator^=(T val) {
        return *this = (T)(*this) ^ val;
    }

    // указатель на данные
    inline void* buf() const {
        return t._cellP(row, col);
    }

    // указатель на данные (строку)
    inline char* str() const {
        return (char*)buf();
    }

   private:
    uint16_t row;
    uint8_t col;
    table_t& t;

    template <typename T>
    T _get() const {
        T val;
        memcpy(&val, buf(), sizeof(T));
        return val;
    }
    void _write(void* p, uint8_t size) {
        if (memcmp(buf(), p, size)) {
            memcpy(buf(), p, size);
            t._change();
        }
    }
};

}  // namespace tbl
