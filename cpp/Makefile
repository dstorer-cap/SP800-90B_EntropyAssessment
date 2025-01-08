#===========================================================
# Layout
#===========================================================
DEP_PATH := ../../../dep
SRC_PATH := ../../../src
TMP_PATH := tmp
OUT_PATH := ../../../out

DEP_DTS_PATH := ${DEP_PATH}/dts
DEP_GST_PATH := ${DEP_PATH}/guest
DEP_LVM_PATH := ${DEP_PATH}/libvmm
DEP_MKT_PATH := ${DEP_PATH}/microkit
DEP_HLT_PATH := ${DEP_PATH}/ent_random_sequence_tester
DEP_PLC_PATH := ${DEP_PATH}/picolibc

CPU := cortex-a53
TCH := aarch64-none-elf
CC := ${TCH}-gcc
LD := ${TCH}-ld
AS := ${TCH}-as

MKT_BOARD := maaxboard
MKT_CONFIG := debug

MKT_SDK_PATH := ${DEP_MKT_PATH}/out/microkit-sdk-1.4.1
MKT_PATH_FILE := ${MKT_SDK_PATH}/bin/microkit
MKT_RTM_PATH_FILE := ${MKT_SDK_PATH}/board/${MKT_BOARD}/${MKT_CONFIG}
PICOLIBC_PATH = ${DEP_PLC_PATH}/picolibc_build/picolibc/aarch64-linux-gnu

DIVSURFSORT_PATH := /home/dstorer/svn_apricot/apricot/development/wip/daniel/single-linux-guest-nitrokey/dep/SP800-90B_EntropyAssessment/libdivsufsort/build

ARCH := aarch64

override CXX := aarch64-none-elf-g++

CXXFLAGS := \
    -c \
    -mcpu=${CPU} \
    -ffreestanding \
    -nostdlib \
    -mstrict-align \
    -g3 \
    -O3 \
    -Wall \
    -Wno-unused-function \
    -std=c++11

CC_OPS := \
    -c \
    -mcpu=${CPU} \
    -ffreestanding \
    -nostdlib \
    -mstrict-align \
    -g3 \
    -O3 \
    -Wall \
    -Wno-unused-function \

LIB := \
    # -L${MKT_RTM_PATH_FILE}/lib \
    # -lmicrokit \
    # -Tmicrokit.ld 

COND_LIB := -lmpfr -lgmp
SHARED_LIB := 
INC := -I${DIVSURFSORT_PATH}/include 

#===========================================================
# Targets
#===========================================================


# non_iid.elf: ${TMP_PATH}/non_iid_main.o
# 	$(CXX) $(CXXFLAGS) ${TMP_PATH}/non_iid_main.o $(LIB) $(SHARED_LIB) -o $@

all: ${TMP_PATH}/non_iid_test.o ${TMP_PATH}/c_interface.o

${TMP_PATH}/non_iid_test.o: non_iid_test.cpp | ${TMP_PATH}
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

${TMP_PATH}/c_interface.o: c_interface.c | ${TMP_PATH}
	$(CC) ${CC_OPS} -I ${MKT_RTM_PATH_FILE}/include -I../../../src/ -I${PICOLIBC_PATH}/include/  -c $< -o $@

${TMP_PATH}:
	mkdir -p ${TMP_PATH}

# clean:
# 	rm -f *.o non_iid.elf
# 	rm -f ea_iid ea_non_iid ea_restart ea_conditioning ea_transpose selftest/*.res