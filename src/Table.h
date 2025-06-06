#pragma once
#include <Arduino.h>

#include "./core/row.h"
#include "./core/table_t.h"

class Table : public tbl::table_t {
   public:
    using tbl::table_t::table_t;

    // строк, столбцов, типы данных ячеек
    Table(uint16_t rows, uint8_t cols, ...) {
        va_list types;
        va_start(types, cols);
        _create(rows, cols, types);
        va_end(types);
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

#if _TABLE_USE_FOLD
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
#endif

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
        for (uint16_t col = 0; col < cols(); col++) {
            if (col) p.print('\t');
            p.print(tbl::readType((cell_t)_types[col]));
        }
        p.println();

        for (uint8_t row = 0; row < rows(); row++) {
            p.print(row);
            p.print('.');
            p.print('\t');
            for (uint16_t col = 0; col < cols(); col++) {
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
        for (uint16_t row = 0; row < rows(); row++) {
            if (row) s += "\r\n";
            for (uint8_t col = 0; col < cols(); col++) {
                if (col) s += separator;
                tbl::Cell cell = get(row, col);

                // TABLE_TYPES
                switch (cell.type()) {
                    case cell_t::Float:
                        if (dec == 2) s += cell.toFloat();
                        else s += String(cell.toFloat(), dec);
                        break;

                    case cell_t::Char:
                        s += (char)cell.toInt();
                        break;

                    case cell_t::Char8:
                    case cell_t::Char16:
                    case cell_t::Char32:
                    case cell_t::Char64:
                    case cell_t::Char128:
                    case cell_t::Char256:
                        s += cell.str();
                        break;

                    default:
                        s += cell.toInt();
                        break;
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