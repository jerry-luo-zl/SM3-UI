QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/SM3.cpp \
    include/openssl/applink.c \
    src/main.cpp \
    src/sm3_mainwindow.cpp

HEADERS += \
    include/errmsg.h \
    include/field_types.h \
    include/my_command.h \
    include/my_compress.h \
    include/my_list.h \
    include/mysql.h \
    include/mysql/client_plugin.h \
    include/mysql/plugin_auth_common.h \
    include/mysql/udf_registration_types.h \
    include/mysql_com.h \
    include/mysql_time.h \
    include/mysql_version.h \
    include/mysqld_error.h \
    include/mysqlx_ername.h \
    include/mysqlx_error.h \
    include/mysqlx_version.h \
    include\SM3.h \
    include\Types.h \
    include\sm3_mainwindow.h

FORMS += \
    sm3.ui

INCLUDEPATH += include/

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
