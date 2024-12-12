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
CC := ${TCH}-gcc
LD := ${TCH}-ld
AS := ${TCH}-as

MKT_BOARD := maaxboard
MKT_CONFIG := debug

MKT_SDK_PATH := ${DEP_MKT_PATH}/out/microkit-sdk-1.4.1
MKT_PATH_FILE := ${MKT_SDK_PATH}/bin/microkit
MKT_RTM_PATH_FILE := ${MKT_SDK_PATH}/board/${MKT_BOARD}/${MKT_CONFIG}
PICOLIBC_PATH := ${DEP_PLC_PATH}/picolibc_build/picolibc/aarch64-linux-gnu

OPENSSL_PATH := /home/dstorer/svn_apricot/apricot/development/wip/daniel/single-linux-guest-nitrokey/dep/SP800-90B_EntropyAssessment/openssl-1.1.1e
DIVSURFSORT_PATH := /home/dstorer/svn_apricot/apricot/development/wip/daniel/single-linux-guest-nitrokey/dep/SP800-90B_EntropyAssessment/libdivsufsort/build
LIBJSONCPP_PATH := /home/dstorer/svn_apricot/apricot/development/wip/daniel/single-linux-guest-nitrokey/dep/SP800-90B_EntropyAssessment/jsoncpp/build

ARCH := aarch64

override CXX := ${TCH}-g++

CXXFLAGS := -std=c++11 -fopenmp -O2 -ffloat-store
ifeq ($(ARCH),x86)
CXXFLAGS += -msse2 -march=native
endif

LIB := \
    -lpthread \
    -L${DIVSURFSORT_PATH}/lib -ldivsufsort -ldivsufsort64 \
    -L${MKT_RTM_PATH_FILE}/lib \
    -lmicrokit \
    -Tmicrokit.ld 

COND_LIB := -lmpfr -lgmp
SHARED_LIB :=  -L${OPENSSL_PATH} -lcrypto -ldl
INC := -I${DIVSURFSORT_PATH}/include -I${OPENSSL_PATH}/include 

#===========================================================
# Targets
#===========================================================
non_iid.elf: non_iid_main.o
	$(CXX) $(CXXFLAGS) non_iid_main.o $(LIB) $(SHARED_LIB) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

clean:
	rm -f *.o non_iid.elf
	rm -f ea_iid ea_non_iid ea_restart ea_conditioning ea_transpose selftest/*.res
