VERSION := v2

TB ?= hw_tb
SAMPLES ?= 1000

EXECUTABLES := hw_tb softmax_tb tlut_tb

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
	@echo "  Muestras (Samples): $(SAMPLES)"
	@echo "=========================================================="
	./$(TB) $(XCLBIN) $(VERSION) $(SAMPLES)

clean:
	$(RM) $(EXECUTABLES) hw_results_*.txt hw_interaction_*.log