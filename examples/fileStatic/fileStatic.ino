#include <Arduino.h>
#include <LittleFS.h>
#include <TableFileStatic.h>

void setup() {
    Serial.begin(115200);
    Serial.println();

#ifdef ESP32
    LittleFS.begin(true);
#else
    LittleFS.begin();
#endif

    TableFileStatic table(&LittleFS, "/table2.tbl", 5);  // макс. 5 строк с перемоткой

    // info
    auto inf = table.getInfo();
    Serial.println(inf.cols);
    Serial.println(inf.rows);

    // инициализация кол-ва столбцов и типов, если файл ещё не существует
    table.init(3, cell_t::Uint32, cell_t::Int16, cell_t::Float);

    // добавить данные напрямую в файл
    table.append(inf.rows, random(10), random(10) / 10.0);

    // таким образом можно вести лог прямо в файле, не ограничиваясь объёмом оперативной памяти

    // dump
    table.dump(Serial);
}

void loop() {
}