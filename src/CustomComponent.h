//! @file CustomComponent.h
//! @author Сивенков С.
//! @note Ответственный: Полевой Д.В.
//! @brief Заголовочный файл для класса реализующего функционал приложения

#pragma once

#include <imgui.h>

#include <string>
#include <sqlite3.h>
#include <filesystem>
#include <vector>
#include <fstream>
#include <algorithm>
#include <queue>
#include <iostream>
#include <sstream>

/*
 * CREATE TABLE table1 (
 *	id_ INTEGER PRIMARY KEY AUTOINCREMENT,
 *	model_ TEXT,
 *	author_ TEXT,
 *	text_ TEXT,
 *	user_ TEXT,
 *	textCharacter_ INTEGER
 * );
 * INSERT INTO table1(model_, author_, text_, user_, textCharacter_) VALUES ('model1', 'author1', 'text1', 1);
 */

namespace fs = std::filesystem;
using BYTE = unsigned char;

class CustomComponent
{
public:
    //! @brief Конструктор класса
    CustomComponent() = default;
    //! @brief Деструктор класса
    ~CustomComponent() = default;
    //! @brief Функция, которая запускает отрисовку интерфейса
    void display(ImVec2 componentPos, ImVec2 componentSize);

public:
    //! @brief Функция, которая отрисовывает интерфейс авторизации
    void loginWindow();
    //! @brief Функция, которая отрисовывает интерфейс главного окна
    void mainWindow();
    //! @brief Функция, которая парсит данные из таблицы
    void getInfoFromCSV();
    //! @brief Функция, которая записывает данные в базу данных
    //! @param[in] textCharacter - параметр, который определяет соответствие текста репертуару автора
    void writeToDatabase(bool textCharacter);
    std::vector<BYTE> base64_decode(std::string const& encoded_string);
    static inline bool is_base64(BYTE c) 
    {
        return (isalnum(c) || (c == '+') || (c == '/'));
    }
private:
    bool loginFlag_ = false; //!< Флаг, который определяет, интерфейс какого окна должен быть отрисован
    ImVec2 componentPos_ = {}; //!< Позиция ImGui виджета внутри окна
    ImVec2 componentSize_ = {}; //!< Размер окна
    std::string loginWindowError_ = {}; //!< Текст ошибки при авторизации
    std::string mainWindowError_ = {}; //!< Текст ошибки в главном окне

    char dbPathBuf_[2048] = {}; //!< Буфер для пути к базе данных
    char csvPathBuf_[2048] = {}; //!< Буфер для пути к таблице
    char loginBuf_[100] = {}; //!< Буфер для логина пользователя
    std::string dbPath_ = {}; //!< Путь к базе данных
    std::string csvPath_ = {}; //!< Путь к таблице
    std::string login_ = {}; //!< Логин пользователя

    std::string dbError_  = {}; //!< Текст ошибки при работе с базой данных

    bool infoWasReceivedFlag_ = false; //!< Флаг, который отвечает за получение данных из таблици
    std::queue<std::vector<std::string> > inputInfo_ = {}; //!< Очередь входных табличных данных
    const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
};
