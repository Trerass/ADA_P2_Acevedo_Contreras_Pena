// multiplicacion.cpp
// Implementación de la multiplicación clásica y de Karatsuba.

#include "multiplicacion.h"

#include <cassert>

Numero multiplicacionClasica(const Numero& A, const Numero& B) {
    Numero r(A.size() + B.size(), 0);
    for (std::size_t i = 0; i < A.size(); i++) {
        int acarreo = 0;
        for (std::size_t j = 0; j < B.size(); j++) {
            int t = r[i + j] + A[i] * B[j] + acarreo;  // máximo 99
            r[i + j] = t % 10;
            acarreo = t / 10;
        }
        r[i + B.size()] += acarreo;
    }
    normalizar(r);
    return r;
}

// Parte recursiva. No se rellena a potencia de 2 aquí: si se hiciera,
// x1 + x0 (que puede tener un dígito más) duplicaría su tamaño y se
// perdería la ventaja de Karatsuba. Basta con igualar longitudes.
static Numero karatsubaRec(Numero x, Numero y, std::size_t umbral) {
    std::size_t n = x.size() > y.size() ? x.size() : y.size();
    x.resize(n, 0);
    y.resize(n, 0);

    // Caso base: números pequeños se multiplican directo.
    if (n <= umbral) {
        return multiplicacionClasica(x, y);
    }

    // x = x1 * 10^m + x0,  y = y1 * 10^m + y0
    std::size_t m = n / 2;
    auto [x1, x0] = partir(x, m);
    auto [y1, y0] = partir(y, m);

    Numero P1 = karatsubaRec(x1, y1, umbral);
    Numero P2 = karatsubaRec(x0, y0, umbral);
    Numero P3 = karatsubaRec(sumar(x1, x0), sumar(y1, y0), umbral);

    // Término del medio = P3 - P1 - P2. Se resta P1 primero para que
    // nunca dé negativo.
    Numero centro = restar(restar(P3, P1), P2);

    // Resultado = P1 * 10^(2m) + centro * 10^m + P2
    int k = static_cast<int>(m);
    return sumar(sumar(desplazar(P1, 2 * k), desplazar(centro, k)), P2);
}

Numero karatsuba(const Numero& A, const Numero& B, std::size_t umbral) {
    assert(umbral >= 1);
    Numero x = A;
    Numero y = B;
    rellenarPotencia2(x, y);  // solo una vez, al inicio
    return karatsubaRec(x, y, umbral);
}
