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

#if _TABLE_USE_FOLD
    // записать в строку
    template <typename... Args>
    void write(Args... args) {
        _idx = 0;
        (_write(args), ...);
    }
#endif

   private:
    uint16_t row;
    tbl::table_t& t;
    uint8_t _idx = 0;

    template <typename T>
    void _write(T arg) {
        if (_idx < t.cols()) Cell(row, _idx++, t) = arg;
    }
};

}  // namespace tbl