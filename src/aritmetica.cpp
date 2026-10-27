// aritmetica.cpp
// Implementación de la base aritmética. Ver aritmetica.h para la
// representación (little-endian, dígitos 0..9) y los contratos.

#include "aritmetica.h"

#include <cassert>
#include <stdexcept>

// Cantidad de dígitos significativos de A (sin contar los ceros a la
// izquierda, que están al final del vector). El cero tiene longitud 1.
// Sirve para comparar y convertir sin copiar ni normalizar A.
static std::size_t longitudEfectiva(const Numero& A) {
    std::size_t n = A.size();
    while (n > 1 && A[n - 1] == 0) {
        n--;
    }
    return n;
}

static bool esCero(const Numero& A) {
    return longitudEfectiva(A) == 1 && A[0] == 0;
}

void normalizar(Numero& A) {
    while (A.size() > 1 && A.back() == 0) {
        A.pop_back();
    }
    if (A.empty()) {
        A.push_back(0);
    }
}

Numero desdeString(const std::string& s) {
    if (s.empty()) {
        throw std::invalid_argument("desdeString: cadena vacia");
    }
    Numero A;
    A.reserve(s.size());
    // El último carácter de la cadena es el dígito menos significativo,
    // así que se recorre la cadena de derecha a izquierda.
    for (std::size_t i = s.size(); i > 0; i--) {
        char c = s[i - 1];
        if (c < '0' || c > '9') {
            throw std::invalid_argument("desdeString: caracter no valido en \"" + s + "\"");
        }
        A.push_back(c - '0');
    }
    normalizar(A);
    return A;
}

std::string aString(const Numero& A) {
    assert(!A.empty());
    std::size_t n = longitudEfectiva(A);
    std::string s;
    s.reserve(n);
    // Del dígito más significativo (índice n-1) al menos significativo (0).
    for (std::size_t i = n; i > 0; i--) {
        s.push_back(static_cast<char>('0' + A[i - 1]));
    }
    return s;
}

int comparar(const Numero& A, const Numero& B) {
    std::size_t na = longitudEfectiva(A);
    std::size_t nb = longitudEfectiva(B);
    // Sin ceros a la izquierda, el número con más dígitos es el mayor.
    if (na != nb) {
        return na < nb ? -1 : 1;
    }
    // Misma longitud: decide el primer dígito distinto empezando por el más
    // significativo. Se usa i > 0 con i - 1 para no desbordar size_t.
    for (std::size_t i = na; i > 0; i--) {
        if (A[i - 1] != B[i - 1]) {
            return A[i - 1] < B[i - 1] ? -1 : 1;
        }
    }
    return 0;
}

Numero sumar(const Numero& A, const Numero& B) {
    std::size_t n = A.size() > B.size() ? A.size() : B.size();
    Numero R;
    R.reserve(n + 1);
    int acarreo = 0;
    // Como el índice 0 es el dígito menos significativo, el acarreo avanza
    // en el mismo sentido que el bucle: de i hacia i + 1.
    for (std::size_t i = 0; i < n; i++) {
        int a = i < A.size() ? A[i] : 0;   // el operando corto se completa con ceros
        int b = i < B.size() ? B[i] : 0;
        int d = a + b + acarreo;           // como máximo 9 + 9 + 1 = 19
        R.push_back(d % 10);
        acarreo = d / 10;                  // 0 o 1
    }
    if (acarreo > 0) {
        R.push_back(acarreo);              // el dígito extra: 999 + 1 = 1000
    }
    normalizar(R);
    return R;
}

Numero restar(const Numero& A, const Numero& B) {
    assert(comparar(A, B) >= 0 && "restar: se requiere A >= B");
    // B puede tener más posiciones que A si son ceros a la izquierda
    // (por ejemplo {5} - {3,0,0}), así que se recorre hasta el mayor tamaño.
    std::size_t n = A.size() > B.size() ? A.size() : B.size();
    Numero R;
    R.reserve(n);
    int prestamo = 0;
    for (std::size_t i = 0; i < n; i++) {
        int a = i < A.size() ? A[i] : 0;
        int b = i < B.size() ? B[i] : 0;
        int d = a - b - prestamo;          // entre 0 - 9 - 1 = -10 y 9
        if (d < 0) {
            d += 10;                       // se pide 10 a la posición siguiente
            prestamo = 1;
        } else {
            prestamo = 0;
        }
        R.push_back(d);
    }
    // Si A >= B, el último préstamo siempre queda saldado.
    assert(prestamo == 0);
    normalizar(R);
    return R;
}

Numero desplazar(const Numero& A, int k) {
    assert(k >= 0 && "desplazar: k debe ser no negativo");
    if (esCero(A)) {
        return Numero{0};
    }
    std::size_t n = longitudEfectiva(A);
    // k ceros en las posiciones menos significativas y luego los dígitos de A.
    Numero R(static_cast<std::size_t>(k), 0);
    R.reserve(static_cast<std::size_t>(k) + n);
    R.insert(R.end(), A.begin(), A.begin() + n);
    return R;
}

std::pair<Numero, Numero> partir(const Numero& A, std::size_t m) {
    assert(m >= 1 && "partir: m debe ser al menos 1");
    if (A.size() <= m) {
        Numero baja = A;
        baja.resize(m, 0);                 // relleno con ceros hasta m dígitos
        return {Numero{0}, baja};
    }
    // baja = A[0 .. m-1] (dígitos menos significativos), alta = A[m .. fin].
    Numero baja(A.begin(), A.begin() + m);
    Numero alta(A.begin() + m, A.end());
    return {alta, baja};
}

void rellenarPotencia2(Numero& A, Numero& B) {
    std::size_t mayor = A.size() > B.size() ? A.size() : B.size();
    std::size_t n = 1;
    while (n < mayor) {
        n *= 2;
    }
    A.resize(n, 0);                        // resize agrega ceros al final
    B.resize(n, 0);
}
