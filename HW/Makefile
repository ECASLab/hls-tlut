ifndef VERSION
$(error Debes especificar la version a compilar. Ejemplo: make VERSION=v1)
endif

# Setting relevant variables
ROOT_DIR=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))
TARGET := hw

PLATFORM ?= xilinx_u55c_gen3x16_xdma_3_202210_1

# Archivo fuente que ahora depende de la carpeta de la versión elegida
HLS_FILES := $(VERSION)/nla_core.cpp
KERNEL_NAME := nla_top

# Setting directories (Separados por versión para evitar sobreescrituras)
TEMP_DIR := ./tmp.$(TARGET).$(VERSION)
BUILD_DIR := ./build.$(TARGET).$(VERSION)
PACKAGE_OUT = ./package.$(TARGET).$(VERSION)
LINK_OUTPUT := $(BUILD_DIR)/kernels.link.xclbin
EMCONFIG_DIR = $(TEMP_DIR)
HLS_BUILD = $(TEMP_DIR)/%.xo

# Packaging: El binario final tendrá la versión en el nombre
XCL_BIN := $(PACKAGE_OUT)/kernels_$(VERSION).xclbin

# Output files
HLS_KERNEL_FILES := $(TEMP_DIR)/$(KERNEL_NAME).xo

# Setting V++ Flags (Se agregó -I$(VERSION) para encontrar nla_core.h)
VPP_PFLAGS :=
VPP_LDFLAGS :=
VPP_FLAGS += --save-temps --jobs 32 -I$(VERSION)

# Set the desired clock frequency (e.g., 200 MHz)
KERNEL_FREQ := 250

USE_FLOAT32 := 1
ifdef USE_FLOAT32
		VPP_FLAGS += -DUSE_FLOAT32
endif

RMDIR = rm -rf

.PHONY: all clean cleanall hls-build build

all: build

build: check_platform emconfig $(LINK_OUTPUT) $(XCL_BIN)

check_platform:
ifndef PLATFORM
		$(error PLATFORM not set. Please set the PLATFORM properly and rerun. Run "make help" for more details.)
endif

$(TEMP_DIR)/$(KERNEL_NAME).xo: $(HLS_FILES)
		mkdir -p $(TEMP_DIR)
		v++ -c $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) -k $(KERNEL_NAME) --temp_dir $(TEMP_DIR) --kernel_frequency $(KERNEL_FREQ) -o '$@' $^

$(LINK_OUTPUT): $(HLS_KERNEL_FILES)
		mkdir -p $(BUILD_DIR)
		v++ -l $(VPP_FLAGS) $(VPP_LDFLAGS) -t $(TARGET) --platform $(PLATFORM) --temp_dir $(TEMP_DIR) --kernel_frequency $(KERNEL_FREQ) -o '$(LINK_OUTPUT)' $^

hls-build: check_platform emconfig $(HLS_KERNEL_FILES)

emconfig: $(EMCONFIG_DIR)/emconfig.json

$(EMCONFIG_DIR)/emconfig.json:
		emconfigutil --platform $(PLATFORM) --od $(EMCONFIG_DIR)

$(XCL_BIN): $(LINK_OUTPUT)
		mkdir -p $(PACKAGE_OUT)
		v++ -p $(LINK_OUTPUT) $(VPP_FLAGS) -t $(TARGET) --platform $(PLATFORM) --package.out_dir $(PACKAGE_OUT) --kernel_frequency $(KERNEL_FREQ) -o $(XCL_BIN)

# Cleaning rules
clean:
		$(RMDIR) $(EXECUTABLE) *.xclbin/{*sw_emu*,*hw_emu*}
		$(RMDIR) profile_* TempConfig system_estimate.xtxt *.rpt *.csv
		$(RMDIR) src/*.ll *v++* .Xil emconfig.json dltmp* xmltmp* *.log *.jou *.wcfg *.wdb

cleanall: clean
		$(RMDIR) build_dir* sd_card* package.* tmp.* build.*
		$(RMDIR) _x* *xclbin.run_summary qemu-memory-_* emulation _vimage pl* start_simulation.sh *.xclbin