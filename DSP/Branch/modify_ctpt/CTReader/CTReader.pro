OTHER_FILES += \
    ../ctpt/Makefile \
    ../ctpt/doconfig.sh

HEADERS += \
    ../ctpt/wave-signal.h \
    ../ctpt/smc_api.h \
    ../ctpt/scan_strategy.hpp \
    ../ctpt/raw_wave.hpp \
    ../ctpt/functions.hpp \
    ../ctpt/fpga.hpp \
    ../ctpt/field_excitation_calculator.hpp \
    ../ctpt/exciting_scanner.hpp \
    ../ctpt/direct_scanner.hpp \
    ../ctpt/data_source_fpga.hpp \
    ../ctpt/data_source.hpp \
    ../ctpt/ctpt_api.hpp \
    ../ctpt/calculator.hpp

SOURCES += \
    ../ctpt/wave-signal.c \
    ../ctpt/wave_gen.cxx \
    ../ctpt/smc_api.c \
    ../ctpt/scan_strategy.cxx \
    ../ctpt/raw_wave.cxx \
    ../ctpt/functions.cxx \
    ../ctpt/fpga.cxx \
    ../ctpt/field_excitation_calculator.cxx \
    ../ctpt/exciting_scanner.cxx \
    ../ctpt/direct_scanner.cxx \
    ../ctpt/data_source_fpga.cxx \
    ../ctpt/data_source.cxx \
    ../ctpt/ctpt_api.cxx \
    ../ctpt/calculator.cxx \
    ../ctpt/test/demo_fpga.cxx
