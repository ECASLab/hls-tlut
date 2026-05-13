ifndef VERSION
$(error Debes especificar la version a compilar/correr. Ejemplo: make run VERSION=v2)
endif

CXX := g++
CXXFLAGS := -Wall -O3 -std=c++17 -I/opt/xilinx/xrt/include -I$(XILINX_HLS)/include -I../HW/$(VERSION) -I../HW
LDFLAGS := -L/opt/xilinx/xrt/lib -lOpenCL -pthread -lxrt_coreutil

TARGET := hw
XCLBIN := ../HW/package.$(TARGET).$(VERSION)/kernels_$(VERSION).xclbin
SRC := hw_tb.cpp
EXE := nla_hw_test

.PHONY: all clean run

all: $(EXE)

$(EXE): $(SRC)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

run: $(EXE)
	@echo "=========================================================="
	@echo "Ejecutando Testbench en FPGA Alveo (Hardware Real)"
	@echo "Bitstream seleccionado: $(XCLBIN)"
	@echo "=========================================================="
	./$(EXE) $(XCLBIN)

clean:
	rm -f $(EXE) hw_results.txt hw_sim.log