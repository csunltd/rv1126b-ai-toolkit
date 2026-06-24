##
## 説明：
## 本サブモジュール（api.pri）は、ユーザーが easyeai_api に対して拡張ラッパーまたはカスタム補助ラッパーを追加するために使用します
##

## Qt プロジェクトで easyapi_api ディレクトリおよびライブラリを参照します
## ========================== easyeai_api ========================== ##
MEDIA_DIR = $$PWD/../../../../easyeai-api/media
COMMONAPI_DIR = $$PWD/../../../../easyeai-api/common
ALGORITHM_DIR = $$PWD/../../../../easyeai-api/algorithm

INCLUDEPATH += \
    $${COMMONAPI_DIR}/system_opt/ \
    $${MEDIA_DIR}/display/ \

HEADERS += \
    $${COMMONAPI_DIR}/system_opt/*.h \
    $${MEDIA_DIR}/display/*.h \

SOURCES += \
    $${COMMONAPI_DIR}/system_opt/*.cpp \

## 一部の API はシステムライブラリ（例：-lpthread）に依存するため、LIBS 変数で追加してください：
LIBS += -L$${MEDIA_DIR}/display/ -ldisplay -lrga -ldrm
LIBS += -lpthread
## ================================================================= ##

