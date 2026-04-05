import math

# ==========================================
# 1. CONFIGURACION GLOBAL
# ==========================================
Q_INT = 6       # Bits de la parte entera
Q_FRAC = 10     # Bits de la parte fraccionaria (j)
B = 8           # Tamano de banda (B)
OUTPUT_FILE = "t-luts.h"

ALPHA_ELU = 1.0

# Funciones y sus rangos de evaluacion [LT, HT]
FUNCTIONS = {
    "tanh":     (math.tanh, 0.0, 4.0),
    "exp":      (math.exp, -8.0, 8.0),
    "sqrt":     (math.sqrt, 0.0, 8.0),
    "exp_neg":  (lambda x: math.exp(-x), 0.0, 8.0),
    "sigmoid":  (lambda x: 1.0 / (1.0 + math.exp(-x)), -8.0, 8.0),
    "softplus": (lambda x: math.log(1.0 + math.exp(x)), -8.0, 8.0),
    "gelu":     (lambda x: 0.5 * x * (1.0 + math.erf(x / math.sqrt(2.0))), -8.0, 8.0),
    "elu":      (lambda x: x if x >= 0 else ALPHA_ELU * (math.exp(x) - 1.0), -8.0, 8.0),
    "relu":     (lambda x: max(0.0, x), 0.0, 8.0),
    "silu":     (lambda x: x / (1.0 + math.exp(-x)), -8.0, 8.0)
}

# ==========================================
# 2. GENERACION DE LA ARQUITECTURA T-LUT
# ==========================================
def generate_luts():
    resolution = 2 ** -Q_FRAC
    scale = 2 ** Q_FRAC
    
    with open(OUTPUT_FILE, "w") as f:
        f.write(f"// Archivo de Tablas t-LUT precalculadas\n")
        f.write(f"// Formato: Q{Q_INT}.{Q_FRAC} | Banda (B): {B} | Resolucion: {resolution}\n\n")
        
        for name, data in FUNCTIONS.items():
            func, start, end = data
            
            # --- 1. CALCULO EXACTO DE LA PROFUNDIDAD (D0) ---
            # Formula: D0 = ((HT - LT) / intervalo_muestreo) + 1
            D0 = int(round((end - start) * scale)) + 1
            
            # --- 2. GENERACION DE LUT ORIGINAL ---
            lut_original = []
            
            for i in range(D0):
                # Calculo de x sin acumulacion de errores de precision
                x = start + (i * resolution)
                
                try:
                    y_val = func(x)
                    y_q_format = int(round(y_val * scale))
                except OverflowError:
                    y_q_format = int(round(float('inf'))) if x > 0 else 0
                
                lut_original.append(y_q_format)
            
            # --- 3. DESCOMPOSICION EN T-LUT (d-LUT y e-LUT) ---
            d_lut = []
            e_lut = []
            
            for i in range(0, D0, B):
                banda = lut_original[i : i + B]
                min_val = min(banda)
                
                d_lut.append(min_val)
                for val in banda:
                    e_lut.append(val - min_val)
            
            # --- 4. EXPORTACION CON METADATOS ---
            f.write(f"// --- Funcion: {name.upper()} | Rango: [{start}, {end}] ---\n")
            f.write(f"// Profundidad LUT (D0): {D0} | d-LUT (Dd): {len(d_lut)} | e-LUT (De): {len(e_lut)}\n")
            
            f.write(f"static const int LUT_{name.upper()}[] = {{\n    ")
            f.write(", ".join(map(str, lut_original)))
            f.write("\n};\n\n")
            
            f.write(f"static const int DLUT_{name.upper()}[] = {{\n    ")
            f.write(", ".join(map(str, d_lut)))
            f.write("\n};\n\n")
            
            f.write(f"static const int ELUT_{name.upper()}[] = {{\n    ")
            f.write(", ".join(map(str, e_lut)))
            f.write("\n};\n\n")

if __name__ == "__main__":
    generate_luts()