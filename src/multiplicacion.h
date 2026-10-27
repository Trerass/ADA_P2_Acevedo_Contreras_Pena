// multiplicacion.h
// Multiplicación de enteros grandes no negativos en base 10: el método
// clásico Θ(n²) y Karatsuba Θ(n^log2(3)). Usa la representación de
// aritmetica.h (std::vector<int> little-endian).

#ifndef MULTIPLICACION_H
#define MULTIPLICACION_H

#include "aritmetica.h"

// Devuelve A * B con el método del colegio: cada dígito de A por cada
// dígito de B, acumulando en la posición i + j y propagando el acarreo.
// Sirve de referencia para validar karatsuba.
// Admite longitudes distintas y operandos sin normalizar.
// Post: resultado normalizado.
// Costo: Θ(|A| * |B|), es decir Θ(n²) si ambos tienen n dígitos.
Numero multiplicacionClasica(const Numero& A, const Numero& B);

// Devuelve A * B con Karatsuba recursivo (3 productos en lugar de 4):
//   x = x1 * 10^m + x0,  y = y1 * 10^m + y0
//   P1 = x1*y1,  P2 = x0*y0,  P3 = (x1 + x0)(y1 + y0)
//   x*y = P1 * 10^(2m) + (P3 - P1 - P2) * 10^m + P2
// Rellena A y B a la misma potencia de 2 una sola vez, al entrar. Caso base:
// si los operandos tienen umbral dígitos o menos, se usa
// multiplicacionClasica. Con umbral = 1 es el Karatsuba "puro" (caso base de
// 1 dígito); las pruebas lo usan para ejercitar la recursión en números
// pequeños.
// Admite longitudes distintas y operandos sin normalizar.
// Pre:  umbral >= 1.
// Post: resultado normalizado.
// Costo: T(n) = 3T(n/2) + Θ(n) = Θ(n^log2(3)) ≈ Θ(n^1.585); el umbral solo
// cambia la constante.
//
// UMBRAL_KARATSUBA = 32, medido en Apple M4, Apple clang 21, -O2
// (resultados/umbral.tsv): con 64 dígitos, la clásica directa (6.3 us) y un
// solo nivel de Karatsuba sobre mitades de 32 (6.2 us) empatan; por debajo
// gana la clásica (n = 32: 1.7 us contra 2.4 us). Con 32 y 64 los tiempos de
// n = 4096 quedan a menos de 4% entre sí, y son 43 veces más rápidos que con
// umbral 1.
const std::size_t UMBRAL_KARATSUBA = 32;
Numero karatsuba(const Numero& A, const Numero& B,
                 std::size_t umbral = UMBRAL_KARATSUBA);

#endif
