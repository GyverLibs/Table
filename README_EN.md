This is an automatic translation, may be incorrect in some places. See sources and examples!

# Table
Dynamic table for Arduino
- supports all numerical data types in any combination
- dynamic addition of lines, scrolling and other convenient features for legging
- Automatic entry to the file when changing (ESP)

## compatibility
Compatible with all arduino platforms (used arduino functions)

### Dependencies
- GTL
- Streamio

## Content
- [documentation] (#docs)
- [Examples] (# ExamPles)
- [versions] (#varsions)
- [installation] (# Install)
- [bugs and feedback] (#fedback)

<a id="docs"> </a>

## Documentation
### Cell_t
Type of these cells
`` `CPP
Cell_t :: None
Cell_t :: int8
Cell_t :: uint8
Cell_t :: int16
Cell_t :: uint16
Cell_t :: int32
Cell_t :: uint32
Cell_t :: Float
`` `

### Table
`` `CPP
Table ();

// lines, columns, types of these cells
Table (Uint16_t Rows, Uint8_t Cols, ...);

// Get the line of the table.Negative numbers - get from the end
TBL :: row operator [] (int row);

// Get the line of the table.Negative numbers - get from the end
TBL :: row get (int row);

// Get a cell
TBL :: Cell get (int row, uint8_t color);

// Bring the table to Print
VOID DUMP (Print & P);

// Bring like csv
String Tocsv (Char Separator = ';', Uint8_t Dec = 2);

// number of lines
uint16_t rows ();

// The number of columns
uint8_t color ();

// Clean cells (install 0)
Void Clear ();

// at least one of the cells was changed.Autos
Bool Changed ();

// Change the number of lines
Bool Resize (Uint16_T RESS);

// reserve the number of lines
Bool Reserve (Uint16_T RESS);

// Add a line from below
Bool Addrow ();

// Scroll the table up 1 line
VOID Shiftup ();

// Scroll the table down by 1 line
VOID ShiftDown ();

// completely free memory
VOID Reset ();

// export size of the table (for writeto)
Size_t Writesize ();

// export the table to size_t Write (uint8_t*, size_t)
TEMPLATE <TYPENAME T>
Bool Writeto (T & Writer);

// export the table in Stream (e.g. file)
Bool Writeto (Stream & Stream);

// export the table to the size of the size of writesize ()
Bool Writeto (Uint8_t* Buffer);

// import a table from Stream (e.g. file)
Bool Readfrom (Stream & Stream, Size_t Len);

// import a table from the buffer
Bool Readfrom (Consta Uint8_t* Buffer, Size_t Len);

// Cell type
Cell_t Type (Uint16_t Row, Uint8_t Col);
`` `

### TableFile
Inherits Table.Automatic entry into a file when changing the timeout
`` `CPP
// specify the file system, the path to the file and the Timout in MS
TableFile (fs :: fs* nfs = nullptr, constel* path = nullptr, uint32_t tout = 10000);

// install the file system and file name
VOID setfs (fs :: fs* nfs, const char* Path);

// Install the Takeout of the Records, MS (silence 1000)
VOID settimeout (uint32_t tout = 10000);

// Read the data
Bool Begin ();

// update data in the file
Bool update ();

// ticker, call in LOOP.He will update the data himself when the timuta is changed and output, it will return True
Bool Tick ();
`` `

### row
The line of the table
`` `CPP
// Access to the cell
Cell Operator [] (uint8_t color);

// write in a line
TEMPLATE <TYPENAME ... Args>
VOID Write (Args ... Args);
`` `

### Cell
The cell of the table
`` `CPPCranberry
// Cell type
Cell_t Type ();

// Print in Print
Size_t Printto (Print & P);

// assign any type
TEMPLATE <TYPENAME T>
T Operator = (T Val);

// in int32
int32_t toint ();

// in Float
Float Tofloat ();

// as well as comparison and changes operators
`` `

<a id="EXAMPLASX> </A>

## Examples
Basic example
`` `CPP
// 4 lines, 3 columns
Table Table (4, 3, Cell_t :: int8, Cell_t :: uint8, Cell_t :: Float);

// entry in the first line
Table [0] [0] = -123;
Table [0] [1] = 123;
Table [0] [2] = 123.456;

// recording in the last line
Table [-1] [0] = -123;

// recording the entire line immediately (the function accepts any number of arguments)
Table [1] .Write (-123, 123, -123.456);

// The output of the table
Table.Dump (Serial);

// The conclusion of the cells
Serial.println (Table [0] [0]);// Printed
int8_t v = guble [0] [1];// Auto Convertation
Table [0] [2] .tofloat ();// manual conversion
(int32_t) Table [0] [2];// manual conversion

// Changing cells
// Any comparison operations and operators
Table [0] [0] == 3;
Table [0] [0]> 3;
Table [0] [0] *= 3;
Table [0] [0] ++;
`` `

An example of a log: a record is always in the last line with a dynamic increase in the table (up to 5), then rewind the table with each new entry:
`` `CPP
// initially 0 lines
Table T (0, 2, Celltype :: int8);

// can be reserved to your maximum
// T.ESERVE (5);

for (int i = 0; i <10; i ++) {
if (t.rows () <5) t.addrow ();// Adding a line
Else t.shiftup ();// rewinding (max. reached)
t [-1] [0] = i;// recording in the last line
}
T.Dump (Serial);
`` `

An example with TableFile
`` `CPP
#include <Littlefs.h>
#include <tablefile.h>
Tablefile TB (& Littlefs, "Table.tbl");

VOID setup () {
Littlefs.Begin ();
tb.begin ();// read data from the file

tb [0] [0] = 123;
}

VOID loop () {
// call a ticker in a lup
tb.tick ();
}
`` `

<a id="versions"> </a>

## versions
- V1.0

<a id="install"> </a>
## Installation
- The library can be found by the name ** Table ** and installed through the library manager in:
- Arduino ide
- Arduino ide v2
- Platformio
- [download the library] (https://github.com/gyverlibs/table/archive/refs/heads/main.zip) .Zip archive for manual installation:
- unpack and put in * C: \ Program Files (X86) \ Arduino \ Libraries * (Windows X64)
- unpack and put in * C: \ Program Files \ Arduino \ Libraries * (Windows X32)
- unpack and put in *documents/arduino/libraries/ *
- (Arduino id) Automatic installation from. Zip: * sketch/connect the library/add .Zip library ... * and specify downloaded archive
- Read more detailed instructions for installing libraries [here] (https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%BD%D0%BE%BE%BE%BED0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Update
- I recommend always updating the library: errors and bugs are corrected in the new versions, as well as optimization and new features are added
- through the IDE library manager: find the library how to install and click "update"
- Manually: ** remove the folder with the old version **, and then put a new one in its place.“Replacement” cannot be done: sometimes in new versions, files that remain when replacing are deleted and can lead to errors!

<a id="feedback"> </a>

## bugs and feedback
Create ** Issue ** when you find the bugs, and better immediately write to the mail [alex@alexgyver.ru] (mailto: alex@alexgyver.ru)
The library is open for refinement and your ** pull Request ** 'ow!

When reporting about bugs or incorrect work of the library, it is necessary to indicate:
- The version of the library
- What is MK used
- SDK version (for ESP)
- version of Arduino ide
- whether the built -in examples work correctly, in which the functions and designs are used, leading to a bug in your code
- what code has been loaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed.Not a canvas of a thousand lines, but a minimum code