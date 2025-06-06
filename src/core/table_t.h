#pragma once
#include <Arduino.h>
#include <GTL.h>
#include <StreamIO.h>

// TABLE_TYPES
enum class cell_t : int {
    None,
    Int8,
    Uint8,
    Int16,
    Uint16,
    Int32,
    Uint32,
    Float,
    Int64,
    Uint64,
    Unix,
    Char,
    Char8,
    Char16,
    Char32,
    Char64,
    Char128,
    Char256,
};

#define _TABLE_USE_FOLD (__cplusplus >= 201703L || defined(TABLE_USE_FOLD) || defined(ESP32) || defined(__AVR__))

namespace tbl {

static const __FlashStringHelper* readType(cell_t type) {
    // TABLE_TYPES
    switch (type) {
        case cell_t::Int8: return F("Int8");
        case cell_t::Uint8: return F("Uint8");
        case cell_t::Int16: return F("Int16");
        case cell_t::Uint16: return F("Uint16");
        case cell_t::Int32: return F("Int32");
        case cell_t::Uint32: return F("Uint32");
        case cell_t::Int64: return F("Int64");
        case cell_t::Uint64: return F("Uint64");
        case cell_t::Float: return F("Float");
        case cell_t::Unix: return F("Unix");
        case cell_t::Char: return F("Char");
        case cell_t::Char8: return F("Char8");
        case cell_t::Char16: return F("Char16");
        case cell_t::Char32: return F("Char32");
        case cell_t::Char64: return F("Char64");
        case cell_t::Char128: return F("Char128");
        case cell_t::Char256: return F("Char256");
        default: break;
    }
    return F("None");
}

static size_t typeSize(cell_t type) {
    // TABLE_TYPES
    switch (type) {
        case cell_t::Char:
        case cell_t::Int8:
        case cell_t::Uint8:
            return 1;
        case cell_t::Int16:
        case cell_t::Uint16:
            return 2;
        case cell_t::Unix:
        case cell_t::Int32:
        case cell_t::Uint32:
        case cell_t::Float:
            return 4;
        case cell_t::Int64:
        case cell_t::Uint64:
            return 8;
        case cell_t::Char8: return 9;
        case cell_t::Char16: return 17;
        case cell_t::Char32: return 33;
        case cell_t::Char64: return 65;
        case cell_t::Char128: return 129;
        case cell_t::Char256: return 257;
        default:
            break;
    }
    return 0;
}

class table_t {
   public:
    // количество строк
    inline uint16_t rows() {
        return _rows;
    }

    // количество столбцов
    inline uint8_t cols() {
        return _types.size();
    }

    // очистить ячейки (установить 0)
    void clear() {
        _data.clear();
        _change();
    }

    // хоть одна из ячеек была изменена
    bool changed() {
        return _changed;
    }

    // сброс состояния changed()
    void clearChanged() {
        _changed = false;
    }

    // изменить количество строк
    bool resize(uint16_t rows) {
        if (reserve(rows)) {
            if (rows > _rows) {
                memset(_data.buf() + _rows * _rowSize, 0x00, (rows - _rows) * _rowSize);
            }
            _rows = rows;
            _change();
            return 1;
        }
        return 0;
    }

    // зарезервировать количество строк
    bool reserve(uint16_t rows) {
        return cols() ? _data.resize(rows * _rowSize) : 0;
    }

    // установить лимит кол-ва строк для add/append, будет прокручивать при превышении. 0 - отключить
    void setLimit(uint16_t limit) {
        _limit = limit;
    }

    // добавить строку в конец
    bool add() {
        if (_limit && rows() >= _limit) {
            scrollUp();
            return true;
        }
        return resize(rows() + 1);
    }

    // удалить строку. Отрицательные - с конца
    bool remove(int row) {
        if (!rows()) return false;
        if (row < 0) row += rows();
        if (row < 0) return false;

        uint8_t* p = _data.buf() + row * _rowSize;
        memmove(p, p + _rowSize, (rows() - row - 1) * _rowSize);
        _rows--;
        _change();
        return true;
    }

    // удалить все строки
    void removeAll() {
        _rows = 0;
        _change();
    }

    // прокрутить строки вверх на 1 строку
    void scrollUp() {
        _scroll(true);
    }

    // прокрутить строки вниз на 1 строку
    void scrollDown() {
        _scroll(false);
    }

    // полностью освободить память
    void reset() {
        _data.reset();
        _shifts.reset();
        _types.reset();
        _rowSize = 0;
        _rows = 0;
        _change();
    }

    // экспортный размер таблицы (для writeTo)
    size_t writeSize() {
        return 1 + 2 + _types.size() + (_rows * _rowSize);
    }

    // экспортировать таблицу в size_t write(uint8_t*, size_t)
    template <typename T>
    bool writeTo(T& writer) {
        // [cols 1b] [rows 2b] [types..] [data..]
        size_t wr = 0;
        uint8_t cl = cols();
        uint16_t rw = rows();
        wr += writer.write((uint8_t*)&cl, 1);
        wr += writer.write((uint8_t*)&rw, 2);
        wr += writer.write(_types.buf(), _types.size());
        wr += writer.write(_data.buf(), _rows * _rowSize);
        return wr == writeSize();
    }

    // экспортировать таблицу в буфер размера writeSize()
    bool writeTo(uint8_t* buffer) {
        Writer wr(buffer);
        return writeTo(wr);
    }

    // импортировать таблицу из Stream (напр. файл)
    bool readFrom(Stream& stream, size_t len) {
        return _readFrom(Reader(stream, len));
    }

    // импортировать таблицу из буфера
    bool readFrom(const uint8_t* buffer, size_t len) {
        return _readFrom(Reader(buffer, len));
    }

    // тип ячейки
    cell_t type(uint16_t row, uint8_t col) {
        return (row < rows() && col < cols()) ? (cell_t)_types[col] : (cell_t::None);
    }

    void* _cellP(uint16_t row, uint8_t col) {
        return _data.buf() + row * _rowSize + _shifts[col];
    }

    void _change() {
        _changed = true;
        _update = true;
    }

   protected:
    gtl::array<uint8_t> _shifts;
    gtl::array<uint8_t> _types;
    gtl::array<uint8_t> _data;
    uint16_t _rowSize = 0;
    uint16_t _limit = 0;
    uint16_t _rows = 0;
    bool _changed = 0;
    bool _update = 0;

    void _scroll(bool up) {
        if (_rows) {
            memmove(_data.buf() + (_rowSize * (!up)), _data.buf() + (_rowSize * up), (_rows - 1) * _rowSize);
            _change();
        }
    }

    inline size_t _cellSize(uint8_t col) {
        return typeSize((cell_t)_types[col]);
    }

    bool _readFrom(Reader reader) {
        uint8_t cols;
        uint16_t rows;
        if (!reader.read(cols)) goto error;
        if (!reader.read(rows)) goto error;
        if (!_types.resize(cols)) goto error;
        if (!_shifts.resize(cols)) goto error;
        if (!reader.read(_types.buf(), cols)) goto error;

        _rowSize = 0;
        for (uint16_t col = 0; col < cols; col++) {
            _shifts[col] = _rowSize;
            _rowSize += _cellSize(col);
        }

        if (!resize(rows)) goto error;
        if (!reader.read(_data.buf(), rows * _rowSize)) goto error;
        _changed = true;
        return 1;

    error:
        reset();
        return 0;
    }
};

}  // namespace tbl