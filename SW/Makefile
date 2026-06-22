VERSION := v2

TB ?= hw_tb
SAMPLES ?= 1000
X_MIN ?= -10
X_MAX ?= 10

EXECUTABLES := hw_tb hw_tb_2 hw_tb_3 softmax_tb tlut_tb host_tb

CFLAGS := -I/opt/xilinx/xrt/include -Wall -std=c++17 -O0 -g -I. -I./hls-types/include
LDFLAGS := -L/opt/xilinx/xrt/lib -pthread -lxrt_core -lxrt_coreutil

TARGET := hw
XCLBIN := ../HW/package.$(TARGET).$(VERSION)/kernels_$(VERSION).xclbin

.PHONY: all clean run

all: $(EXECUTABLES)

%: %.cpp
	$(CXX) -o $@ $< $(CFLAGS) $(LDFLAGS)

run: $(TB)
	@echo "=========================================================="
	@echo "  Iniciando Testbench: $(TB)"
	@echo "  Bitstream: $(XCLBIN)"
	@echo "=========================================================="

ifeq ($(TB),host_tb)
	./$(TB) $(XCLBIN) $(SAMPLES) $(X_MIN) $(X_MAX)
else ifeq ($(TB),hw_tb)
	./$(TB) $(XCLBIN) $(SAMPLES)
else
	./$(TB) $(XCLBIN)
endif

clean:
	$(RM) $(EXECUTABLES) hw_results_*.txt hw_interaction_*.log