# source.pro - qmake for povray/source
# builds: libpovray.a - The main povray library
# Copyright 2018 BuckoSoft
#

QT       -= gui

TARGET = platform
TEMPLATE = lib
CONFIG += staticlib warn_off
CONFIG += object_parallel_to_source

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#DEFINES += OPENEXR_MISSING
#DEFINES += BUILD_ARCH="\"$$QMAKE_HOST.arch\""
QMAKE_CLEAN += libplatform.a

pDIR = ../../platform/x86

INCLUDEPATH += "../../source"
INCLUDEPATH += "../../platform/unix"
INCLUDEPATH += "../../unix/povconfig"
INCLUDEPATH += $$pDIR

# QMAKE_CXXFLAGS_WARN_OFF -= -Wunused-parameter

SOURCES += \
	$$pDIR/cpuid.cpp \
	$$pDIR/optimizednoise.cpp \


HEADERS += \
	$$pDIR/cpuid.h \
	$$pDIR/optimizednoise.h \
	$$pDIR/avx/avxnoise.h \
	$$pDIR/avx/avxportablenoise.h \
	$$pDIR/avx2fma3/avx2fma3noise.h \
	$$pDIR/avxfma4/avxfma4noise.h \

SOURCES_AVX = $$pDIR/avx/avxnoise.cpp $$pDIR/avx/avxportablenoise.cpp
avx.name = avx
avx.input = SOURCES_AVX
avx.dependency_type = TYPE_C
avx.variable_out = OBJECTS
avx.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_IN_BASE}$${first(QMAKE_EXT_OBJ)}
avx.commands = $${QMAKE_CXX} $(CXXFLAGS) -mavx $(INCPATH) -c ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
QMAKE_EXTRA_COMPILERS += avx

SOURCES_AVX2FMA3 = $$pDIR/avx2fma3/avx2fma3noise.cpp
avx2fma3.name = avx2fma3
avx2fma3.input = SOURCES_AVX2FMA3
avx2fma3.dependency_type = TYPE_C
avx2fma3.variable_out = OBJECTS
avx2fma3.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_IN_BASE}$${first(QMAKE_EXT_OBJ)}
avx2fma3.commands = $${QMAKE_CXX} $(CXXFLAGS) -mavx2 -mfma $(INCPATH) -c ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
QMAKE_EXTRA_COMPILERS += avx2fma3

SOURCES_AVXFMA4 = $$pDIR/avxfma4/avxfma4noise.cpp
avxfma4.name = avxfma4
avxfma4.input = SOURCES_AVXFMA4
avxfma4.dependency_type = TYPE_C
avxfma4.variable_out = OBJECTS
avxfma4.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_IN_BASE}$${first(QMAKE_EXT_OBJ)}
avxfma4.commands = $${QMAKE_CXX} $(CXXFLAGS) -mavx -mfma4 $(INCPATH) -c ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
QMAKE_EXTRA_COMPILERS += avxfma4

linux-g++ {
	DEFINES += TRY_OPTIMIZED_NOISE
	DEFINES += TRY_OPTIMIZED_NOISE_AVX_PORTABLE
	DEFINES += TRY_OPTIMIZED_NOISE_AVX
	DEFINES += TRY_OPTIMIZED_NOISE_AVX2FMA3
	DEFINES += TRY_OPTIMIZED_NOISE_AVXFMA4
}

CONFIG(debug, debug|release) {
	message(Build platform debug version)
	DEFINES += _DEBUG
}

#unix {
#    target.path = /usr/lib
#    INSTALLS += target
#}

