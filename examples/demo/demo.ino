#include <Arduino.h>
#include <Table.h>

void setup() {
    Serial.begin(115200);
    // 4 строки, 3 столбца
    Table table(4, 3, cell_t::Int8, cell_t::Uint8, cell_t::Float);

    // запись в первую строку [строка][столбец]
    table[0][0] = -123;
    table[0][1] = 123;
    table[0][2] = 123.456;

    // запись в последнюю строку в ячейку 0 (отрицательные строки - с конца таблицы)
    table[-1][0] = -123;

    // запись всей строки сразу (функция принимает любое кол-во аргументов)
    table[1].write(-123, 123, -123.456);

    // вывод таблицы
    table.dump(Serial);

    // вывод ячеек
    Serial.println(table[0][0]);  // печатается
    int8_t v = table[0][1];       // авто конвертация
    table[0][2].toFloat();        // ручная конвертация
    (int32_t)table[0][2];         // ручная конвертация

    // изменение ячеек
    // любые операции сравнения и операторы
    table[0][0] == 3;
    table[0][0] > 3;
    table[0][0] *= 3;
    table[0][0]++;

    // ================================================
    // динамическое добавление строк

    // таблица 3 столбца без строк
    Table t2;
    t2.init(3, cell_t::Int8, cell_t::Uint8, cell_t::Float);

    // добавляем строки с данными
    t2.append(1, 2, 3);
    t2.append(4, 5, 6);

    t2.dump(Serial);

    // ================================================
    // строковые форматы

    // 1 строки, 3 столбца
    Table t3(1, 3, cell_t::Char, cell_t::Char8, cell_t::Char16);

    // row 0
    t3[0][0] = 'a';
    t3[0][1] = "char8";
    t3[0][2] = "char16";

    // row 1
    t3.append('b', "string", "another string");

    t3.dump(Serial);

    Serial.println(t3[0][2] == "char16");  // true
    Serial.println(t3[0][2] == "char17");  // false
}

void loop() {
}