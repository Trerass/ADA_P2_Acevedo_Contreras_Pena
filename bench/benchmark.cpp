// benchmark.cpp
// Mide el tiempo de multiplicacionClasica y karatsuba para n = 2, 4, ..., 4096
// digitos y escribe un CSV.
//
// Protocolo:
//  - Reloj: std::chrono::steady_clock.
//  - Cada celda (algoritmo, n) se repite hasta acumular al menos MIN_MS
//    milisegundos; el tiempo reportado es total / repeticiones.
//  - Cada celda se mide NUM_ENSAYOS veces y se reporta la MEDIANA, para
//    reducir el efecto de picos del sistema operativo.
//  - Los operandos son aleatorios (semilla fija) de exactamente n digitos.
//  - El resultado se consume: se suman sus digitos en un acumulador volatile
//    para que el compilador no elimine el calculo. Ademas, para cada n se
//    verifica que los tres metodos den el mismo producto.
//
// Uso: benchmark [salida.csv]   (por defecto resultados/tiempos.csv)

#include "../src/aritmetica.h"
#include "../src/multiplicacion.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

static const double MIN_MS = 200.0;   // tiempo minimo por ensayo
static const int NUM_ENSAYOS = 5;     // se reporta la mediana
static const std::size_t N_MIN = 2;
static const std::size_t N_MAX = 4096;

static volatile long long sumideroGlobal = 0;  // consume los resultados

static Numero aleatorio(std::size_t n, std::mt19937_64& rng) {
    std::uniform_int_distribution<int> digito(0, 9);
    std::uniform_int_distribution<int> primero(1, 9);
    Numero A(n);
    for (std::size_t i = 0; i + 1 < n; i++) A[i] = digito(rng);
    A[n - 1] = primero(rng);  // digito mas significativo != 0
    return A;
}

static long long sumaDigitos(const Numero& R) {
    long long s = 0;
    for (int d : R) s += d;
    return s;
}

// Tiempo promedio (microsegundos) de una multiplicacion, para el algoritmo f.
template <typename F>
static double medir(F f, const Numero& A, const Numero& B) {
    using reloj = std::chrono::steady_clock;
    std::vector<double> ensayos;
    for (int e = 0; e < NUM_ENSAYOS; e++) {
        long long reps = 0;
        long long acum = 0;
        auto t0 = reloj::now();
        double ms = 0.0;
        do {
            Numero R = f(A, B);
            acum += sumaDigitos(R);
            reps++;
            ms = std::chrono::duration<double, std::milli>(reloj::now() - t0).count();
        } while (ms < MIN_MS);
        sumideroGlobal = sumideroGlobal + acum;
        ensayos.push_back(ms * 1000.0 / static_cast<double>(reps));
    }
    std::sort(ensayos.begin(), ensayos.end());
    return ensayos[ensayos.size() / 2];
}

int main(int argc, char** argv) {
    std::string salida = argc > 1 ? argv[1] : "resultados/tiempos.csv";
    std::ofstream out(salida);
    if (!out) {
        std::cerr << "No se pudo abrir " << salida << "\n";
        return 1;
    }
    out << "n,clasica_us,karatsuba_u1_us,karatsuba_u32_us,"
           "razon_u1,razon_u32,checksum\n";

    std::mt19937_64 rng(20261030);
    std::printf("%6s %12s %14s %14s %9s %9s\n", "n", "clasica(us)",
                "kara U=1(us)", "kara U=32(us)", "cl/U1", "cl/U32");

    for (std::size_t n = N_MIN; n <= N_MAX; n *= 2) {
        Numero A = aleatorio(n, rng);
        Numero B = aleatorio(n, rng);

        // Verificacion: los tres metodos deben coincidir.
        Numero rc = multiplicacionClasica(A, B);
        Numero r1 = karatsuba(A, B, 1);
        Numero r32 = karatsuba(A, B, 32);
        if (comparar(rc, r1) != 0 || comparar(rc, r32) != 0) {
            std::cerr << "ERROR: resultados distintos para n = " << n << "\n";
            return 1;
        }
        long long checksum = sumaDigitos(rc);

        double tc = medir([](const Numero& a, const Numero& b) {
            return multiplicacionClasica(a, b); }, A, B);
        double t1 = medir([](const Numero& a, const Numero& b) {
            return karatsuba(a, b, 1); }, A, B);
        double t32 = medir([](const Numero& a, const Numero& b) {
            return karatsuba(a, b, 32); }, A, B);

        out << n << "," << tc << "," << t1 << "," << t32 << ","
            << tc / t1 << "," << tc / t32 << "," << checksum << "\n";
        std::printf("%6zu %12.3f %14.3f %14.3f %9.3f %9.3f\n", n, tc, t1, t32,
                    tc / t1, tc / t32);
        std::fflush(stdout);
    }
    std::cout << "checksum global (evita eliminar el calculo): "
              << sumideroGlobal << "\n";
    return 0;
}
