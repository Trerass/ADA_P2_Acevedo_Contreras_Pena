// test_correctitud.cpp
// Comprueba que Karatsuba da lo mismo que la clásica, y que la clásica da
// el resultado correcto. Imprime "OK" o los casos que fallaron.

#include "../src/aritmetica.h"
#include "../src/multiplicacion.h"

#include <iostream>
#include <random>
#include <string>

static int fallos = 0;
static int verificaciones = 0;

// Como assert, pero no detiene el programa: anota el fallo y sigue.
#define VERIFICAR(cond, descripcion)                                         \
    do {                                                                     \
        verificaciones++;                                                    \
        if (!(cond)) {                                                       \
            fallos++;                                                        \
            std::cout << "FALLO: " << (descripcion) << "  [linea "          \
                      << __LINE__ << "]\n";                                  \
        }                                                                    \
    } while (0)

static std::mt19937_64 rng(20261028);  // semilla fija: siempre los mismos casos

// Cadena aleatoria de exactamente n dígitos, sin cero inicial.
static std::string cadenaAleatoria(std::size_t n) {
    std::uniform_int_distribution<int> digito(0, 9);
    std::uniform_int_distribution<int> primero(1, 9);
    std::string s(1, static_cast<char>('0' + primero(rng)));
    for (std::size_t i = 1; i < n; i++) {
        s.push_back(static_cast<char>('0' + digito(rng)));
    }
    return s;
}

// Multiplica a y b con los dos métodos y compara. Si se da "esperado",
// también lo compara con ese resultado.
static void verificarProducto(const std::string& a, const std::string& b,
                              const std::string& esperado = "") {
    Numero A = desdeString(a);
    Numero B = desdeString(b);
    std::string clasica = aString(multiplicacionClasica(A, B));
    std::string kara = aString(karatsuba(A, B));
    std::string pura = aString(karatsuba(A, B, 1));  // parte hasta 1 dígito
    std::string caso = a.substr(0, 20) + " x " + b.substr(0, 20);
    if (!esperado.empty()) {
        VERIFICAR(clasica == esperado, "clasica " + caso);
    }
    VERIFICAR(kara == clasica, "karatsuba == clasica en " + caso);
    VERIFICAR(pura == clasica, "karatsuba(umbral 1) == clasica en " + caso);
}

// Resultado correcto para números de hasta 9 dígitos (solo para probar).
static std::string productoNativo(const std::string& a, const std::string& b) {
    return std::to_string(std::stoull(a) * std::stoull(b));
}

// 99...9 al cuadrado da 99...980...01. Ejemplo: 999^2 = 998001.
static std::string cuadradoDeNueves(std::size_t n) {
    return std::string(n - 1, '9') + "8" + std::string(n - 1, '0') + "1";
}

static void probarUnDigito() {
    for (int a = 0; a <= 9; a++) {
        for (int b = 0; b <= 9; b++) {
            verificarProducto(std::to_string(a), std::to_string(b),
                              std::to_string(a * b));
        }
    }
}

static void probarCeroYUno() {
    std::string grande = cadenaAleatoria(257);
    verificarProducto(grande, "0", "0");
    verificarProducto("0", grande, "0");
    verificarProducto(grande, "1", grande);
    verificarProducto("1", grande, grande);
    verificarProducto("0", "0", "0");
}

static void probarCerosALaIzquierda() {
    verificarProducto("0000000123", "0045", "5535");
    verificarProducto("000", "0000007", "0");
    verificarProducto("00000000000000000000000000000001", "0009", "9");
    verificarProducto("0" + std::string(40, '0') + "99", "000099", "9801");
}

static void probarCerosIntercalados() {
    verificarProducto("100200300", "4000005", "400801701001500");
    verificarProducto("1000000001", "1000000001", "1000000002000000001");
    verificarProducto("10101010", "1010101", productoNativo("10101010", "1010101"));
    verificarProducto("90000009", "70000007", productoNativo("90000009", "70000007"));
}

static void probarLongitudes() {
    // Longitudes impares y distintas entre sí.
    for (std::size_t la = 1; la <= 9; la++) {
        for (std::size_t lb = 1; lb <= 9; lb++) {
            std::string a = cadenaAleatoria(la);
            std::string b = cadenaAleatoria(lb);
            verificarProducto(a, b, productoNativo(a, b));
        }
    }
    verificarProducto(cadenaAleatoria(333), cadenaAleatoria(7));
    verificarProducto(cadenaAleatoria(1), cadenaAleatoria(513));
}

static void probarGrandes() {
    // Puros nueves: obligan a llevar acarreo en todas las sumas.
    for (std::size_t n : {2u, 3u, 64u, 255u, 500u, 1024u}) {
        std::string nueves(n, '9');
        verificarProducto(nueves, nueves, cuadradoDeNueves(n));
    }
    verificarProducto(cadenaAleatoria(700), cadenaAleatoria(650));
    verificarProducto(cadenaAleatoria(4096), cadenaAleatoria(4096));
}

static void probarAleatoriasMasivas() {
    std::uniform_int_distribution<std::size_t> longitud(1, 300);
    for (int i = 0; i < 3000; i++) {
        verificarProducto(cadenaAleatoria(longitud(rng)),
                          cadenaAleatoria(longitud(rng)));
    }
}

int main() {
    probarUnDigito();
    probarCeroYUno();
    probarCerosALaIzquierda();
    probarCerosIntercalados();
    probarLongitudes();
    probarGrandes();
    probarAleatoriasMasivas();

    if (fallos == 0) {
        std::cout << "OK (" << verificaciones << " verificaciones)\n";
        return 0;
    }
    std::cout << fallos << " de " << verificaciones << " verificaciones fallaron\n";
    return 1;
}
