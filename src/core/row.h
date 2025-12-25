#pragma once
#include <Arduino.h>

#include "cell.h"
#include "table_t.h"

namespace tbl {

class Row {
   public:
    Row(uint16_t row, tbl::table_t& t) : row(row), t(t) {}

    // доступ к ячейке
    Cell operator[](uint8_t col) {
        return Cell(row, col, t);
    }

    // записать в строку
    template <typename... Args>
    void write(Args... args) {
        _idx = 0;
        _write(args...);
    }

   private:
    uint16_t row;
    tbl::table_t& t;
    uint8_t _idx = 0;

    void _write() {}

    template <typename T, typename... Rest>
    void _write(T first, Rest... rest) {
        if (_idx >= t.cols()) return;

        Cell(row, _idx++, t) = first;
        _write(rest...);
    }
};

}  // namespace tbl