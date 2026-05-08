import os
import math
import sys

# ==========================================
# 1. CONFIGURACIÓN GLOBAL Y LÍMITES Q6.10
# ==========================================
Q_INT = 6
Q_FRAC = 10
SCALE = 1 << Q_FRAC  # 1024
RESOLUTION = 1.0 / SCALE

# Límites de precisión Q6.10 (6 bits enteros con signo -> -32 a 31.99)
MAX_Y_LIMIT = (1 << (Q_INT - 1)) - RESOLUTION  # 31.999
MIN_Y_LIMIT = -(1 << (Q_INT - 1))              # -32.0

B_SIZE = 8
W_E = int(math.log2(B_SIZE))
MAX_E = (1 << W_E) - 1

OUTPUT_FILE = os.path.join(os.path.dirname(__file__), f"tluts_B{B_SIZE}.h")

# ==========================================
# 2. DEFINICIÓN DE FUNCIONES
# ==========================================
# (func, x_min, x_max, c_sym, c_upper, c_lower, use_sym, use_lin)
FUNCTIONS = {
    "sigmoid":  (lambda x: 1.0 / (1.0 + math.exp(-x)), -6.0, 6.0,  1.0, 1.0,  0.0, 1, 0),
    "tanh":     (math.tanh,                            -4.0, 4.0,  0.0, 1.0, -1.0, 1, 0),
    "softsign": (lambda x: x / (1.0 + abs(x)),         -8.0, 8.0,  0.0, 1.0, -1.0, 1, 0),
    "erf":      (math.erf,                             -3.0, 3.0,  0.0, 1.0, -1.0, 1, 0),
    "swish":    (lambda x: x / (1.0 + math.exp(-x)),   -6.0, 6.0,  0.0, 0.0,  0.0, 0, 1),
    "gelu":     (lambda x: 0.5 * x * (1.0 + math.erf(x / math.sqrt(2.0))), -4.0, 4.0, 0.0, 0.0, 0.0, 0, 1),
    "softplus": (lambda x: math.log(1.0 + math.exp(x)), -5.0, 4.0,  0.0, 0.0,  0.0, 0, 1),
    "mish":     (lambda x: x * math.tanh(math.log(1.0 + math.exp(x))), -5.0, 3.0, 0.0, 0.0, 0.0, 0, 1),
    "elu":      (lambda x: (math.exp(x) - 1.0),        -6.0, 0.0,  0.0, 0.0, -1.0, 0, 1),
    "exp":      (math.exp,                             -8.0, 3.0,  0.0, 0.0,  0.0, 0, 0),
    "sqrt":     (math.sqrt,                             0.0, 10.0, 0.0, 0.0,  0.0, 0, 0),
    "relu":     (lambda x: x if x > 0.0 else 0.0,      -1.0, 1.0,  0.0, 0.0,  0.0, 0, 1),
}

# ==========================================
# 3. GENERACIÓN DE LA ARQUITECTURA T-LUT
# ==========================================
def generate_luts():
    try:
        with open(OUTPUT_FILE, "w") as f:
            f.write("// Archivo t_LUTS.h - Arquitectura t-LUT (LUT + d-LUT + e-LUT)\n")
            f.write(f"// Formato Datos: Q{Q_INT}.{Q_FRAC} | Banda (B): {B_SIZE}\n\n")

            for name, data in FUNCTIONS.items():
                func, x_min, x_max, c_sym, c_upper, c_lower, use_sym, use_lin = data
                
                # Rango de guardado: si hay simetria, empezamos en 0
                store_start = 0.0 if use_sym == 1 else x_min
                start_idx = int(round(store_start * SCALE))
                end_idx = int(round(x_max * SCALE))
                D0 = (end_idx - start_idx) + 1
                
                lut_raw = []
                for i in range(D0):
                    curr_x = (start_idx + i) / SCALE
                    try:
                        val_y = func(curr_x)
                        
                        # ADVERTENCIA DE PRECISIÓN Q6.10
                        if val_y > MAX_Y_LIMIT or val_y < MIN_Y_LIMIT:
                            print(f"\n[ERROR CRITICO] {name.upper()}: Desbordamiento de precision Q6.10.")
                            print(f"En X = {curr_x}, Y = {val_y}. El maximo es {MAX_Y_LIMIT}.")
                            sys.exit(1)
                            
                        lut_raw.append(int(round(val_y * SCALE)))
                    except (OverflowError, ValueError):
                        lut_raw.append(0)

                # Descomposicion Twofold
                lut_hw, d_lut, e_lut = [], [], []
                for i in range(0, D0, B_SIZE):
                    banda = lut_raw[i : i + B_SIZE]
                    if not banda: break
                    
                    base_min = min(banda)
                    d_lut.append(base_min)
                    
                    for val in banda:
                        err = min(val - base_min, MAX_E)
                        e_lut.append(err)
                        lut_hw.append(base_min + err)

                # Exportación al archivo
                uname = name.upper()
                f.write(f"// --- {uname} (Guardado: {store_start} a {x_max}) ---\n")
                
                f.write(f"static const int {uname}_LUT[] = {{ ")
                f.write(", ".join(map(str, lut_hw)))
                f.write(" };\n")
                
                f.write(f"static const int {uname}_DLUT[] = {{ ")
                f.write(", ".join(map(str, d_lut)))
                f.write(" };\n")
                
                f.write(f"static const int {uname}_ELUT[] = {{ ")
                f.write(", ".join(map(str, e_lut)))
                f.write(" };\n")
                
                # Control literal del mundo real (Enteros sin escalar)
                ctrl_values = [int(c_sym), int(x_max), int(x_min), int(c_upper), int(c_lower), int(use_sym), int(use_lin)]
                f.write(f"static const int {uname}_CONTROL[] = {{ " + ", ".join(map(str, ctrl_values)) + " };\n\n")

        print(f"Finalizado: '{OUTPUT_FILE}' generado con exito.")
        
    except IOError as e:
        print(f"Error al escribir el archivo: {e}")

if __name__ == "__main__":
    generate_luts()