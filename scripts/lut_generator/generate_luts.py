import os
import numpy as np
import matplotlib.pyplot as plt
import shutil

SCALING_FACTOR_UP = 1e9
SCALING_FACTOR_DOWN = 1e-9
MAX_EXP_INPUT = 88.0
PRECISIONS = [8, 12, 16]
MSE_MAX_FILTER = 1e-3 # Umbral de MSE para filtrar curvas en las gráficas.
FUNCTIONS_CONFIG = {
    'tanh':     {'limits': [-8.0, 8.0], 'symmetric': True,  'positive_only': False},
    'exp':      {'limits': [-8.0, 8.0], 'symmetric': False, 'positive_only': True},
    'sqrt':     {'limits': [0.0, 32.0], 'symmetric': False, 'positive_only': True},
    'gelu':     {'limits': [-8.0, 8.0], 'symmetric': False, 'positive_only': False},
    'softmax':  {'limits': [-8.0, 8.0], 'symmetric': False, 'positive_only': True},
    'sigmoid':  {'limits': [-8.0, 8.0], 'symmetric': False, 'positive_only': False},
    'softplus': {'limits': [-8.0, 8.0], 'symmetric': False, 'positive_only': False},
    'elu':      {'limits': [-8.0, 8.0], 'symmetric': False, 'positive_only': False},
    'swish':    {'limits': [-8.0, 8.0], 'symmetric': False, 'positive_only': False}
}

def eval_func_safe(name, x):
    try:
        if name == 'tanh':
            res = np.tanh(x)
        elif name in ['exp', 'softmax']:
            x_safe = np.clip(x, -MAX_EXP_INPUT, np.log(SCALING_FACTOR_UP))
            res = np.exp(x_safe)
        elif name == 'sqrt':
            res = np.sqrt(np.maximum(x, 0))
        elif name == 'gelu':
            res = 0.5 * x * (1.0 + np.tanh(0.7978845 * (x + 0.044715 * (x ** 3))))
        elif name == 'sigmoid':
            x_safe = np.clip(x, -np.log(SCALING_FACTOR_UP), np.log(SCALING_FACTOR_UP))
            res = 1.0 / (1.0 + np.exp(-x_safe))
        elif name == 'softplus':
            res = np.where(x > 20, x, np.log(1.0 + np.exp(np.clip(x, -MAX_EXP_INPUT, 20))))
        elif name == 'elu':
            res = np.where(x > 0, x, 1.0 * (np.exp(np.clip(x, -MAX_EXP_INPUT, 20)) - 1.0))
        elif name == 'swish':
            x_safe = np.clip(x, -MAX_EXP_INPUT, MAX_EXP_INPUT)
            res = x * (1.0 / (1.0 + np.exp(-x_safe)))
        else:
            res = np.zeros_like(x)
        return np.clip(res, -SCALING_FACTOR_UP, SCALING_FACTOR_UP)
    except Exception as e:
        print(f"Error evaluando {name}: {e}")
        return np.zeros_like(x)

def main():
    if os.path.exists("output"):
        shutil.rmtree("output")
    os.makedirs("output", exist_ok=True)
    
    global_results_per_prec = {f: {nb: [] for nb in PRECISIONS} for f in FUNCTIONS_CONFIG}
    global_all_mses_per_prec = {nb: [] for nb in PRECISIONS}

    use_opt_str = input("¿Optimizar rango para funciones simetricas/positivas? (y/n) [n]: ").strip().lower()
    apply_domain_opt = use_opt_str == 'y'

    for func, config in FUNCTIONS_CONFIG.items():
        func_dir = os.path.join("output", func)
        os.makedirs(func_dir, exist_ok=True)
        best_curves_per_prec = {}
        
        ideal_limits = config['limits']
        is_sym = config['symmetric']
        is_pos = config['positive_only']
        
        if apply_domain_opt and (is_sym or is_pos):
            eval_min = 0.0
            eval_max = ideal_limits[1] / 2.0 if is_sym else ideal_limits[1]
        else:
            eval_min = ideal_limits[0]
            eval_max = ideal_limits[1]
            
        x_ideal = np.linspace(eval_min, eval_max, 5000)
        y_ideal = eval_func_safe(func, x_ideal)
        
        for n_bits in PRECISIONS:
            if n_bits not in [8, 12, 16]:
                continue
                
            results = []
            curves_for_this_prec = []
            best_mse = float('inf')
            best_data = None
            total_lines = 1 << n_bits
            
            for frac_bits in range(n_bits + 1):
                int_bits = n_bits - frac_bits
                
                if apply_domain_opt and (is_sym or is_pos):
                    step = 1.0 / (1 << (frac_bits + 1))
                    min_int_val = 0
                    max_int_val = total_lines - 1
                else:
                    step = 1.0 / (1 << frac_bits) if frac_bits > 0 else 1.0
                    min_int_val = -(1 << (n_bits - 1))
                    max_int_val = (1 << (n_bits - 1)) - 1
                
                min_q_val = min_int_val * step
                max_q_val = max_int_val * step
                
                x_ints_lut = np.arange(min_int_val, max_int_val + 1)
                x_quant_lut = x_ints_lut * step
                
                y_real_lut = eval_func_safe(func, x_quant_lut)
                y_scaled_lut = np.round(y_real_lut / step)
                y_q_ints_lut = np.clip(y_scaled_lut, min_int_val, max_int_val)
                y_quant_lut = y_q_ints_lut * step
                
                x_scaled = np.round(x_ideal / step)
                x_q_ints = np.clip(x_scaled, min_int_val, max_int_val)
                x_quant = x_q_ints * step
                
                y_real = eval_func_safe(func, x_quant)
                y_scaled = np.round(y_real / step)
                y_q_ints_calc = np.clip(y_scaled, min_int_val, max_int_val)
                y_quant = y_q_ints_calc * step
                
                errors = np.abs(y_ideal - y_quant)
                mse = np.mean(errors**2)
                
                if mse == 0.0 and len(errors) > 0:
                    mse = SCALING_FACTOR_DOWN
                    
                q_format = f"Q{int_bits}.{frac_bits}"
                results.append((mse, q_format, step, total_lines, min_q_val, max_q_val))
                global_results_per_prec[func][n_bits].append((mse, q_format))
                global_all_mses_per_prec[n_bits].append(mse)
                
                if mse <= MSE_MAX_FILTER:
                    curves_for_this_prec.append((q_format, x_ideal, y_quant))
                    
                if mse < best_mse:
                    best_mse = mse
                    best_data = (q_format, x_ideal, y_ideal, y_quant)

                calc_dir = os.path.join(func_dir, f"{func}_calcs")
                os.makedirs(calc_dir, exist_ok=True)
                calc_path = os.path.join(calc_dir, f"{func}_{n_bits}bit_{q_format}.txt")
                
                with open(calc_path, "w") as fcalc:
                    fcalc.write(f"# FUNCIÓN {func.upper()}\n")
                    fcalc.write(f"# Precision en Bits_Totales: {n_bits} | Formato: {q_format}\n")
                    fcalc.write(f"# Error estadístico de Cuantización (MSE): {mse:.6e}\n")
                    fcalc.write(f"# Rango de Operación: [{min_q_val:.4f}, {max_q_val:.4f}] | Amplitud Min: {np.min(y_quant_lut):.4f} | Amplitud Max: {np.max(y_quant_lut):.4f}\n")
                    fcalc.write(f"# Resolución Delta de paso: {step:.6f} | Es Simetrica: {is_sym}\n")
                    fcalc.write(f"# Total Lineas LUT: {total_lines}\n\n")
                    
                    header = f"{'Index':<8} | {'Input_Bin':^{n_bits}} | {'x':>12} | {'f(x)':>12}\n"
                    fcalc.write(header)
                    fcalc.write("-" * len(header.strip()) + "\n")
                    
                    for i in range(total_lines):
                        x_bin = format(int(x_ints_lut[i]) & ((1 << n_bits) - 1), f'0{n_bits}b')
                        x_str = f"{x_quant_lut[i]:12.6f}"
                        fx_str = f"{y_quant_lut[i]:12.6f}"
                        fcalc.write(f"{i:<8} | {x_bin} | {x_str} | {fx_str}\n")
                        
            results.sort(key=lambda x: x[0])
            report_path = os.path.join(func_dir, f"{func}_{n_bits}bit.txt")
            
            with open(report_path, "w") as f:
                f.write(f"{'Rank':<5} | {'Q_Format':<8} | {'Step':>10} | {'Lineas_LUT':>12} | {'Range_Min':>12} | {'Range_Max':>12} | {'MSE':>15}\n")
                f.write("-" * 96 + "\n")
                for i, r in enumerate(results):
                    f.write(f"{i+1:<5} | {r[1]:<8} | {r[2]:>10.6f} | {r[3]:>12} | {r[4]:>12.4f} | {r[5]:>12.4f} | {r[0]:>15.8e}\n")
                    
            if best_data:
                best_curves_per_prec[n_bits] = best_data
                
            plt.figure(figsize=(12, 7))
            plt.plot(x_ideal, y_ideal, color='gray', linestyle='-', linewidth=3, alpha=0.4, label="Ideal (Float)")
            for q_fmt, x_v, y_q in curves_for_this_prec:
                plt.plot(x_v, y_q, label=q_fmt, alpha=0.8, linewidth=1.2)
            plt.plot([0], [0], marker='o', color='red', label='(0,0)', markersize=6)
            plt.xlim(eval_min, eval_max)
            y_min_ideal, y_max_ideal = np.min(y_ideal), np.max(y_ideal)
            y_margin = (y_max_ideal - y_min_ideal) * 0.1 if y_max_ideal != y_min_ideal else 1.0
            plt.ylim(y_min_ideal - y_margin, y_max_ideal + y_margin)
            plt.title(f"Comparativa de Formatos Q - {func.upper()} ({n_bits} Bits)")
            plt.xlabel("Vector de Entrada (X)")
            plt.ylabel("Amplitud Cuantizada f(X)")
            plt.grid(True, linestyle=':', alpha=0.6)
            plt.legend(bbox_to_anchor=(1.02, 1), loc='upper left', fontsize='small', ncol=1 if n_bits <= 8 else 2)
            plt.tight_layout()
            plt.savefig(os.path.join(func_dir, f"{func}_{n_bits}bit.png"))
            plt.close()

        if best_curves_per_prec:
            plt.figure(figsize=(10, 6))
            plt.plot(x_ideal, y_ideal, color='gray', linestyle='-', linewidth=3, alpha=0.4, label="Ideal (Float)")
            plt.plot([0], [0], marker='o', color='red', label='(0,0)', markersize=6)
            line_styles = ['--', '-.', ':', '--', '-.']
            
            for idx, n_bits in enumerate(PRECISIONS):
                if n_bits in best_curves_per_prec:
                    fmt, x_v, y_ideal_v, y_q = best_curves_per_prec[n_bits]
                    mse_curve = np.mean((y_ideal_v - y_q) ** 2)
                    ls = line_styles[idx % len(line_styles)]
                    plt.plot(x_v, y_q, label=f"{n_bits}b ({fmt}) | MSE={mse_curve:.2e}", linestyle=ls, alpha=0.8, linewidth=1.5)
            
            plt.xlim(eval_min, eval_max)
            plt.ylim(y_min_ideal - y_margin, y_max_ideal + y_margin)
            plt.title(f"Mejor Representacion por Precision - {func.upper()}")
            plt.xlabel("Vector de Entrada (X)")
            plt.ylabel("Amplitud Cuantizada f(X)")
            plt.grid(True, which='both', linestyle=':', alpha=0.6)
            plt.legend()
            plt.tight_layout()
            plt.savefig(os.path.join(func_dir, f"{func}_best_curves.png"))
            plt.close()

    report_general_path = os.path.join("output", "reporte_general.txt")
    global_max_int_per_prec = {nb: 0 for nb in PRECISIONS}
    
    with open(report_general_path, "w") as f:
        f.write("========================================================\n")
        f.write(" REPORTE GENERAL: BUSQUEDA DE FORMATO Q UNIVERSAL (Q I.F)\n")
        f.write("========================================================\n\n")
        
        for func in FUNCTIONS_CONFIG:
            f.write(f"--- Análisis para: {func.upper()} ---\n")
            for nb in PRECISIONS:
                resultados = global_results_per_prec[func][nb]
                if not resultados:
                    continue
                
                resultados.sort(key=lambda x: x[0])
                top4 = resultados[:4]
                
                max_int_local = 0
                f.write(f"  Precisión {nb} bits:\n")
                for rank, (mse, q_fmt) in enumerate(top4, 1):
                    int_bits = int(q_fmt.replace('Q', '').split('.')[0])
                    max_int_local = max(max_int_local, int_bits)
                    f.write(f"    Top {rank}: {q_fmt:<6} | MSE: {mse:.2e} | Bits Enteros: {int_bits}\n")
                
                f.write(f"    -> Bits Enteros máximos necesarios (Top 4): {max_int_local}\n\n")
                global_max_int_per_prec[nb] = max(global_max_int_per_prec[nb], max_int_local)
                
        f.write("========================================================\n")
        f.write(" CONCLUSIÓN GLOBAL: FORMATO UNIVERSAL RECOMENDADO\n")
        f.write("========================================================\n")
        for nb in PRECISIONS:
            i_bits = global_max_int_per_prec[nb]
            f_bits = nb - i_bits
            f.write(f" Para hardware de {nb} bits -> Formato Global Sugerido: Q{i_bits}.{f_bits}\n")

    plt.figure(figsize=(10, 6))
    data_to_plot = []
    labels = []
    
    for nb in PRECISIONS:
        mses = global_all_mses_per_prec[nb]
        valid_mses = [m for m in mses if m < 1e3]
        data_to_plot.append(valid_mses)
        labels.append(f"{nb} Bits")

    plt.boxplot(data_to_plot, tick_labels=labels, patch_artist=True, boxprops=dict(facecolor="lightblue", alpha=0.7))
    plt.yscale('log')
    plt.title("Dispersión de Error (MSE) por Precisión (Todas las funciones)")
    plt.xlabel("Ancho de Palabra (Bits Totales)")
    plt.ylabel("Distribución de MSE (Escala Log)")
    plt.grid(True, which="both", ls=":", alpha=0.5)
    plt.tight_layout()
    plt.savefig(os.path.join("output", "MSE_Dispersion_Global.png"))
    plt.close()

if __name__ == "__main__":
    main()