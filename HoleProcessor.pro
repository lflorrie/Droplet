QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/opencv_utils.cpp \
    src/paintscene.cpp

INCLUDEPATH += include/

HEADERS += \
    mainwindow.h \
    opencv_utils.h \
    paintscene.h

FORMS += \
    forms/mainwindow.ui

TRANSLATIONS += \
    translate/HoleProcessor_ru_RU.ts

CONFIG += lrelease
CONFIG += embed_translations


#example
#unix: include(/home/user/myProject/myLybrary/my-lib.pri)
#win32: include(C:/myProject/myLybrary/my-lib.pri)

unix {
    INCLUDEPATH += /usr/include/opencv4
    LIBS += -lopencv_core
    LIBS += -lopencv_highgui
    LIBS += -lopencv_imgcodecs
    LIBS += -lopencv_imgproc
    LIBS += -lopencv_features2d
    LIBS += -lopencv_calib3d
}
win32 {
    INCLUDEPATH += C:\Programs2\opencv\opencv\build\include

    LIBS += C:\Programs2\opencv\build\bin\libopencv_core480.dll
    LIBS += C:\Programs2\opencv\build\bin\libopencv_highgui480.dll
    LIBS += C:\Programs2\opencv\build\bin\libopencv_imgcodecs480.dll
    LIBS += C:\Programs2\opencv\build\bin\libopencv_imgproc480.dll
    LIBS += C:\Programs2\opencv\build\bin\libopencv_features2d480.dll
    LIBS += C:\Programs2\opencv\build\bin\libopencv_calib3d480.dll
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
