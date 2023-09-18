QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    opencv_utils.cpp \
    paintscene.cpp

HEADERS += \
    mainwindow.h \
    opencv_utils.h \
    paintscene.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    HoleProcessor_ru_RU.ts
CONFIG += lrelease
CONFIG += embed_translations


INCLUDEPATH += C:\Programs2\opencv\opencv\build\include

LIBS += C:\Programs2\opencv\build\bin\libopencv_core480.dll
LIBS += C:\Programs2\opencv\build\bin\libopencv_highgui480.dll
LIBS += C:\Programs2\opencv\build\bin\libopencv_imgcodecs480.dll
LIBS += C:\Programs2\opencv\build\bin\libopencv_imgproc480.dll
LIBS += C:\Programs2\opencv\build\bin\libopencv_features2d480.dll
LIBS += C:\Programs2\opencv\build\bin\libopencv_calib3d480.dll


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
