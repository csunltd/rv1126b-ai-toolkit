##
## 説明：
## 本サブモジュール（common.pri）は、ユーザーがサードパーティモジュール（ライブラリまたはソースコード）を管理するために使用します
##

INCLUDEPATH += \
    $$PWD/ \
    $$PWD/player/ \

SOURCES += \
    $$PWD/player/player.cpp \

HEADERS += \
    $$PWD/system.h \
    $$PWD/player/player.h \
