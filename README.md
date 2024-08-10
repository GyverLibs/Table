[![latest](https://img.shields.io/github/v/release/GyverLibs/Table.svg?color=brightgreen)](https://github.com/GyverLibs/Table/releases/latest/download/Table.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/Table.svg)](https://registry.platformio.org/libraries/gyverlibs/Table)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/Table?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# Table
Динамическая таблица для Arduino
- Поддерживает все численные типы данных в любом сочетании
- Динамическое добавление строк, прокрутка и прочие удобные фичи для ведения логов
- Автоматическая запись в файл при изменении (esp)

### Совместимость
Совместима со всеми Arduino платформами (используются Arduino-функции)

### Зависимости
- GTL
- StreamIO

## Содержание
- [Документация](#docs)
- [Примеры](#examples)
- [Версии](#versions)
- [Установка](#install)
- [Баги и обратная связь](#feedback)

<a id="docs"></a>

## Документация
### cell_t
Тип данных ячейки
```cpp
cell_t::None
cell_t::Int8
cell_t::Uint8
cell_t::Int16
cell_t::Uint16
cell_t::Int32
cell_t::Uint32
cell_t::Float
```

### Table
```cpp
Table();

// строк, столбцов, типы данных ячеек
Table(uint16_t rows, uint8_t cols, ...);

// получить строку таблицы. Отрицательные числа - получить с конца
tbl::Row operator[](int row);

// получить строку таблицы. Отрицательные числа - получить с конца
tbl::Row get(int row);

// получить ячейку
tbl::Cell get(int row, uint8_t col);

// вывести таблицу в print
void dump(Print& p);

// вывести как CSV
String toCSV(char separator = ';', uint8_t dec = 2);

// количество строк
uint16_t rows();

// количество столбцов
uint8_t cols();

// очистить ячейки (установить 0)
void clear();

// хоть одна из ячеек была изменена. Автосброс
bool changed();

// изменить количество строк
bool resize(uint16_t rows);

// зарезервировать количество строк
bool reserve(uint16_t rows);

// добавить строку снизу
bool addRow();

// прокрутить таблицу вверх на 1 строку
void shiftUp();

// прокрутить таблицу вниз на 1 строку
void shiftDown();

// полностью освободить память
void reset();

// экспортный размер таблицы (для writeTo)
size_t writeSize();

// экспортировать таблицу в size_t write(uint8_t*, size_t)
template <typename T>
bool writeTo(T& writer);

// экспортировать таблицу в Stream (напр. файл)
bool writeTo(Stream& stream);

// экспортировать таблицу в буфер размера writeSize()
bool writeTo(uint8_t* buffer);

// импортировать таблицу из Stream (напр. файл)
bool readFrom(Stream& stream, size_t len);

// импортировать таблицу из буфера
bool readFrom(const uint8_t* buffer, size_t len);

// тип ячейки
cell_t type(uint16_t row, uint8_t col);
```

### TableFile
Наследует Table. Автоматическая запись в файл при изменении по таймауту
```cpp
// указать файловую систему, путь к файлу и таймаут в мс
TableFile(fs::FS* nfs = nullptr, const char* path = nullptr, uint32_t tout = 10000);

// установить файловую систему и имя файла
void setFS(fs::FS* nfs, const char* path);

// установить таймаут записи, мс (умолч. 10000)
void setTimeout(uint32_t tout = 10000);

// прочитать данные
bool begin();

// обновить данные в файле
bool update();

// тикер, вызывать в loop. Сам обновит данные при изменении и выходе таймаута, вернёт true
bool tick();
```

### Row
строка таблицы
```cpp
// доступ к ячейке
Cell operator[](uint8_t col);

// записать в строку
template <typename... Args>
void write(Args... args);
```

### Cell
Ячейка таблицы
```cpp
// тип ячейки
cell_t type();

// напечатать в print
size_t printTo(Print& p);

// присвоить любой тип
template <typename T>
T operator=(T val);

// в int32
int32_t toInt();

// в float
float toFloat();

// а также операторы сравнения и изменения
```

<a id="examples"></a>

## Примеры
Базовый пример
```cpp
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
Serial.println(table[0][0]);    // печатается
int8_t v = table[0][1];         // авто конвертация
table[0][2].toFloat();          // ручная конвертация
(int32_t)table[0][2];           // ручная конвертация

// изменение ячеек
// любые операции сравнения и операторы
table[0][0] == 3;
table[0][0] > 3;
table[0][0] *= 3;
table[0][0]++;
```

Пример ведения лога: запись всегда в последнюю строку с динамическим увеличением таблицы (до 5), затем перемотка таблицы при каждой новой записи:
```cpp
// изначально 0 строк
Table t(0, 2, Celltype::Int8);

// можно зарезервировать на свой максимум
// t.reserve(5);

for (int i = 0; i < 10; i++) {
    if (t.rows() < 5) t.addRow();  // добавление строки
    else t.shiftUp();              // перемотка (макс. достигнут)
    t[-1][0] = i;                  // запись в последнюю строку
}
t.dump(Serial);
```

Пример с TableFile
```cpp
#include <LittleFS.h>
#include <TableFile.h>
TableFile tb(&LittleFS, "table.tbl");

void setup() {
    LittleFS.begin();
    tb.begin(); // прочитать данные из файла

    tb[0][0] = 123;
}

void loop() {
    // вызывать тикер в лупе
    tb.tick();
}
```

<a id="versions"></a>

## Версии
- v1.0

<a id="install"></a>
## Установка
- Библиотеку можно найти по названию **Table** и установить через менеджер библиотек в:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Скачать библиотеку](https://github.com/GyverLibs/Table/archive/refs/heads/main.zip) .zip архивом для ручной установки:
    - Распаковать и положить в *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Распаковать и положить в *C:\Program Files\Arduino\libraries* (Windows x32)
    - Распаковать и положить в *Документы/Arduino/libraries/*
    - (Arduino IDE) автоматическая установка из .zip: *Скетч/Подключить библиотеку/Добавить .ZIP библиотеку…* и указать скачанный архив
- Читай более подробную инструкцию по установке библиотек [здесь](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Обновление
- Рекомендую всегда обновлять библиотеку: в новых версиях исправляются ошибки и баги, а также проводится оптимизация и добавляются новые фичи
- Через менеджер библиотек IDE: найти библиотеку как при установке и нажать "Обновить"
- Вручную: **удалить папку со старой версией**, а затем положить на её место новую. "Замену" делать нельзя: иногда в новых версиях удаляются файлы, которые останутся при замене и могут привести к ошибкам!

<a id="feedback"></a>

## Баги и обратная связь
При нахождении багов создавайте **Issue**, а лучше сразу пишите на почту [alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
Библиотека открыта для доработки и ваших **Pull Request**'ов!

При сообщении о багах или некорректной работе библиотеки нужно обязательно указывать:
- Версия библиотеки
- Какой используется МК
- Версия SDK (для ESP)
- Версия Arduino IDE
- Корректно ли работают ли встроенные примеры, в которых используются функции и конструкции, приводящие к багу в вашем коде
- Какой код загружался, какая работа от него ожидалась и как он работает в реальности
- В идеале приложить минимальный код, в котором наблюдается баг. Не полотно из тысячи строк, а минимальный код