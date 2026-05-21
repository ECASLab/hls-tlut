ifeq ($(filter clean,$(MAKECMDGOALS)),)
ifndef VERSION
$(error Debes especificar la version a usar. Ejemplo: make run VERSION=v2)
endif
endif

SOURCES := hw_tb.cpp
EXECUTABLES := nla_hw_test
CFLAGS := -I/opt/xilinx/xrt/include -Wall -std=c++17 -O0 -g -I. -I./hls-types/include
LDFLAGS := -L/opt/xilinx/xrt/lib -pthread -lxrt_core -lxrt_coreutil

TARGET := hw
XCLBIN := ../HW/package.$(TARGET).$(VERSION)/kernels_$(VERSION).xclbin

.PHONY: all clean run

all: $(EXECUTABLES)

$(EXECUTABLES): $(SOURCES)
	$(CXX) -o $@ $< $(CFLAGS) $(LDFLAGS)

run: $(EXECUTABLES)
	@echo "=========================================================="
	@echo "  Iniciando validación HW - Bitstream: $(XCLBIN)"
	@echo "=========================================================="
	./$(EXECUTABLES) $(XCLBIN) $(VERSION)

clean:
	$(RM) $(EXECUTABLES) hw_results_*.txt hw_interaction_*.log