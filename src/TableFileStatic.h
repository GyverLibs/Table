#pragma once
#include <Arduino.h>
#include <FS.h>

#include "./core/table_t.h"

class TableFileStatic {
   public:
    // указать файловую систему, путь к файлу и макс. кол-во строк (0 - без лимита)
    TableFileStatic(fs::FS* nfs, const char* path, uint16_t maxRows = 0) : _fs(nfs), _path(path), _maxRows(maxRows) {}

    struct Info {
        gtl::array<uint8_t> types;  // массив типов длиной cols
        uint16_t rows = 0;          // строк
        uint8_t cols = 0;           // столбцов

        inline cell_t getType(int n) {
            return (cell_t)types[n];
        }

        explicit operator bool() {
            return cols;
        }
    };

    // получить инфо о таблице
    Info getInfo() {
        if (!_fs || !_fs->exists(_path)) return Info();

        File file = _fs->open(_path, "r");
        if (!file) return Info();

        Info inf;
        if (!_fread(file, &inf.cols, 1) ||
            !inf.cols ||
            !_fread(file, &inf.rows, 2) ||
            !inf.types.resize(inf.cols) ||
            !_fread(file, inf.types.buf(), inf.cols)) return Info();
        return inf;
    }

    // установить макс. количество строк (будут смещаться при append)
    void setMaxRows(uint16_t maxRows) {
        _maxRows = maxRows;
    }

    // удалить все строки
    bool removeAll() {
        Info inf = getInfo();
        if (!inf) return false;

        inf.rows = 0;
        File file = _fs->open(_path, "w");
        return (file &&
                _fwrite(file, &inf.cols, 1) &&
                _fwrite(file, &_maxRows, 2) &&
                _fwrite(file, inf.types.buf(), inf.cols));
    }

    // вывести таблицу в print
    void dump(Print& p) {
        Info inf = getInfo();
        if (!inf) return;

        p.print('\t');
        for (uint16_t col = 0; col < inf.cols; col++) {
            if (col) p.print('\t');
            p.print(tbl::readType(inf.getType(col)));
        }
        p.println();
        if (!inf.rows) return;

        File file = _fs->open(_path, "r");
        if (!file || !file.seek(1 + 2 + inf.cols)) return;

        for (uint16_t row = 0; row < inf.rows; row++) {
            p.print(row);
            p.print('.');
            p.print('\t');
            for (uint8_t col = 0; col < inf.cols; col++) {
                uint8_t size = tbl::typeSize(inf.getType(col));
                uint8_t raw[size];
                _fread(file, raw, size);
                // TABLE_TYPES
                switch (inf.getType(col)) {
                    case cell_t::Float: p.print(*((float*)raw)); break;
                    case cell_t::Int8: p.print(*((int8_t*)raw)); break;
                    case cell_t::Uint8: p.print(*((uint8_t*)raw)); break;
                    case cell_t::Int16: p.print(*((int16_t*)raw)); break;
                    case cell_t::Uint16: p.print(*((uint16_t*)raw)); break;
                    case cell_t::Int32: p.print(*((int32_t*)raw)); break;
                    case cell_t::Uint32: p.print(*((uint32_t*)raw)); break;
                    case cell_t::Unix: p.print(*((uint32_t*)raw)); break;
                    case cell_t::Char: p.print(*((char*)raw)); break;
                    case cell_t::Char8:
                    case cell_t::Char16:
                    case cell_t::Char32:
                    case cell_t::Char64:
                    case cell_t::Char128:
                    case cell_t::Char256:
                        p.print((const char*)raw);
                        break;

#if defined(ESP32) || defined(ESP8266)
                    case cell_t::Int64: p.print(*((int64_t*)raw)); break;
                    case cell_t::Uint64: p.print(*((uint64_t*)raw)); break;
#else
                    case cell_t::Int64: p.print(*((int32_t*)raw)); break;
                    case cell_t::Uint64: p.print(*((uint32_t*)raw)); break;
#endif
                    default: break;
                }
                p.print('\t');
            }
            p.println();
        }
    }

    // инициализировать в файле
    bool init(uint8_t cols, ...) {
        if (!_fs) return false;
        bool create = true;

        if (_fs->exists(_path)) {
            File file = _fs->open(_path, "r");
            if (!file) return false;

            uint8_t rcols;
            if (!_fread(file, &rcols, 1)) return false;

            if (rcols == cols) {
                create = false;
                if (!file.seek(1 + 2)) return false;

                uint8_t rtypes[rcols];
                if (!_fread(file, rtypes, rcols)) return false;

                va_list types;
                va_start(types, cols);
                for (uint8_t col = 0; col < cols; col++) {
                    uint8_t type = va_arg(types, int);
                    if (type != rtypes[col]) {
                        create = true;
                        break;
                    }
                }
                va_end(types);
            }
        }

        if (create) {
            File file = _fs->open(_path, "w");
            if (!file) return false;

            if (!_fwrite(file, &cols, 1)) goto erase;

            uint16_t rows = 0;
            if (!_fwrite(file, &rows, 2)) goto erase;

            bool err = false;
            va_list types;
            va_start(types, cols);
            for (uint8_t col = 0; col < cols; col++) {
                uint8_t type = va_arg(types, int);
                if (!_fwrite(file, &type, 1)) {
                    err = true;
                    break;
                }
            }
            va_end(types);
            if (err) goto erase;
        }

        return true;

    erase:
        File file = _fs->open(_path, "w");
        return false;
    }

#if _TABLE_USE_FOLD
    // добавить строку к таблице (сдвинет таблицу, если превышает макс. строк)
    template <typename... Args>
    bool append(Args... args) {
        Info inf = getInfo();
        if (!inf) return false;

        File file = _fs->open(_path, "a");
        if (!file) return false;

        _col = 0;
        _inf = &inf;
        _file = &file;

        (_write(args), ...);
        while (_col < inf.cols) _write(0);
        file.close();

        inf.rows++;

        if (_maxRows && inf.rows > _maxRows) {
            file = _fs->open(_path, "r");

            uint16_t rowSize = 0;
            for (uint16_t col = 0; col < inf.cols; col++) {
                rowSize += tbl::typeSize(inf.getType(col));
            }

            static const char* suffix = ".tmp";
            char tempName[strlen(_path) + strlen(suffix) + 1];
            strcpy(tempName, _path);
            strcat(tempName, suffix);
            File temp = _fs->open(tempName, "w");

            if (!file ||
                !file.seek(1 + 2 + inf.cols + rowSize * (inf.rows - _maxRows)) ||
                !temp ||
                !_fwrite(temp, &inf.cols, 1) ||
                !_fwrite(temp, &_maxRows, 2) ||
                !_fwrite(temp, inf.types.buf(), inf.cols) ||
                !_fcpy(temp, file, rowSize, _maxRows)) {
                temp.close();
                _fs->remove(tempName);
                return false;
            }
            temp.close();
            file.close();
            _fs->remove(_path);
            _fs->rename(tempName, _path);
        } else {
            file = _fs->open(_path, "r+");
            if (!file ||
                !file.seek(1) ||
                !_fwrite(file, &inf.rows, 2)) return false;
        }
        return true;
    }
#endif

   private:
    fs::FS* _fs;
    const char* _path;
    File* _file;
    Info* _inf;
    uint16_t _maxRows;
    uint8_t _col;

    bool _fread(File& file, void* data, int len) {
        return file.read((uint8_t*)data, len) == len;
    }
    bool _fwrite(File& file, void* data, size_t len) {
        return file.write((uint8_t*)data, len) == len;
    }
    bool _fcpy(File& to, File& from, uint16_t chunkSize, uint16_t amount) {
        uint8_t buf[chunkSize];
        while (amount) {
            if (from.read(buf, chunkSize) != chunkSize) break;
            if (to.write(buf, chunkSize) != chunkSize) break;
            amount--;
        }
        return !amount;
    }

    template <typename T>
    bool _write(T arg) {
        if (_col >= _inf->cols) return false;

        // TABLE_TYPES
        switch (_inf->getType(_col++)) {
            case cell_t::Char:
            case cell_t::Int8:
            case cell_t::Uint8: {
                uint8_t v = arg;
                return _fwrite(*_file, &v, 1);
            }
            case cell_t::Int16:
            case cell_t::Uint16: {
                uint16_t v = arg;
                return _fwrite(*_file, &v, 2);
            }
            case cell_t::Unix:
            case cell_t::Int32:
            case cell_t::Uint32: {
                uint32_t v = arg;
                return _fwrite(*_file, &v, 4);
            }
            case cell_t::Int64:
            case cell_t::Uint64: {
                uint64_t v = arg;
                return _fwrite(*_file, &v, 8);
            }
            case cell_t::Float: {
                float v = arg;
                return _fwrite(*_file, &v, 4);
            }
            default: break;
        }
        return false;
    }
    bool _write(const char* arg) {
        if (_col >= _inf->cols) return false;

        cell_t type = _inf->getType(_col++);
        // TABLE_TYPES
        switch (type) {
            case cell_t::Char8:
            case cell_t::Char16:
            case cell_t::Char32:
            case cell_t::Char64:
            case cell_t::Char128:
            case cell_t::Char256: {
                size_t len = tbl::typeSize(type);
                char buf[len];
                strncpy(buf, arg, len - 1);
                buf[len - 1] = 0;
                return _fwrite(*_file, buf, len);
            }
            default: break;
        }
        return false;
    }
};