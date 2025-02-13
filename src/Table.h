#pragma once
#include <Arduino.h>

#include "./core/row.h"
#include "./core/table_t.h"

class Table : public tbl::table_t {
   public:
    // создать пустую таблицу
    Table() {}

    // строк, столбцов, типы данных ячеек
    Table(uint16_t rows, uint8_t cols, ...) {
        va_list types;
        va_start(types, cols);
        _create(rows, cols, types);
        va_end(types);
    }

    ~Table() {
        reset();
    }

    // создать таблицу (строк, столбцов, типы данных ячеек)
    bool create(uint16_t rows, uint8_t cols, ...) {
        va_list types;
        va_start(types, cols);
        bool res = _create(rows, cols, types);
        va_end(types);
        return res;
    }

    // инициализировать количество и типы столбцов (не изменит таблицу если совпадает)
    bool init(uint8_t cols, ...) {
        bool create = true;

        if (_types.size() == cols) {
            create = false;
            va_list types;
            va_start(types, cols);
            for (uint8_t col = 0; col < cols; col++) {
                if (_types[col] != va_arg(types, int)) {
                    create = true;
                    break;
                }
            }
            va_end(types);
        }

        if (create) {
            va_list types;
            va_start(types, cols);
            bool res = _create(cols, types);
            va_end(types);
            return res;
        }
        return true;
    }

    // получить строку таблицы. Отрицательные числа - получить с конца
    tbl::Row operator[](int row) {
        return get(row);
    }

    // получить строку таблицы. Отрицательные числа - получить с конца
    tbl::Row get(int row) {
        if (row < 0) row += rows();
        if (row < 0) row = 0;
        return tbl::Row(row, *this);
    }

    // добавить строку со значениями в конец
    template <typename... Args>
    bool append(Args... args) {
        if (add()) {
            get(-1).write(args...);
            return true;
        }
        return false;
    }

    // сместить таблицу вверх и записать значения в конец
    template <typename... Args>
    void shift(Args... args) {
        scrollUp();
        get(-1).write(args...);
    }

    // получить ячейку
    inline tbl::Cell get(int row, uint8_t col) {
        return get(row)[col];
    }

    // дублировать последнюю строку и добавить в конец
    bool dupLast() {
        if (!rows() || !add()) return false;
        uint8_t* p = _data.buf() + _data.size();
        memcpy(p - _rowSize, p - _rowSize * 2, _rowSize);
        return true;
    }

    // вывести таблицу в print
    void dump(Print& p) {
        p.print('\t');
        for (size_t col = 0; col < cols(); col++) {
            if (col) p.print('\t');
            switch ((cell_t)_types[col]) {
                case cell_t::None: p.print(F("None")); break;
                case cell_t::Int8: p.print(F("Int8")); break;
                case cell_t::Uint8: p.print(F("Uint8")); break;
                case cell_t::Int16: p.print(F("Int16")); break;
                case cell_t::Uint16: p.print(F("Uint16")); break;
                case cell_t::Int32: p.print(F("Int32")); break;
                case cell_t::Uint32: p.print(F("Uint32")); break;
                case cell_t::Int64: p.print(F("Int64")); break;
                case cell_t::Uint64: p.print(F("Uint64")); break;
                case cell_t::Float: p.print(F("Float")); break;
            }
        }
        p.println();

        for (size_t row = 0; row < rows(); row++) {
            p.print(row);
            p.print('.');
            p.print('\t');
            for (size_t col = 0; col < cols(); col++) {
                p.print(get(row, col));
                p.print('\t');
            }
            p.println();
        }
    }

    // вывести как CSV
    String toCSV(char separator = ';', unsigned int dec = 2) {
        String s;
        s.reserve(rows() * cols() * 4);
        for (size_t row = 0; row < rows(); row++) {
            if (row) s += "\r\n";
            for (size_t col = 0; col < cols(); col++) {
                if (col) s += separator;
                tbl::Cell cell = get(row, col);
                if (cell.type() == cell_t::Float) {
                    if (dec == 2) s += cell.toFloat();
                    else s += String(cell.toFloat(), dec);
                } else {
                    s += cell.toInt();
                }
            }
        }
        return s;
    }

   private:
    using table_t::_cellP;
    using table_t::_change;

    bool _create(uint16_t rows, uint8_t cols, va_list types) {
        return _create(cols, types) ? resize(rows) : false;
    }
    bool _create(uint8_t cols, va_list types) {
        reset();
        if (_types.resize(cols) && _shifts.resize(cols)) {
            _rowSize = 0;
            for (uint8_t col = 0; col < cols; col++) {
                _types[col] = va_arg(types, int);
                _shifts[col] = _rowSize;
                _rowSize += _cellSize(col);
            }
            _change();
            return true;
        }
        reset();
        return false;
    }
};