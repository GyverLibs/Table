#include <Arduino.h>
#include <LittleFS.h>
#include <TableFile.h>

TableFile t(&LittleFS, "/table.tbl");

void setup() {
    Serial.begin(115200);
    Serial.println();

#ifdef ESP32
    LittleFS.begin(true);
#else
    LittleFS.begin();
#endif

    // прочитать таблицу в оперативку
    t.begin();

    // инициализация кол-ва столбцов и типов. Сбросит таблицу, если не соответствует
    t.init(3, cell_t::Uint32, cell_t::Int16, cell_t::Float);

    // добавить строку
    t.append(random(100), random(100) - 50, (random(100) - 50) / 2.0);
}

void loop() {
    // запишется в файл при изменении
    t.tick();
}