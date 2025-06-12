[![latest](https://img.shields.io/github/v/release/GyverLibs/Table.svg?color=brightgreen)](https://github.com/GyverLibs/Table/releases/latest/download/Table.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/Table.svg)](https://registry.platformio.org/libraries/gyverlibs/Table)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/Table?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# Table
Динамическая бинарная таблица для Arduino
- Поддерживает все численные типы данных, символы и нуль-терминированные строки в любом сочетании
- Динамическое добавление строк, прокрутка и прочие удобные фичи для ведения логов
- Автоматическая запись в файл при изменении (esp)
- Возможность добавления строк без чтения файла
- Вес в среднем в 2 раза меньше, чем у текстовой CSV

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
cell_t::None,
cell_t::Int8,
cell_t::Uint8,
cell_t::Int16,
cell_t::Uint16,
cell_t::Int32,
cell_t::Uint32,
cell_t::Float,
cell_t::Int64,
cell_t::Uint64,
cell_t::Unix,   // uint32
cell_t::Char,   // одиночный символ
cell_t::Char8,  // строка макс. 8 символов
cell_t::Char16,
cell_t::Char32,
cell_t::Char64,
cell_t::Char128,
cell_t::Char256,
```

В строковых типах ячеек указана максимальная длина строки без учёта нулевого символа, т.е. `Char16` может хранить строку до 16 символов включительно - размер буфера у ячейки 17 символов.

### Table
```cpp
Table();

// строк, столбцов, типы данных ячеек
Table(uint16_t rows, uint8_t cols, ...);

// создать таблицу (строк, столбцов, типы данных ячеек)
bool create(uint16_t rows, uint8_t cols, ...);

// инициализировать количество и типы столбцов (не изменит таблицу если совпадает)
bool init(uint8_t cols, ...);

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

// добавить пустую строку в конец
bool add();

// добавить строку со значениями в конец
bool append(...);

// установить лимит кол-ва строк для add/append, будет прокручивать при превышении. 0 - отключить
void setLimit(uint16_t limit);

// сместить таблицу вверх и записать значения в конец
void shift(...);

// удалить строку. Отрицательные - с конца
bool remove(int row);

// удалить все строки
void removeAll();

// дублировать последнюю строку и добавить в конец
bool dupLast();

// прокрутить строки вверх на 1 строку
void scrollUp();

// прокрутить строки вниз на 1 строку
void scrollDown();

// полностью освободить память
void reset();

// экспортный размер таблицы (для writeTo)
size_t writeSize();

// экспортировать таблицу в size_t write(uint8_t*, size_t)
template <typename T>
bool writeTo(T& writer);

// экспортировать таблицу в write(uint8_t, size_t)
bool writeTo(T& stream);

// экспортировать таблицу в буфер размера writeSize()
bool writeTo(uint8_t* buffer);

// импортировать таблицу из Stream (напр. файл)
bool readFrom(Stream& stream, size_t len);

// импортировать таблицу из буфера
bool readFrom(const uint8_t* buffer, size_t len);

// тип ячейки
cell_t type(uint16_t row, uint8_t col);
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

// указатель на данные
void* buf();

// указатель на данные (строку)
char* str();

// а также операторы сравнения и изменения
```

К строковым ячейкам корректно присваиваются `const char*` строки, а также сравниваются с ними через `==` и `!=`.

### TableFile
Наследует `Table`. Автоматическая запись в файл при изменении по таймауту

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

### TableFileStatic
Добавление данных в файл таблицы без чтения самой таблицы в оперативную память. Позволяет вести большие таблицы, т.к. нет лимита на открытие в оперативке. Формат данных такой же, как у обычной таблицы выше, т.е. файл `TableFileStatic` можно открыть как `TableFile`.

```cpp
// указать файловую систему, путь к файлу и макс. кол-во строк (0 - без лимита)
TableFileStatic(fs::FS* nfs, const char* path, uint16_t maxRows = 0);

// получить инфо о таблице
Info getInfo();

// установить макс. количество строк (будет смещаться при append)
void setMaxRows(uint16_t maxRows);

// удалить все строки
bool removeAll();

// вывести таблицу в print
void dump(Print& p);

// инициализировать в файле
bool init(uint8_t cols, ...);

// добавить строку к таблице (сдвинет таблицу, если превышает макс. строк)
bool append(Args... args);

{
    gtl::array<cell_t> types;  // массив типов длиной cols
    uint16_t rows = 0;         // строк
    uint8_t cols = 0;          // столбцов
}
```

При установке лимита на макс. строк и при его превышении таблица будет сдвинута и обрезана под лимит. Для вызова `append()` потребуется свободное место под временный файл размером с установленный лимит - по сути такого же размера как текущая таблица.

<a id="examples"></a>

## Примеры
### Table
```cpp
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

### Строковые типы ячеек
```cpp
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
```

### Динамическая таблица
```cpp
// таблица 3 столбца без ячеек
Table table;
table.init(3, cell_t::Int8, cell_t::Uint8, cell_t::Float);

// добавляем строки с данными
table.append(1, 2, 3);
table.append(4, 5, 6);

table.dump(Serial);
```

### Ограничение строк
Пример ведения ограниченного лога: запись всегда в последнюю строку с динамическим увеличением таблицы до 5, затем перемотка таблицы при каждой новой записи:
```cpp
// изначально 0 строк
Table t(0, 2, cell_t::Int8, cell_t::Float);

// лимит строк 5
t.setLimit(5);

for (int i = 0; i < 10; i++) t.append(i, i / 10.0);

t.dump(Serial);
```

Добавление новой строки как копии последней строки и её изменение
```cpp
Table t(0, 2, cell_t::Int8, cell_t::Float);
t.append(1, 3.14);

for (int i = 0; i < 5; i++) {
    t.dupLast();
    t[-1][0] += 5;
}

t.dump(Serial);
```

### TableFile
```cpp
#include <LittleFS.h>
#include <TableFile.h>
TableFile t(&LittleFS, "table.tbl");

void setup() {
    LittleFS.begin();

    // прочитать таблицу в оперативку
    t.begin();

    // инициализация кол-ва столбцов и типов, если таблица ещё не существует
    t.init(3, cell_t::Uint32, cell_t::Int16, cell_t::Float);

    // добавить строку
    t.append(1, 2, 3.14);
}

void loop() {
    // вызывать тикер в лупе, таблица сама запишется в файл при изменениях
    t.tick();
}
```

### TableFileStatic
Пример с добавления строк в файл без чтения таблицы
```cpp
#include <LittleFS.h>
#include <TableFileStatic.h>

void setup() {
    Serial.begin(115200);

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
```

<a id="versions"></a>

## Версии
- v1.0
- v1.1.0

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