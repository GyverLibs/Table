#include <Arduino.h>
#include <Table.h>

void setup() {
    Serial.begin(115200);
    // 4 строки, 3 столбца
    Table table(4, 3, cell_t::Int8, cell_t::Uint8, cell_t::Float);

    // запись в первую строку
    table[0][0] = -123;
    table[0][1] = 123;
    table[0][2] = 123.456;

    // запись в последнюю строку
    table[-1][0] = -123;

    // запись всей строки сразу (функция принимает любое кол-во аргументов)
    table[1].write(-123, 123, -123.456);

    // вывод таблицы
    table.dump(Serial);

    // вывод ячеек
    Serial.println(table[0][0]);  // печатается
    int8_t v = table[0][1];       // авто конвертация
    table[0][2].toFloat();        // ручная конвертация
    (int32_t) table[0][2];        // ручная конвертация
}

void loop() {
}