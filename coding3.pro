QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
TARGET = DormitoryManager
TEMPLATE = app

SOURCES += main.cpp \
           AnnouncementDialog.cpp \
           ButtonDelegate.cpp \
           CheckInDialog.cpp \
           CheckOutDialog.cpp \
           DormitoryManager.cpp \
           PaymentDialog.cpp \
           RepairButtonDelegate.cpp \
           StudentManager.cpp \
           mainwindow.cpp \
           AddStudentDialog.cpp \
           AddDormitoryDialog.cpp

HEADERS  += mainwindow.h \
            AnnouncementDialog.h \
            ButtonDelegate.h \
            CheckInDialog.h \
            CheckOutDialog.h \
            DormitoryManager.h \
            PaymentDialog.h \
            RepairButtonDelegate.h \
            StudentManager.h \
            AddStudentDialog.h \
            AddDormitoryDialog.h

FORMS    += mainwindow.ui \
    AnnouncementDialog.ui \
    PaymentDialog.ui

INCLUDEPATH += "C:/Program Files/MySQL/MySQL Server 8.0/include"

LIBS += -L"C:/Program Files/MySQL/MySQL Server 8.0/lib" -lmysql

QMAKE_CXXFLAGS += -D__MSVCRT_VERSION__=0x0700
QMAKE_LFLAGS += -lws2_32 -lmswsock -ladvapi32 -lcrypt32 -lgdi32 -luser32 -lkernel32

