# Evitar que exija la VERSION si solo estamos haciendo un "make clean"
ifeq ($(filter clean,$(MAKECMDGOALS)),)
ifndef VERSION
$(error Debes especificar la version a usar. Ejemplo: make run VERSION=v2)
endif
endif

# Archivos fuente y ejecutables
SOURCES := hw_tb.cpp
EXECUTABLES := nla_hw_test

# Banderas estrictas
CFLAGS := -I/opt/xilinx/xrt/include -Wall -std=c++17 -O0 -g -I. -I./hls-types/include
LDFLAGS := -L/opt/xilinx/xrt/lib -pthread -lxrt_core -lxrt_coreutil

# Rutas dinámicas basadas en la versión
TARGET := hw
XCLBIN := ../HW/package.$(TARGET).$(VERSION)/kernels_$(VERSION).xclbin

.PHONY: all clean run

# Default target
all: $(EXECUTABLES)

# Apunta explícitamente a hw_tb.cpp
$(EXECUTABLES): $(SOURCES)
	$(CXX) -o $@ $< $(CFLAGS) $(LDFLAGS)

# Regla para ejecutar el bitstream dinámico
run: $(EXECUTABLES)
	@echo "=========================================================="
	@echo "  Iniciando validación HW - Bitstream: $(XCLBIN)"
	@echo "=========================================================="
	./$(EXECUTABLES) $(XCLBIN)

# Regla de limpieza
clean:
	$(RM) $(EXECUTABLES) hw_results.txt