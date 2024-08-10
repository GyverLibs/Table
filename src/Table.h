#pragma once
#include <Arduino.h>

#include "core/row.h"
#include "core/table_t.h"

class Table : public tbl::table_t {
   public:
    using tbl::table_t::table_t;

    // строк, столбцов, типы данных ячеек
    Table(uint16_t rows, uint8_t cols, ...) {
        if (!_types.resize(cols)) goto error;
        if (!_shifts.resize(cols)) goto error;
        _rowSize = 0;

        va_list types;
        va_start(types, cols);
        for (uint16_t col = 0; col < cols; col++) {
            _types[col] = va_arg(types, int);
            _shifts[col] = _rowSize;
            _rowSize += _cellSize(col);
        }
        va_end(types);
        if (!resize(rows)) goto error;
        return;

    error:
        reset();
    }

    ~Table() {
        reset();
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

    // получить ячейку
    inline tbl::Cell get(int row, uint8_t col) {
        return get(row)[col];
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
    String toCSV(char separator = ';', uint8_t dec = 2) {
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
};