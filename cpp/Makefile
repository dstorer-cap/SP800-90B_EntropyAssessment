#===========================================================
# Layout
#===========================================================
DEP_PATH := ../..
SRC_PATH := ../../../src
TMP_PATH := ../../../tmp
OUT_PATH := ../../../out

DEP_DTS_PATH := ${DEP_PATH}/dts
DEP_GST_PATH := ${DEP_PATH}/guest
DEP_LVM_PATH := ${DEP_PATH}/libvmm
DEP_MKT_PATH := ${DEP_PATH}/microkit
DEP_HLT_PATH := ${DEP_PATH}/ent_random_sequence_tester
DEP_PLC_PATH := ${DEP_PATH}/picolibc

CPU := cortex-a53
TCH := aarch64-linux-gnu

MKT_BOARD := maaxboard
MKT_CONFIG := debug

MKT_SDK_PATH := ${DEP_MKT_PATH}/out/microkit-sdk-1.4.1
MKT_PATH_FILE := ${MKT_SDK_PATH}/bin/microkit
MKT_RTM_PATH_FILE := ${MKT_SDK_PATH}/board/${MKT_BOARD}/${MKT_CONFIG}
PICOLIBC_PATH = ${DEP_PLC_PATH}/picolibc_build/picolibc/aarch64-linux-gnu


ARCH ?= x86

override CXX = $(CROSS_COMPILE)g++

$(info CXX is set to: $(CXX))

CXXFLAGS = -std=c++11 -fopenmp -O2 -ffloat-store -I/usr/include/jsoncpp
ifeq ($(ARCH),x86)
CXXFLAGS += -msse2 -march=native
endif

#CXX = clang++-15
#CXXFLAGS = -g -Wno-padded -Wno-disabled-macro-expansion -Wno-gnu-statement-expression -Wno-bad-function-cast -fopenmp -O1 -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer -fdenormal-fp-math=ieee -msse2 -march=native -I/usr/include/jsoncpp
#static analysis in clang using
#scan-build-15 --use-c++=/usr/bin/clang++-15 make
LIB = -lbz2 -lpthread -ldivsufsort -ldivsufsort64
COND_LIB = -lmpfr -lgmp
SHARED_LIB = -ljsoncpp -lcrypto
INC =

######
# Main operations
######

all: ${TMP_PATH}/health_check_long.elf

.PHONY: all clean ${TMP_PATH}

${TMP_PATH}:
	mkdir -p ${TMP_PATH}

clean:
	rm -f *.o ea_* ${TMP_PATH}/*.elf selftest/*.res

# Use non_iid target

# IID Target
# iid: iid_main.o
# 	$(CXX) $(CXXFLAGS) $(INC) iid_main.o -o ea_iid $(LIB) $(SHARED_LIB)

# iid_main.o: iid_main.cpp
# 	$(CXX) $(CXXFLAGS) $(INC) -c iid_main.cpp -o iid_main.o

# ELF Target
${TMP_PATH}/health_check_long.elf: non_iid_main.o | ${TMP_PATH}
	$(LD) \
	-L $(MKT_RTM_PATH_FILE)/lib \
	$^ \
	-lmicrokit \
	-Tmicrokit.ld \
	/usr/lib/gcc-cross/aarch64-linux-gnu/12/libgcc.a \
	-L${PICOLIBC_PATH}/lib \
	-lc -lm /usr/lib/gcc-cross/aarch64-linux-gnu/12/libgcc.a \
	-o $@

non_iid: non_iid_main.o	
non_iid_main.o: non_iid_main.cpp
	$(CXX) $(CXXFLAGS) $(INC) non_iid_main.cpp -o ea_non_iid $(LIB) $(SHARED_LIB)

# restart: restart_main.o
# restart_main.o: restart_main.cpp
# 	$(CXX) $(CXXFLAGS) $(INC) restart_main.cpp -o ea_restart $(LIB) $(SHARED_LIB)

# conditioning: conditioning_main.o
# conditioning_main.o: conditioning_main.cpp
# 	$(CXX) $(CXXFLAGS) $(INC) conditioning_main.cpp -o ea_conditioning $(LIB) $(COND_LIB) $(SHARED_LIB)

# transpose: transpose_main.o
# transpose_main.o: transpose_main.cpp
# 	$(CXX) $(CXXFLAGS) $(INC) transpose_main.cpp -o ea_transpose $(LIB) $(SHARED_LIB)
