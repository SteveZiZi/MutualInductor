
SOURCES += ../../src/ctpt/ctpt_api.cxx \
    $$PWD/../../src/ctpt/ctpt_ctrl_p.cpp \
    $$PWD/../../src/ctpt/status_mgr.cpp \
    $$PWD/../../src/ctpt/lowess.cpp \
    $$PWD/../../src/ctpt/splinefit.cpp


HEADERS += ../../src/fpga/fpga.hpp \
           ../../src/fpga/raw_wave.hpp \
           ../../src/ctpt/ctpt_api.hpp \
           ../../src/ctpt/data_type.hpp \
    $$PWD/../../src/ctpt/ctpt_ctrl_p.h \
    $$PWD/../../src/ctpt/ctpt_def.h \
    $$PWD/../../src/ctpt/status_mgr.h \
    $$PWD/../../src/inc/tmutex.h \
    $$PWD/../../src/inc/auto_guard.h \
    $$PWD/../../src/ctpt/lowess.h \
    $$PWD/../../src/ctpt/splinefit.h
