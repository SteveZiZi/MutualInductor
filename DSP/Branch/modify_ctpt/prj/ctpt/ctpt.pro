# ----------------------------------------------------
# This file is the template of dynamic lib pro file
# ------------------------------------------------------


rootdir=../..                       #解决方案根目录
PROJECT=ctpt                     #xxx=子工程名

TEMPLATE = lib                   #工程类型 lib：链接库、app：应用程序
#CONFIG += staticlib             #配置参数 staticlib：静态链接库，如果是动态链接库则不需要该参数
CONFIG += debug_and_release      #配置参数 debug_and_release：编译选项支持
r
#包含头文件路径
INCLUDEPATH += $${GeneratedFileDir} \
               $${rootdir} \
               .

DESTDIR = $${rootdir}/bin        #生成目标文件的路径
LIBS += -L$${rootdir}/lib \
        -L$${rootdir}/bin        #外部链接库的路径

QT -= core gui                   #需要使用到的Qt模块库  core、gui、xml、network等等
DEFINES += QT_DLL


GeneratedFileDir=$${rootdir}/build/intermediate/$${PROJECT}
DEPENDPATH += .
UI_DIR  += $${GeneratedFileDir}  #自动生成的ui文件存放路径
RCC_DIR += $${GeneratedFileDir}  #自动生成的rc文件存放路径

CONFIG(debug, debug|release) {
    TARGET = $${PROJECT}_d                      #最终生成的目标文件名
    MOC_DIR += $${GeneratedFileDir}/debug       #自动生成的moc文件存放路径
    INCLUDEPATH += $${GeneratedFileDir}/Debug
    OBJECTS_DIR += $${GeneratedFileDir}/Debug   #编译器生成的obj文件存放路径
    LIBS += -lfpga_d
} else {
    DEFINES += QT_NO_DEBUG
    TARGET = $${PROJECT}
    INCLUDEPATH += $${GeneratedFileDir}/Release
    MOC_DIR += $${GeneratedFileDir}/Release
    OBJECTS_DIR += $${GeneratedFileDir}/Release
    LIBS += -lfpga
}

include($${PROJECT}.pri)                        #包含的源文件

DISTFILES +=
