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
        switch (type()) {
            case cell_t::Int8: return p.print((int8_t)_get8());
            case cell_t::Uint8: return p.print((uint8_t)_get8());
            case cell_t::Int16: return p.print((int16_t)_get16());
            case cell_t::Uint16: return p.print((uint16_t)_get16());
            case cell_t::Int32: return p.print((int32_t)_get32());
            case cell_t::Uint32: return p.print((uint32_t)_get32());
            case cell_t::Int64: return p.print((int32_t)_get64());
            case cell_t::Uint64: return p.print((uint32_t)_get64());
            case cell_t::Float: return p.print((float)_getF());
            default: break;
        }
        return 0;
    }

    template <typename T>
    T operator=(T val) {
        switch (type()) {
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

    int32_t toInt() const {
        switch (type()) {
            case cell_t::Int8:
            case cell_t::Uint8:
                return _get8();

            case cell_t::Int16:
            case cell_t::Uint16:
                return _get16();

            case cell_t::Int32:
            case cell_t::Uint32:
                return _get32();

            case cell_t::Int64:
            case cell_t::Uint64:
                return _get64();

            case cell_t::Float:
                return _getF();

            default:
                break;
        }
        return 0;
    }

    float toFloat() const {
        return (type() == cell_t::Float) ? _getF() : toInt();
    }

    int64_t toInt64() const {
        switch (type()) {
            case cell_t::Int64:
            case cell_t::Uint64:
                return _get64();

            default:
                break;
        }
        return toInt();
    }

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

    // compare
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

   private:
    uint16_t row;
    uint8_t col;
    table_t& t;

    inline void* _p() const {
        return t._cellP(row, col);
    }
    uint8_t _get8() const {
        uint8_t val;
        memcpy(&val, _p(), 1);
        return val;
    }
    uint16_t _get16() const {
        uint16_t val;
        memcpy(&val, _p(), 2);
        return val;
    }
    uint32_t _get32() const {
        uint32_t val;
        memcpy(&val, _p(), 4);
        return val;
    }
    uint64_t _get64() const {
        uint64_t val;
        memcpy(&val, _p(), 8);
        return val;
    }
    float _getF() const {
        float val;
        memcpy(&val, _p(), 4);
        return val;
    }
    void _write(void* p, uint8_t size) {
        if (memcmp(_p(), p, size)) {
            memcpy(_p(), p, size);
            t._change();
        }
    }
};

}  // namespace tbl