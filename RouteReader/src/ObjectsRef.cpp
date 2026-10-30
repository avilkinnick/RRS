#include "RouteReader/ObjectsRef.h"

#include <Journal.h>

#include <QString>

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <string>

enum State
{
    STATE_BEFORE_LABEL = 0,
    STATE_READING_LABEL,
    STATE_BEFORE_REL_PATH,
    STATE_READING_REL_PATH,
    STATE_AFTER_REL_PATH
};

bool ObjectsRef::read(const char* path)
{
    // Открываем файл
    std::FILE* const file = std::fopen(path, "rb");
    if (file == nullptr)
    {
        Journal::instance()->error(QString("Failed to open %1")
                                       .arg(path));
        return false;
    }

    // Считаем размер файла
    std::fseek(file, 0, SEEK_END);
    const std::size_t file_size = (std::size_t)std::ftell(file);
    std::rewind(file);

    // Выделяем буфер под содержимое файла
    // (+ 1 байт под null terminator)
    char* const content = (char*)std::malloc(file_size + 1);
    if (content == nullptr)
    {
        Journal::instance()->error(QString("Failed to read %1 in buffer")
                                       .arg(path));
        std::fclose(file);
        return false;
    }

    // Копируем всё содержимое файла в буфер
    std::fread(content, file_size, 1, file);
    content[file_size] = '\0';

    // Закрываем файл после успешного копирования
    std::fclose(file);

    // Выделяем буферы фиксированного размера под label и относительный путь
    char label_buf[256];
    std::size_t label_len = 0;

    char rel_path_buf[512];
    std::size_t rel_path_len = 0;

    // Устанавливаем начальное состояние и номер строки
    // (номер строки будем использовать для отладочных сообщений)
    State state = STATE_BEFORE_LABEL;
    std::size_t line_number = 1;

    // Посимвольно проходимся по всему содержимому objects.ref
    const char* ptr = content;
    while (const char ch = *ptr)
    {
        switch (state)
        {
            case STATE_BEFORE_LABEL:
            {
                switch (ch)
                {
                    case '\t':
                    case '\r':
                    case ' ':
                    {
                        break;
                    }
                    case '\n':
                    {
                        ++line_number;
                        break;
                    }
                    default:
                    {
                        label_buf[label_len] = ch;
                        ++label_len;
                        state = STATE_READING_LABEL;
                        break;
                    }
                }

                break;
            }
            case STATE_READING_LABEL:
            {
                switch (ch)
                {
                    case '\t':
                    case '\r':
                    case ' ':
                    {
                        state = STATE_BEFORE_REL_PATH;
                        break;
                    }
                    case '\n':
                    {
                        Journal::instance()->warning(
                            QString("\\n after label. "
                                    "Ignoring this line (%1:%2)")
                                .arg(path)
                                .arg(line_number));
                        label_len = 0;
                        state = STATE_BEFORE_LABEL;
                        ++line_number;
                        break;
                    }
                    // case '\r':
                    // {
                    //     Journal::instance()->error(
                    //         QString("\\r after label (%1:%2)")
                    //             .arg(path)
                    //             .arg(line_number));
                    //     std::free(content);
                    //     return false;
                    // }
                    default:
                    {
                        label_buf[label_len] = ch;
                        ++label_len;
                        break;
                    }
                }

                break;
            }
            case STATE_BEFORE_REL_PATH:
            {
                switch (ch)
                {
                    case '\t':
                    case '\r':
                    case ' ':
                    {
                        break;
                    }
                    case '\n':
                    {
                        Journal::instance()->warning(
                            QString("\\n after label. "
                                    "Ignoring this line (%1:%2)")
                                .arg(path)
                                .arg(line_number));
                        label_len = 0;
                        state = STATE_BEFORE_LABEL;
                        ++line_number;
                        break;
                    }
                    // case '\r':
                    // {
                    //     Journal::instance()->error(
                    //         QString("\\r after label (%1:%2)")
                    //             .arg(path)
                    //             .arg(line_number));
                    //     std::free(content);
                    //     return false;
                    // }
                    default:
                    {
                        rel_path_buf[rel_path_len] = ch;
                        ++rel_path_len;
                        state = STATE_READING_REL_PATH;
                        break;
                    }
                }

                break;
            }
            case STATE_READING_REL_PATH:
            {
                switch (ch)
                {
                    case '\t':
                    case '\r':
                    case ' ':
                    {
                        state = STATE_AFTER_REL_PATH;
                        break;
                    }
                    case '\n':
                    {
                        this->emplace(std::string(label_buf, label_len),
                                      std::string(rel_path_buf,
                                                  rel_path_len));
                        label_len = 0;
                        rel_path_len = 0;
                        state = STATE_BEFORE_LABEL;
                        ++line_number;
                        break;
                    }
                    default:
                    {
                        rel_path_buf[rel_path_len] = ch;
                        ++rel_path_len;
                        break;
                    }
                }

                break;
            }
            case STATE_AFTER_REL_PATH:
            {
                switch (ch)
                {
                    case '\t':
                    case '\r':
                    case ' ':
                    {
                        break;
                    }
                    case '\n':
                    {
                        this->emplace(std::string(label_buf, label_len),
                                      std::string(rel_path_buf,
                                                  rel_path_len));
                        label_len = 0;
                        rel_path_len = 0;
                        state = STATE_BEFORE_LABEL;
                        ++line_number;
                        break;
                    }
                    default:
                    {
                        Journal::instance()->error(
                            QString("Extra text after relative path (%1:%2)")
                                .arg(path)
                                .arg(line_number));
                        std::free(content);
                        return false;
                    }
                }

                break;
            }
            default:
            {
                Journal::instance()->error("Invalid state in "
                                           "ObjectsRef::read()");
                std::free(content);
                return false;
            }
        }

        ++ptr;
    }

    // Вносим в map последние label и относительный путь,
    // если файл не заканчивается символом новой строки
    if (state == STATE_READING_REL_PATH || state == STATE_AFTER_REL_PATH)
    {
        this->emplace(std::string(label_buf, label_len),
                      std::string(rel_path_buf, rel_path_len));
    }

    // Очищаем память, выделенную под содержимое файла
    std::free(content);

    return true;
}
