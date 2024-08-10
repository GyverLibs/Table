#pragma once
#include <Arduino.h>
#include <GTL.h>
#include <StreamIO.h>

enum class cell_t : int {
    None,
    Int8,
    Uint8,
    Int16,
    Uint16,
    Int32,
    Uint32,
    Float,
};

namespace tbl {

class table_t {
   public:
    table_t() {}

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
    }

    // хоть одна из ячеек была изменена. Автосброс
    bool changed() {
        return _changeF ? _changeF = false, 1 : 0;
    }

    // изменить количество строк
    bool resize(uint16_t rows) {
        if (reserve(rows)) {
            if (rows > _rows) {
                memset(_data.buf() + _rows * _rowSize, 0x00, (rows - _rows) * _rowSize);
            }
            _rows = rows;
            return 1;
        }
        return 0;
    }

    // зарезервировать количество строк
    bool reserve(uint16_t rows) {
        return cols() ? _data.resize(rows * _rowSize) : 0;
    }

    // добавить строку снизу
    bool addRow() {
        return resize(rows() + 1);
    }

    // прокрутить таблицу вверх на 1 строку
    void shiftUp() {
        _shift(true);
    }

    // прокрутить таблицу вниз на 1 строку
    void shiftDown() {
        _shift(false);
    }

    // полностью освободить память
    void reset() {
        _data.reset();
        _shifts.reset();
        _types.reset();
        _changeF = 0;
        _rowSize = 0;
        _rows = 0;
    }

    // экспортный размер таблицы (для writeTo)
    size_t writeSize() {
        return 1 + 2 + _types.size() + (_rows * _rowSize);
    }

    // экспортировать таблицу в size_t write(uint8_t*, size_t)
    template <typename T>
    bool writeTo(T& writer) {
        uint8_t cl = cols();
        uint16_t rw = rows();
        size_t writed = 0;
        writed += writer.write((uint8_t*)&cl, (size_t)1);
        writed += writer.write((uint8_t*)&rw, (size_t)2);
        writed += writer.write(_types.buf(), _types.size());
        writed += writer.write(_data.buf(), _rows * _rowSize);
        return writed == writeSize();
    }

    // экспортировать таблицу в Stream (напр. файл)
    bool writeTo(Stream& stream) {
        return _writeTo(Writer(stream));
    }

    // экспортировать таблицу в буфер размера writeSize()
    bool writeTo(uint8_t* buffer) {
        return _writeTo(Writer(buffer));
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

    table_t(table_t& val) {
        move(val);
    }
    table_t& operator=(table_t& val) {
        move(val);
        return *this;
    }

#if __cplusplus >= 201103L
    table_t(table_t&& rval) noexcept {
        move(rval);
    }
    table_t& operator=(table_t&& rval) noexcept {
        move(rval);
        return *this;
    }
#endif

    void move(table_t& rval) noexcept {
        _shifts.move(rval._shifts);
        _types.move(rval._types);
        _data.move(rval._data);
        _changeF = rval._changeF;
        _rowSize = rval._rowSize;
        _rows = rval._rows;
        rval.reset();
    }

    void* _cellP(uint16_t row, uint8_t col) {
        return _data.buf() + row * _rowSize + _shifts[col];
    }

    void _change() {
        _changeF = true;
    }

   protected:
    gtl::array_uniq<uint8_t> _types;
    gtl::array_uniq<uint8_t> _shifts;
    gtl::array_uniq<uint8_t> _data;
    uint16_t _rowSize = 0;
    uint16_t _rows = 0;
    bool _changeF = 0;

    void _shift(bool up) {
        if (_rows) memmove(_data.buf() + (_rowSize * (!up)), _data.buf() + (_rowSize * up), (_rows - 1) * _rowSize);
    }

    size_t _cellSize(uint8_t col) {
        if (col >= cols()) return 0;
        switch ((cell_t)_types[col]) {
            case cell_t::Int8:
            case cell_t::Uint8:
                return 1;
            case cell_t::Int16:
            case cell_t::Uint16:
                return 2;
            case cell_t::Int32:
            case cell_t::Uint32:
            case cell_t::Float:
                return 4;
            default:
                break;
        }
        return 0;
    }

    bool _writeTo(Writer writer) {
        // [cols 1b] [rows 2b] [types..] [data..]
        writer.write((uint8_t)cols());
        writer.write((uint16_t)rows());
        writer.write(_types.buf(), _types.size());
        writer.write(_data.buf(), _rows * _rowSize);
        return writer.writed() == writeSize();
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
        return 1;

    error:
        reset();
        return 0;
    }
};

}  // namespace tbl