# Определяет тип шаблона проекта. 'app' для исполняемого приложения.
TEMPLATE = app

# Указывает, что Qt будет использоваться в проекте.
# Включает основные модули:
# core: Базовые не-GUI классы Qt.
# gui: Базовые GUI классы Qt.
# widgets: Модуль для QWidget-основанных графических элементов UI (QMainWindow, QApplication, QLineEdit, QLabel, QPushButton, QGraphicsView, QDoubleValidator, QIntValidator). [cite: 46, 50, 52, 64, 65]
# multimedia: Модуль для работы с аудио и видео (QMediaPlayer). [cite: 60]
QT       += core gui widgets multimedia

# Эта строка рекомендуется для проектов Qt5, чтобы использовать виджеты.
# Если у вас Qt6, она также не повредит.
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Конфигурация проекта.
# c++17: Включает поддержку стандарта C++17.
# debug_and_release: Позволяет создавать как отладочную, так и релизную версии.
# build_all: Собирать все конфигурации.
CONFIG += c++17 debug_and_release build_all

# Определяет, где искать заголовочные файлы. $$PWD - текущая директория проекта.
INCLUDEPATH += $$PWD

# Определяет исходные файлы (.cpp), которые будут скомпилированы.
SOURCES += \
    main.cpp \
    mainwindow.cpp
    # Добавьте сюда любые другие ваши .cpp файлы, если они есть.

# Определяет заголовочные файлы (.h), которые будут использоваться.
HEADERS += \
    mainwindow.h
    # Добавьте сюда любые другие ваши .h файлы, если они есть.

# Определяет файлы пользовательского интерфейса (.ui), созданные в Qt Designer.
FORMS += \
    mainwindow.ui
    # Добавьте сюда любые другие ваши .ui файлы, если они есть.

# Если вы используете QRC-файл для MP3 (например, media.qrc), добавьте его здесь:
# RESOURCES += \
#    media.qrc
