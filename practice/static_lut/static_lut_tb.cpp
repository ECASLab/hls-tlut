#include <iostream>
#include <cmath>
#include "static_lut.h"

int main() {
    int errores = 0;
    std::cout << "--- Iniciando Sweep Determinista ---" << std::endl;

    // Expandimos los límites de prueba para forzar la saturación en ambos extremos.
    float limite_inf = (float)RANGE_MIN - 2.0;
    float limite_sup = (float)RANGE_MAX + 2.0;

    // Si es simétrica, probamos también la parte negativa que no está en la RAM
    if (IS_SYMMETRIC) {
        limite_inf = -((float)RANGE_MAX + 2.0);
    }

    for (float f = limite_inf; f <= limite_sup; f += 0.5) {
        data_t x_test = (data_t)f;
        data_t y_hw;

        // Invocación del Hardware
        acelerador_nolineal(x_test, &y_hw);

        std::cout << "Entrada: " << x_test 
                  << "\t| Salida HW: " << y_hw << std::endl;
    }

    std::cout << "TEST FINALIZADO." << std::endl;
    return errores;
}