// test_aritmetica.cpp
// Pruebas unitarias de la base aritmética, sin frameworks externos.
// Cada verificación que falla imprime su descripción; al final se imprime
// "OK" o la cantidad de fallos, y el código de salida es distinto de 0 si
// algo falló.

#include "../src/aritmetica.h"

#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

static int fallos = 0;
static int verificaciones = 0;

// Como assert, pero no aborta: registra el fallo, dice cuál fue y sigue.
#define VERIFICAR(cond, descripcion)                                         \
    do {                                                                     \
        verificaciones++;                                                    \
        if (!(cond)) {                                                       \
            fallos++;                                                        \
            std::cout << "FALLO: " << (descripcion) << "  [linea "          \
                      << __LINE__ << "]\n";                                  \
        }                                                                    \
    } while (0)

static std::mt19937_64 rng(20261027);  // semilla fija: pruebas reproducibles

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

// Suma de referencia sobre cadenas big-endian (el dígito más significativo
// primero), escrita aparte de sumar() y con otra representación, para
// verificar la suma de 300 dígitos con un cálculo independiente.
static std::string sumaColumnas(const std::string& a, const std::string& b) {
    std::string r;
    int i = static_cast<int>(a.size()) - 1;
    int j = static_cast<int>(b.size()) - 1;
    int acarreo = 0;
    while (i >= 0 || j >= 0 || acarreo > 0) {
        int d = acarreo;
        if (i >= 0) d += a[i--] - '0';
        if (j >= 0) d += b[j--] - '0';
        r.insert(r.begin(), static_cast<char>('0' + d % 10));
        acarreo = d / 10;
    }
    return r;
}

static void probarConversiones() {
    VERIFICAR(aString(desdeString("0")) == "0", "\"0\" ida y vuelta");
    VERIFICAR(aString(desdeString("000")) == "0", "\"000\" -> \"0\"");
    VERIFICAR((desdeString("000") == Numero{0}), "\"000\" se guarda como {0}");
    VERIFICAR(aString(desdeString("7")) == "7", "\"7\" ida y vuelta");
    VERIFICAR(aString(desdeString("0012300")) == "12300", "\"0012300\" -> \"12300\"");
    VERIFICAR((desdeString("1234") == Numero{4, 3, 2, 1}), "1234 es {4,3,2,1} (little-endian)");

    std::string grande = cadenaAleatoria(500);
    VERIFICAR(aString(desdeString(grande)) == grande, "numero de 500 digitos ida y vuelta");

    VERIFICAR(aString(Numero{5, 0, 0}) == "5", "aString ignora ceros a la izquierda sin normalizar");

    bool lanzo = false;
    try {
        desdeString("12a3");
    } catch (const std::invalid_argument&) {
        lanzo = true;
    }
    VERIFICAR(lanzo, "\"12a3\" lanza invalid_argument");

    lanzo = false;
    try {
        desdeString("");
    } catch (const std::invalid_argument&) {
        lanzo = true;
    }
    VERIFICAR(lanzo, "cadena vacia lanza invalid_argument");

    lanzo = false;
    try {
        desdeString("-5");
    } catch (const std::invalid_argument&) {
        lanzo = true;
    }
    VERIFICAR(lanzo, "\"-5\" lanza invalid_argument (no hay signo)");
}

static void probarNormalizar() {
    Numero a{0, 0, 0};
    normalizar(a);
    VERIFICAR((a == Numero{0}), "normalizar {0,0,0} -> {0}");

    Numero b;
    normalizar(b);
    VERIFICAR((b == Numero{0}), "normalizar {} -> {0}");

    Numero c{1, 2, 0, 0};
    normalizar(c);
    VERIFICAR((c == Numero{1, 2}), "normalizar {1,2,0,0} -> {1,2}");

    Numero d{1, 2};
    normalizar(d);
    VERIFICAR((d == Numero{1, 2}), "normalizar no toca un numero ya normalizado");
}

static void probarComparar() {
    VERIFICAR(comparar(Numero{5, 0, 0}, Numero{5}) == 0, "{5,0,0} == {5}");
    VERIFICAR(comparar(Numero{0, 0}, Numero{0}) == 0, "{0,0} == {0}");
    VERIFICAR(comparar(desdeString("124"), desdeString("123")) == 1, "124 > 123");
    VERIFICAR(comparar(desdeString("123"), desdeString("124")) == -1, "123 < 124");
    VERIFICAR(comparar(desdeString("1000"), desdeString("999")) == 1, "1000 > 999 (mas largo)");
    VERIFICAR(comparar(desdeString("99"), desdeString("100")) == -1, "99 < 100 (mas corto)");
    VERIFICAR(comparar(Numero{9, 0, 0, 0}, Numero{0, 1}) == -1, "{9,0,0,0} < {0,1}: 9 < 10 aunque sea mas largo");
}

static void probarSumar() {
    VERIFICAR(aString(sumar(desdeString("0"), desdeString("0"))) == "0", "0 + 0 = 0");
    VERIFICAR(aString(sumar(desdeString("999"), desdeString("1"))) == "1000", "999 + 1 = 1000");
    VERIFICAR(aString(sumar(desdeString("123456"), desdeString("78"))) == "123534", "123456 + 78");
    VERIFICAR(aString(sumar(desdeString("78"), desdeString("123456"))) == "123534", "78 + 123456");
    VERIFICAR(aString(sumar(Numero{5, 2, 0, 0, 0}, Numero{5, 0, 0})) == "30", "25 + 5 con ceros a la izquierda");
    VERIFICAR((sumar(Numero{0, 0}, Numero{0}) == Numero{0}), "{0,0} + {0} se normaliza a {0}");

    std::string a = cadenaAleatoria(300);
    std::string b = cadenaAleatoria(300);
    VERIFICAR(aString(sumar(desdeString(a), desdeString(b))) == sumaColumnas(a, b),
              "suma de dos numeros de 300 digitos contra suma por columnas");

    std::string nueves(300, '9');
    VERIFICAR(aString(sumar(desdeString(nueves), desdeString("1"))) == "1" + std::string(300, '0'),
              "999...9 (300) + 1 = 1000...0 (acarreo en cadena)");
}

static void probarRestar() {
    VERIFICAR(aString(restar(desdeString("1000"), desdeString("1"))) == "999", "1000 - 1 = 999");
    VERIFICAR(aString(restar(desdeString("1000"), desdeString("1000"))) == "0", "1000 - 1000 = 0");
    VERIFICAR((restar(desdeString("1000"), desdeString("1000")) == Numero{0}), "1000 - 1000 se guarda como {0}");
    VERIFICAR(aString(restar(desdeString("500"), desdeString("499"))) == "1", "500 - 499 = 1");
    VERIFICAR(aString(restar(desdeString("123456"), desdeString("78"))) == "123378", "123456 - 78");
    VERIFICAR(aString(restar(desdeString("100000"), desdeString("99999"))) == "1", "100000 - 99999 (prestamos encadenados)");
    VERIFICAR(aString(restar(Numero{5}, Numero{3, 0, 0})) == "2", "{5} - {3,0,0}: B mas largo por ceros a la izquierda");
    VERIFICAR(aString(restar(Numero{0, 0, 1, 0}, Numero{1})) == "99", "100 - 1 con A sin normalizar");
}

static void probarDesplazar() {
    VERIFICAR(aString(desplazar(desdeString("123"), 3)) == "123000", "123 * 10^3");
    VERIFICAR((desplazar(desdeString("123"), 3) == Numero{0, 0, 0, 3, 2, 1}), "los ceros van al inicio del vector");
    VERIFICAR((desplazar(desdeString("0"), 5) == Numero{0}), "0 * 10^5 = {0}");
    VERIFICAR((desplazar(Numero{0, 0}, 5) == Numero{0}), "{0,0} * 10^5 = {0}");
    VERIFICAR((desplazar(desdeString("123"), 0) == Numero{3, 2, 1}), "k = 0 devuelve una copia");
    VERIFICAR((desplazar(Numero{5, 0}, 2) == Numero{0, 0, 5}), "{5,0} * 10^2 sale normalizado");
}

static void probarPartir() {
    auto p = partir(desdeString("12345678"), 4);
    VERIFICAR(aString(p.first) == "1234", "12345678, m=4: alta = 1234");
    VERIFICAR(aString(p.second) == "5678", "12345678, m=4: baja = 5678");

    auto q = partir(desdeString("10000005"), 4);
    VERIFICAR(aString(q.first) == "1000", "10000005, m=4: alta = 1000");
    VERIFICAR((q.second == Numero{5, 0, 0, 0}), "10000005, m=4: baja = 0005 con longitud 4");

    auto r = partir(desdeString("123"), 4);
    VERIFICAR((r.first == Numero{0}), "A.size() <= m: alta = {0}");
    VERIFICAR((r.second == Numero{3, 2, 1, 0}), "A.size() <= m: baja rellenada hasta m");

    auto s = partir(Numero{1, 2, 0, 0}, 2);
    VERIFICAR((s.first == Numero{0, 0}), "alta conserva sus ceros (no normaliza)");
    VERIFICAR((s.second == Numero{1, 2}), "baja = {1,2}");

    // Propiedad: A = alta * 10^m + baja.
    Numero a = desdeString(cadenaAleatoria(200));
    auto t = partir(a, 64);
    VERIFICAR(comparar(sumar(desplazar(t.first, 64), t.second), a) == 0,
              "A = alta * 10^m + baja (200 digitos, m = 64)");
}

static void probarRellenar() {
    Numero a(1, 7), b(1, 3);
    rellenarPotencia2(a, b);
    VERIFICAR(a.size() == 1 && b.size() == 1, "tamanos 1 y 1 -> 1");

    Numero c(3, 1), d(5, 1);
    rellenarPotencia2(c, d);
    VERIFICAR(c.size() == 8 && d.size() == 8, "tamanos 3 y 5 -> 8");
    VERIFICAR((c == Numero{1, 1, 1, 0, 0, 0, 0, 0}), "el relleno son ceros al final del vector");

    Numero e(8, 1), f(8, 1);
    rellenarPotencia2(e, f);
    VERIFICAR(e.size() == 8 && f.size() == 8, "tamanos 8 y 8 -> 8");

    Numero g(9, 1), h(2, 1);
    rellenarPotencia2(g, h);
    VERIFICAR(g.size() == 16 && h.size() == 16, "tamanos 9 y 2 -> 16");
    VERIFICAR(aString(h) == "11", "rellenar no cambia el valor del numero");
}

// ORÁCULO CON unsigned long long
// El enunciado prohíbe usar tipos nativos para el número completo en la
// implementación. Aquí se usan SOLO dentro del test, como referencia
// independiente para verificar sumar y restar. Con números de hasta 18
// dígitos (< 10^18), la suma es < 2 * 10^18, que cabe en unsigned long long
// (máximo ~1.8 * 10^19), así que el oráculo nunca desborda.
static void probarOraculoULL() {
    const unsigned long long limite = 1000000000000000000ULL;  // 10^18
    std::uniform_int_distribution<unsigned long long> dist(0, limite - 1);
    std::uniform_int_distribution<int> ceros(0, 3);
    int errores = 0;
    for (int i = 0; i < 20000; i++) {
        unsigned long long x = dist(rng);
        unsigned long long y = dist(rng);
        if (x < y) {
            std::swap(x, y);
        }
        // A veces se agregan ceros a la izquierda para probar entradas sin normalizar.
        Numero A = desdeString(std::to_string(x));
        Numero B = desdeString(std::to_string(y));
        A.resize(A.size() + ceros(rng), 0);
        B.resize(B.size() + ceros(rng), 0);

        if (aString(sumar(A, B)) != std::to_string(x + y)) errores++;
        if (aString(restar(A, B)) != std::to_string(x - y)) errores++;
        int esperado = x < y ? -1 : (x > y ? 1 : 0);
        if (comparar(A, B) != esperado) errores++;
    }
    VERIFICAR(errores == 0, "20000 casos aleatorios de sumar/restar/comparar contra unsigned long long");
}

// Propiedades con números grandes (fuera del alcance de cualquier tipo nativo).
static void probarPropiedadesGrandes() {
    std::uniform_int_distribution<int> largo(1, 600);
    int errores = 0;
    for (int i = 0; i < 500; i++) {
        Numero A = desdeString(cadenaAleatoria(largo(rng)));
        Numero B = desdeString(cadenaAleatoria(largo(rng)));
        Numero S = sumar(A, B);
        if (comparar(S, sumar(B, A)) != 0) errores++;   // conmutatividad
        if (comparar(restar(S, B), A) != 0) errores++;  // (A + B) - B = A
        if (comparar(restar(S, A), B) != 0) errores++;  // (A + B) - A = B
    }
    VERIFICAR(errores == 0, "500 casos grandes: A+B = B+A y (A+B)-B = A");
}

int main() {
    probarConversiones();
    probarNormalizar();
    probarComparar();
    probarSumar();
    probarRestar();
    probarDesplazar();
    probarPartir();
    probarRellenar();
    probarOraculoULL();
    probarPropiedadesGrandes();

    if (fallos == 0) {
        std::cout << "OK (" << verificaciones << " verificaciones)\n";
        return 0;
    }
    std::cout << fallos << " de " << verificaciones << " verificaciones fallaron\n";
    return 1;
}
