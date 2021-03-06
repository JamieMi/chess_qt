# Add more folders to ship with the application, here
folder_01.source = qml/Chess
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
   PromotionDlg.cpp
QT += widgets # to use QMessageBox
QT += declarative
QT += core gui declarative # necessary?

# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

CONFIG+=debugS

HEADERS += \
    chess.h \
    PromotionDlg.h

QMAKE_CXXFLAGS += -std=c++11
