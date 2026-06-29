This is an automatic translation and may be incorrect in some places. See the source README and examples for authoritative information.

[![latest](https://img.shields.io/github/v/release/GyverLibs/Table.svg?color=brightgreen)](https://github.com/GyverLibs/Table/releases/latest/download/Table.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/Table.svg)](https://registry.platformio.org/libraries/gyverlibs/Table)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/Table?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# Table
Dynamic binary table for Arduino
- Supports all numerical data types, symbols and null-terminated strings in any combination
- Dynamic addition of lines, scrolling and other convenient features for logs
- Automatic write to file when changed (esp)
- Ability to add lines without reading the file
- Weight on average 2 times less than text CSV

### Compatibility
Compatible with all Arduino platforms (Arduino features are used)

### Dependencies
- GTL

## Contents
- [Documentation.](#docs)
- [Examples](#examples)
- [Versions](#versions)
- [Installation](#install)
- [Bugs and feedback](#feedback)

<a id="docs"></a>

## Documentation.
### cell_t
Cell data type
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
cell_t::Char,   // singular
cell_t::Char8,  // Max line. 8 characters
cell_t::Char16,
cell_t::Char32,
cell_t::Char64,
cell_t::Char128,
cell_t::Char256,
```

In string types of cells, the maximum length of the string is indicated without taking into account the zero character, i.e., the maximum length of the string is indicated.`Char16`can store a string of up to 16 characters inclusive - the buffer size of the 17-character cell.

### Table
```cpp
Table();

// rows, columns, cell data types
Table(uint16_t rows, uint8_t cols, ...);

// create a table (lines, columns, cell data types)
bool create(uint16_t rows, uint8_t cols, ...);

// initialize the number and types of columns (will not change the table if it matches)
bool init(uint8_t cols, ...);

// Get a row of the table. Negative Numbers: Getting from the End
tbl::Row operator[](int row);

// Get a row of the table. Negative Numbers: Getting from the End
tbl::Row get(int row);

// cellize
tbl::Cell get(int row, uint8_t col);

// print out
void dump(Print& p);

// output
String toCSV(char separator = ';', uint8_t dec = 2);

// lineage
uint16_t rows();

// columnage
uint8_t cols();

// clean the cells (set 0)
void clear();

// At least one of the cells has been changed. Discharge vehicle
bool changed();

// change
bool resize(uint16_t rows);

// reserve
bool reserve(uint16_t rows);

// fill out
bool add();

// insert
bool append(...);

// set the limit of the number of lines for add/append, will scroll when exceeded. 0 - disconnect
void setLimit(uint16_t limit);

// Move the table up and write down the values at the end
void shift(...);

// delete the line. Negative from the end.
bool remove(int row);

// delete
void removeAll();

// Duplicate the last line and add at the end
bool dupLast();

// scroll up the lines by 1 line
void scrollUp();

// scroll down by 1 line
void scrollDown();

// free up
void reset();

// Export size of the table (for writeTo)
size_t writeSize();

// Export the table to size t write(uint8 t*, size t)
template <typename T>
bool writeTo(T& writer);

// Export the table to write(uint8 t, size t)
bool writeTo(T& stream);

// export the table to the writeSize buffer()
bool writeTo(uint8_t* buffer);

// Import a table from Stream (e.g. file)
bool readFrom(Stream& stream, size_t len);

// buffer
bool readFrom(const uint8_t* buffer, size_t len);

// cell
cell_t type(uint16_t row, uint8_t col);
```

### Row
table-line
```cpp
// cell access
Cell operator[](uint8_t col);

// line up
template <typename... Args>
void write(Args... args);
```

### Cell
Cell table
```cpp
// cell
cell_t type();

// print
size_t printTo(Print& p);

// appropriate
template <typename T>
T operator=(T val);

// int32
int32_t toInt();

// float
float toFloat();

// pointer
void* buf();

// pointer
char* str();

// Comparison and change operators
```

The string cells are correctly assigned`const char*`The lines are compared with them through`==`and`!=`.

### TableFile
Inherit.`Table`. Automatic writing to a file when changing by timeout

```cpp
// specify the file system, file path and timeout in MS
TableFile(fs::FS* nfs = nullptr, const char* path = nullptr, uint32_t tout = 10000);

// install the file system and file name
void setFS(fs::FS* nfs, const char* path);

// Set timeout records, MS (silent 10000)
void setTimeout(uint32_t tout = 10000);

// read
bool begin();

// update the file
bool update();

// ticker, call the loop. You will update the data when you change and exit the timeout, return the true
bool tick();
```

### TableFileStatic
Adding data to a table file without reading the table itself into RAM. Allows you to keep large tables, because there is no limit to open in RAM. The data format is the same as the normal table above, i.e. the file`TableFileStatic`can be opened`TableFile`.

```cpp
// specify the file system, file path and max. number of lines (0 - no limit)
TableFileStatic(fs::FS* nfs, const char* path, uint16_t maxRows = 0);

// file
Info getInfo();

// set max. Number of lines (will be shifted at append)
void setMaxRows(uint16_t maxRows);

// delete
bool removeAll();

// print out
void dump(Print& p);

// initialize
bool init(uint8_t cols, ...);

// add a row to the table (move the table if it exceeds the maximum rows)
bool append(Args... args);

{
    gtl::array<cell_t> types;  // coltype
    uint16_t rows = 0;         // line
    uint8_t cols = 0;          // column
}
```

When setting a max limit. If it is exceeded, the table will be shifted and cut under the limit. To summon`append()`You will need free space for a temporary file the size of a set limit - essentially the same size as the current table.

<a id="examples"></a>

## Examples
### Table
```cpp
// 4 lines, 3 columns
Table table(4, 3, cell_t::Int8, cell_t::Uint8, cell_t::Float);

// The first line of [the line] [the column]
table[0][0] = -123;
table[0][1] = 123;
table[0][2] = 123.456;

// entry in the last row in cell 0 (negative rows - from the end of the table)
table[-1][0] = -123;

// Write the entire line at once (the function accepts any number of arguments)
table[1].write(-123, 123, -123.456);

// tabulation
table.dump(Serial);

// celling
Serial.println(table[0][0]);    // printable
int8_t v = table[0][1];         // converter
table[0][2].toFloat();          // manual
(int32_t)table[0][2];           // manual

// cell-change
// any comparison operations and operators
table[0][0] == 3;
table[0][0] > 3;
table[0][0] *= 3;
table[0][0]++;
```

### String cell types
```cpp
// 1 lines, 3 columns
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

### Dynamic table
```cpp
// table 3 columns without cells
Table table;
table.init(3, cell_t::Int8, cell_t::Uint8, cell_t::Float);

// add lines of data
table.append(1, 2, 3);
table.append(4, 5, 6);

table.dump(Serial);
```

### Limitation of lines
An example of a limited log: always write to the last row with a dynamic increase of the table to 5, then rewind the table with each new entry:
```cpp
// originally 0 lines
Table t(0, 2, cell_t::Int8, cell_t::Float);

// lineage
t.setLimit(5);

for (int i = 0; i < 10; i++) t.append(i, i / 10.0);

t.dump(Serial);
```

Adding a new line as a copy of the last line and changing it
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

    // diagram
    t.begin();

    // initialization of the number of columns and types, if the table does not already exist
    t.init(3, cell_t::Uint32, cell_t::Int16, cell_t::Float);

    // string
    t.append(1, 2, 3.14);
}

void loop() {
    // call the ticker in the magnifier, the table itself will be written into the file when changes are made
    t.tick();
}
```

### TableFileStatic
An example of adding lines to a file without reading a table
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

    TableFileStatic table(&LittleFS, "/table2.tbl", 5);  // Max. 5 lines rewinded

    // info
    auto inf = table.getInfo();
    Serial.println(inf.cols);
    Serial.println(inf.rows);

    // initialize the number of columns and types if the file does not already exist
    table.init(3, cell_t::Uint32, cell_t::Int16, cell_t::Float);

    // add data directly to the file
    table.append(inf.rows, random(10), random(10) / 10.0);

    // Thus, you can log directly in the file, not limited to the amount of RAM

    // dump
    table.dump(Serial);
}

void loop() {
}
```

<a id="versions"></a>

## Versions
- v1.0
- v1.1.0

<a id="install"></a>
## Installation
- The library can be found by the name **Table** and installed through the library manager in:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Download the library](https://github.com/GyverLibs/Table/archive/refs/heads/main.zip).zip archive for manual installation:
    - Unpack and put in *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Unpack and put in *C:\Program Files\Arduino\libraries* (Windows x32)
    - Unpack and put in *Documents/Arduino/libraries/ *
    - (Arduino IDE) Automatic installation from .zip: *Sketch/Connect library/Add .ZIP library...* and specify downloaded archive
- Read more detailed instructions for installing libraries[here](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Update
- I recommend always updating the library: new versions fix errors and bugs, as well as optimize and add new features.
- Through the library manager IDE: find the library as when installing and click "Update"
- Manually: **Delete the folder with the old version** and then put the new one in its place. “Replacement” can not be done: sometimes new versions delete files that will remain when replaced and can lead to errors!

<a id="feedback"></a>

## Bugs and feedback
If you find bugs, create **Issue**, or better write to the mail immediately.[alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
The library is open for revision and your **Pull Requests*!

When reporting bugs or incorrect work of the library, it is necessary to specify:
- Library version
- What is used by the IC
- SDK version (for ESP)
- Arduino IDE version
- Are embedded examples that use features and designs that cause bugs in your code working correctly?
- What code was downloaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed. Not a canvas of a thousand lines, but a minimum code.
