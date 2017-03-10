# ----------------------------------------------------
# This file is the template of app pro file
# ------------------------------------------------------

rootdir=../..                    #解决方案根目录
PROJECT=demo_fpga                #xxx=子工程名

TEMPLATE = app                   #工程类型 lib：链接库、app：应用程序

CONFIG += debug_and_release      #配置参数 debug_and_release：编译选项支持

#包含头文件路径
INCLUDEPATH += $${GeneratedFileDir} \
               $${rootdir} \
               .

DESTDIR = $${rootdir}/bin        #生成目标文件的路径
LIBS += -L$${rootdir}/lib \
        -L$${rootdir}/bin        #外部链接库的路径


QT -= core gui                   #需要使用到的Qt模块库  core、gui、xml、network等等

GeneratedFileDir=$${rootdir}/build/intermediate/$${PROJECT}
DEPENDPATH += .
UI_DIR  += $${GeneratedFileDir}  #自动生成的ui文件存放路径
RCC_DIR += $${GeneratedFileDir}  #自动生成的rc文件存放路径


CONFIG(debug, debug|release) {
    TARGET = $${PROJECT}_d                      #最终生成的目标文件名
    MOC_DIR += $${GeneratedFileDir}/debug       #自动生成的moc文件存放路径
    INCLUDEPATH += $${GeneratedFileDir}/Debug
    OBJECTS_DIR += $${GeneratedFileDir}/Debug   #编译器生成的obj文
    LIBS += -lfpga_d                         #需要调用到的外部链接库
} else {
    DEFINES += QT_NO_DEBUGs
    TARGET = $${PROJECT}
    INCLUDEPATH += $${GeneratedFileDir}/Release
    MOC_DIR += $${GeneratedFileDir}/Release
    OBJECTS_DIR += $${GeneratedFileDir}/Release
    LIBS += -lfpga
}

include($${PROJECT}.pri)

