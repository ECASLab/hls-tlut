# ----------------------------------------------------------------------------
# Copyright (c) 2026 Sergio Porras Escobar <sporras29@estudiantec.cr>
# ----------------------------------------------------------------------------

import os
import math
import sys

# ==========================================
# 1. CONFIGURACIÓN GLOBAL Y LÍMITES
# ==========================================
Q_INT = 6
Q_FRAC = 10
B_SIZE = 16

FORMAT_NAME = f"Q{Q_INT}_{Q_FRAC}"  # Crea el nombre de la carpeta base
SCALE = 1 << Q_FRAC  # 1024
RESOLUTION = 1.0 / SCALE

# Límites de precisión Q6.10
MAX_Y_LIMIT = (1 << (Q_INT - 1)) - RESOLUTION  # 31.999
MIN_Y_LIMIT = -(1 << (Q_INT - 1))              # -32.0

W_E = int(math.log2(B_SIZE))
MAX_E = (1 << W_E) - 1

# Directorio base: ./tluts/Q6_10/
BASE_DIR = os.path.join(os.path.dirname(__file__), "tluts", FORMAT_NAME)

# ==========================================
# 2. DEFINICIÓN DE FUNCIONES
# ==========================================
# (func, lower_threshold, upper_threshold, c_lower, c_upper, c_sym, use_sym, use_lin)
FUNCTIONS = {
    "sigmoid":  (lambda x: 1.0 / (1.0 + math.exp(-x)), -6.0, 6.0,   0.0, 1.0,  1.0, 1, 0),
    "tanh":     (math.tanh,                            -4.0, 4.0,  -1.0, 1.0,  0.0, 1, 0),
    "softsign": (lambda x: x / (1.0 + abs(x)),         -8.0, 8.0,  -1.0, 1.0,  0.0, 1, 0),
    "erf":      (math.erf,                             -3.0, 3.0,  -1.0, 1.0,  0.0, 1, 0),
    "swish":    (lambda x: x / (1.0 + math.exp(-x)),   -6.0, 6.0,   0.0, 0.0,  0.0, 0, 1),
    "gelu":     (lambda x: 0.5 * x * (1.0 + math.erf(x / math.sqrt(2.0))), -4.0, 4.0, 0.0, 0.0, 0.0, 0, 1),
    "softplus": (lambda x: math.log(1.0 + math.exp(x)), -5.0, 4.0,   0.0, 0.0,  0.0, 0, 1),
    "mish":     (lambda x: x * math.tanh(math.log(1.0 + math.exp(x))), -5.0, 3.0, 0.0, 0.0, 0.0, 0, 1),
    "elu":      (lambda x: (math.exp(x) - 1.0),        -6.0, 0.0,  -1.0, 0.0,  0.0, 0, 1),
    "exp":      (math.exp,                             -1.0, 1.0,   0.0, 0.0,  0.0, 0, 0),
    "sqrt":     (math.sqrt,                             0.0, 10.0,  0.0, 0.0,  0.0, 0, 0),
    "relu":     (lambda x: x if x > 0.0 else 0.0,      -1.0, 1.0,   0.0, 0.0,  0.0, 0, 1),
}

# ==========================================
# 3. GENERACIÓN DE LA ARQUITECTURA T-LUT
# ==========================================
def generate_luts():
    try:
        # Crea el directorio base si no existe
        os.makedirs(BASE_DIR, exist_ok=True)

        for name, data in FUNCTIONS.items():
            # Desempaquetado según el nuevo orden
            func, lower_threshold, upper_threshold, c_lower, c_upper, c_sym, use_sym, use_lin = data
            
            # Rango de guardado: si hay simetría, empezamos en 0
            store_start = 0.0 if use_sym == 1 else lower_threshold
            start_idx = int(round(store_start * SCALE))
            end_idx = int(round(upper_threshold * SCALE))
            D0 = (end_idx - start_idx) + 1
            
            lut_raw = []
            for i in range(D0):
                curr_x = (start_idx + i) / SCALE
                try:
                    val_y = func(curr_x)
                    
                    if val_y > MAX_Y_LIMIT or val_y < MIN_Y_LIMIT:
                        print(f"\n[ERROR CRITICO] {name.upper()}: Desbordamiento de precision {FORMAT_NAME}.")
                        print(f"En X = {curr_x}, Y = {val_y}. El maximo es {MAX_Y_LIMIT}.")
                        sys.exit(1)
                        
                    lut_raw.append(int(round(val_y * SCALE)))
                except (OverflowError, ValueError):
                    lut_raw.append(0)

            # Descomposicion Twofold
            d_lut, e_lut = [], []
            for i in range(0, D0, B_SIZE):
                banda = lut_raw[i : i + B_SIZE]
                if not banda: break
                
                base_min = min(banda)
                d_lut.append(base_min)
                
                for val in banda:
                    err = min(val - base_min, MAX_E)
                    e_lut.append(err)

            # ----- GUARDADO EN ARCHIVOS INDIVIDUALES -----
            func_dir = os.path.join(BASE_DIR, name)
            os.makedirs(func_dir, exist_ok=True)
            
            # Guardar DLUT
            with open(os.path.join(func_dir, "dlut.txt"), "w") as f:
                f.write("\n".join(map(str, d_lut)))
                
            # Guardar ELUT
            with open(os.path.join(func_dir, "elut.txt"), "w") as f:
                f.write("\n".join(map(str, e_lut)))
            
            # Guardar CONTROL (Escalado y exportado en el orden exacto solicitado)
            ctrl_values = [
                int(round(lower_threshold * SCALE)),
                int(round(upper_threshold * SCALE)),
                int(round(c_lower * SCALE)),
                int(round(c_upper * SCALE)),
                int(round(c_sym * SCALE)),
                int(use_sym),
                int(use_lin)
            ]
            with open(os.path.join(func_dir, "control.txt"), "w") as f:
                f.write("\n".join(map(str, ctrl_values)))

        print(f"Finalizado: Archivos generados con exito en '{BASE_DIR}'.")
        
    except IOError as e:
        print(f"Error al escribir los archivos: {e}")

if __name__ == "__main__":
    generate_luts()