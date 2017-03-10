
SOURCES += ../../src/fpga/fpga.cxx \
           ../../src/fpga/raw_wave.cxx \
           ../../src/fpga/smc_api.c \ 
           ../../src/fpga/fpga_reg.cpp \
           ../../src/fpga/ac_source.cpp \
           ../../src/fpga/adc_ctrl.cpp \
    $$PWD/../../src/fpga/adcCalibration.cpp \
    $$PWD/../../src/fpga/dc_source.cpp


HEADERS += ../../src/fpga/fpga.hpp \
           ../../src/fpga/raw_wave.hpp \
           ../../src/fpga/smc_api.h \
           ../../src/fpga/fftw3.h \
           ../../src/fpga/fpga_reg.h \
           ../../src/fpga/ac_source.h \
           ../../src/fpga/private_def.h \
           ../../src/fpga/adc_ctrl.h \
    $$PWD/../../src/fpga/adcCalibration.h \
    $$PWD/../../src/fpga/common.h \
    $$PWD/../../src/fpga/dc_source.h \
    $$PWD/../../src/inc/tmutex.h \
    $$PWD/../../src/inc/auto_guard.h \
    $$PWD/../../src/inc/highprecisiontimecounter.h
