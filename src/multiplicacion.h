// multiplicacion.h
// Las dos formas de multiplicar números grandes: la clásica y Karatsuba.

#ifndef MULTIPLICACION_H
#define MULTIPLICACION_H

#include "aritmetica.h"

// Multiplicación del colegio: cada dígito de A por cada dígito de B.
// Costo Θ(n²). Sirve de referencia para comprobar Karatsuba.
Numero multiplicacionClasica(const Numero& A, const Numero& B);

// Por debajo de 32 dígitos la clásica es más rápida que seguir partiendo
// (medido en resultados/umbral.tsv).
const std::size_t UMBRAL_KARATSUBA = 32;

// Karatsuba: parte cada número en mitad alta y baja y usa 3 productos en
// vez de 4. Costo Θ(n^log2(3)) ≈ Θ(n^1.585).
// Con umbral = 1 parte hasta llegar a un solo dígito.
Numero karatsuba(const Numero& A, const Numero& B,
                 std::size_t umbral = UMBRAL_KARATSUBA);

#endif
