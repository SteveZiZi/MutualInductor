srcdir=../../src

HEADERS +=  $$srcdir/vKeyboard/vkeyboard.h \
            $$srcdir/vKeyboard/characterPad.h \
            $$srcdir/vKeyboard/numPad.h \
            $$srcdir/vKeyboard/mPushButton.h

SOURCES +=  $$srcdir/vKeyboard/characterPad.cpp \
            $$srcdir/vKeyboard/numPad.cpp \
            $$srcdir/vKeyboard/vkeyboard.cpp \
            $$srcdir/vKeyboard/mPushButton.cpp

FORMS +=    $$srcdir/vKeyboard/characterPad.ui \
            $$srcdir/vKeyboard/numpad.ui
