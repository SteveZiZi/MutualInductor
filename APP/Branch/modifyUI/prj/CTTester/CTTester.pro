#-------------------------------------------------
#
# Project created by QtCreator 2016-07-18T18:16:49
#
#-------------------------------------------------

rootdir=../..
PROJECT=CT

INCLUDEPATH += $${GeneratedFileDir} \
               $${rootdir} \
               $${rootdir}/src/CTTester \
               .

DESTDIR = $${rootdir}/bin
LIBS += -L/home/tarena/xtoee/CTPT/Branch/modify_ctpt/bin\
        -L$${rootdir}/lib

TEMPLATE = app
QT += core gui xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += staticlib debug_and_release

GeneratedFileDir=$${rootdir}/build/intermediate/$${PROJECT}
DEPENDPATH += .
UI_DIR  += $${GeneratedFileDir}
RCC_DIR += $${GeneratedFileDir}

CONFIG(debug, debug|release) {
    TARGET = $${PROJECT}_d
    MOC_DIR += $${GeneratedFileDir}/debug
    INCLUDEPATH += $${GeneratedFileDir}/Debug
    OBJECTS_DIR += $${GeneratedFileDir}/Debug
    LIBS +=  -lvKeyboard_d
} else {
    DEFINES += QT_NO_DEBUG
    TARGET = $${PROJECT}
    INCLUDEPATH += $${GeneratedFileDir}/Release
    MOC_DIR += $${GeneratedFileDir}/Release
    OBJECTS_DIR += $${GeneratedFileDir}/Release
    LIBS +=  -lvKeyboard
}

USE_FPGA=1
unix {
    equals(USE_FPGA,1) {
        CONFIG(debug, debug|release) {
            LIBS += -lctpt_d -lfpga_d
        } else {
            LIBS += -lctpt -lfpga
        }
    }
}

include($${PROJECT}.pri)
