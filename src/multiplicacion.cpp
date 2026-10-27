// multiplicacion.cpp
// Implementación de multiplicacionClasica y karatsuba. Ver multiplicacion.h.

#include "multiplicacion.h"

#include <cassert>

Numero multiplicacionClasica(const Numero& A, const Numero& B) {
    Numero r(A.size() + B.size(), 0);
    for (std::size_t i = 0; i < A.size(); i++) {
        int acarreo = 0;
        for (std::size_t j = 0; j < B.size(); j++) {
            // r[i+j] <= 9, A[i]*B[j] <= 81 y acarreo <= 9: t <= 99 cabe en int
            int t = r[i + j] + A[i] * B[j] + acarreo;
            r[i + j] = t % 10;
            acarreo = t / 10;
        }
        r[i + B.size()] += acarreo;  // esa posición aún no se había tocado
    }
    normalizar(r);
    return r;
}

// Recursión de Karatsuba. x e y llegan por valor porque se igualan en
// longitud aquí mismo.
//
// Solo se rellena a potencia de 2 en karatsuba(); aquí basta igualar las
// longitudes. Si se volviera a rellenar a potencia de 2 en cada llamada,
// x1 + x0 con m + 1 dígitos saltaría a 2m = n dígitos: P3 tendría el mismo
// tamaño que el problema padre y el costo dejaría de ser Θ(n^log2(3)).
// Con longitudes iguales pero no potencia de 2, partir(x, n/2) deja la
// mitad alta con un dígito más cuando n es impar, lo cual es correcto.
static Numero karatsubaRec(Numero x, Numero y, std::size_t umbral) {
    std::size_t n = x.size() > y.size() ? x.size() : y.size();
    x.resize(n, 0);
    y.resize(n, 0);

    if (n <= umbral) {
        return multiplicacionClasica(x, y);
    }

    std::size_t m = n / 2;
    auto [x1, x0] = partir(x, m);
    auto [y1, y0] = partir(y, m);

    Numero P1 = karatsubaRec(x1, y1, umbral);
    Numero P2 = karatsubaRec(x0, y0, umbral);
    Numero P3 = karatsubaRec(sumar(x1, x0), sumar(y1, y0), umbral);

    // Primero P3 - P1 y después - P2: ninguna resta queda negativa.
    Numero centro = restar(restar(P3, P1), P2);

    int k = static_cast<int>(m);
    return sumar(sumar(desplazar(P1, 2 * k), desplazar(centro, k)), P2);
}

Numero karatsuba(const Numero& A, const Numero& B, std::size_t umbral) {
    assert(umbral >= 1);
    Numero x = A;
    Numero y = B;
    rellenarPotencia2(x, y);
    return karatsubaRec(x, y, umbral);
}
