#pragma once
#include <Arduino.h>
#include <FS.h>

#include "Table.h"

class TableFileStatic {
   public:
    // указать файловую систему, путь к файлу и таймаут в мс
    TableFileStatic(fs::FS* nfs, const char* path) : _fs(nfs), _path(path) {}

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

    // добавить строку к таблице
    template <typename... Args>
    bool append(Args... args) {
        if (!_fs || !_fs->exists(_path)) return false;

        // read
        File file = _fs->open(_path, "r+");
        if (!file) return false;

        uint8_t cols;
        if (!_fread(file, &cols, 1) || !cols) return false;

        uint16_t rows;
        if (!_fread(file, &rows, 2)) return false;

        uint8_t types[cols];
        if (!_fread(file, types, cols)) return false;

        if (!file.seek(1)) return false;

        rows++;
        if (!_fwrite(file, &rows, 2)) return false;

        file.close();

        // append
        file = _fs->open(_path, "a");
        if (!file) return false;

        _idx = 0;
        _types = types;
        _cols = cols;
        _f = &file;
        (_write(args), ...);
        while (_idx < cols) _write(0);
        return true;
    }

   private:
    fs::FS* _fs;
    const char* _path;
    uint8_t* _types = nullptr;
    File* _f = nullptr;
    uint8_t _idx = 0;
    uint8_t _cols = 0;

    bool _fread(File& file, void* data, int len) {
        return file.read((uint8_t*)data, len) == len;
    }
    bool _fwrite(File& file, void* data, size_t len) {
        return file.write((uint8_t*)data, len) == len;
    }

    template <typename T>
    void _write(T arg) {
        if (_idx >= _cols) return;

        switch ((cell_t)_types[_idx]) {
            default:
            case cell_t::Int8:
            case cell_t::Uint8: {
                uint8_t v = arg;
                _fwrite(*_f, &v, 1);
            } break;
            case cell_t::Int16:
            case cell_t::Uint16: {
                uint16_t v = arg;
                _fwrite(*_f, &v, 2);
            } break;
            case cell_t::Int32:
            case cell_t::Uint32: {
                uint32_t v = arg;
                _fwrite(*_f, &v, 4);
            } break;
            case cell_t::Uint64: {
                uint64_t v = arg;
                _fwrite(*_f, &v, 8);
            } break;
            case cell_t::Float: {
                float v = arg;
                _fwrite(*_f, &v, 4);
            } break;
        }
        _idx++;
    }
};