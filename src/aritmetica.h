// aritmetica.h
// Base aritmética para enteros grandes no negativos en base 10.
//
// REPRESENTACIÓN (decisión del grupo)
//   using Numero = std::vector<int>;
//   - Little-endian: el índice 0 es el dígito MENOS significativo.
//     Ejemplo: 1234 se guarda como {4,3,2,1}.
//   - Cada elemento está en el rango 0..9.
//   - El cero normalizado es {0}. Ninguna función devuelve un vector vacío.
//   - Los "ceros a la izquierda" del número son ceros al FINAL del vector.
//
// CONVENCIÓN DE NORMALIZACIÓN
//   Toda función que devuelve un Numero lo devuelve normalizado (sin ceros a
//   la izquierda), EXCEPTO partir y rellenarPotencia2, que conservan la
//   longitud exacta a propósito para que la recursión de Karatsuba sea
//   predecible.
//
//   Todas las funciones ACEPTAN operandos sin normalizar y de longitudes
//   distintas: en Karatsuba, x1 + x0 puede tener m + 1 dígitos.
//
// RESTRICCIÓN DEL ENUNCIADO
//   No se usa ningún tipo nativo (long long, __int128...) para guardar o
//   calcular el número completo; int solo se usa para operar dígitos
//   individuales (un dígito más un acarreo o un préstamo).

#ifndef ARITMETICA_H
#define ARITMETICA_H

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

using Numero = std::vector<int>;

// Convierte una cadena decimal en Numero. Ejemplo: "001234" -> {4,3,2,1}.
// Pre:  s no vacía y formada solo por caracteres '0'..'9'.
//       Si no se cumple, lanza std::invalid_argument.
// Post: resultado normalizado ("000" -> {0}).
// Costo: Θ(n), se recorre cada carácter una vez.
Numero desdeString(const std::string& s);

// Convierte un Numero en cadena decimal sin ceros a la izquierda.
// El cero se imprime "0". No modifica A.
// Pre:  A no vacío.
// Costo: Θ(n).
std::string aString(const Numero& A);

// Elimina los ceros del final del vector (ceros a la izquierda del número),
// dejando al menos un dígito. Un vector vacío pasa a ser {0}.
// Costo: O(n).
void normalizar(Numero& A);

// Compara A y B como números: devuelve -1 si A < B, 0 si A == B y 1 si A > B.
// Ignora los ceros a la izquierda: {5,0,0} es igual a {5}.
// Costo: O(n).
int comparar(const Numero& A, const Numero& B);

// Devuelve A + B. Admite longitudes distintas; el resultado puede tener un
// dígito más que el operando mayor (por el último acarreo).
// Costo: Θ(n), con n = max(|A|, |B|): un recorrido con trabajo constante
// (una suma, un % y un /) por posición.
Numero sumar(const Numero& A, const Numero& B);

// Devuelve A - B.
// Pre:  comparar(A, B) >= 0 (se verifica con assert). No hay aritmética con
//       signo: el resultado nunca es negativo.
//
// IMPORTANTE para Karatsuba: el término central P3 - P1 - P2 debe calcularse
// como restar(restar(P3, P1), P2), EN ESE ORDEN. Así ninguna resta tiene un
// resultado negativo:
//     P3 >= P1, porque P3 = P1 + x1*y0 + x0*y1 + P2, y
//     P3 - P1 = P2 + x1*y0 + x0*y1 >= P2.
// restar no sabe representar negativos, así que depende de que quien la
// llama respete este orden; si no, el assert de la precondición falla.
//
// Costo: Θ(n), un recorrido con trabajo constante por posición.
Numero restar(const Numero& A, const Numero& B);

// Devuelve A * 10^k: inserta k ceros al INICIO del vector, que en
// little-endian equivale a añadir k ceros a la derecha del número.
// Si A es cero devuelve {0} (no {0,0,0}). Si k == 0 devuelve una copia
// normalizada de A.
// Pre:  k >= 0 (se verifica con assert).
// Costo: Θ(n + k), se escribe cada dígito del resultado una vez.
Numero desplazar(const Numero& A, int k);

// Separa A respecto de la posición m y devuelve {alta, baja}:
//   baja = los primeros m elementos (los m dígitos menos significativos),
//   alta = el resto,
// de modo que A = alta * 10^m + baja.
// NO normaliza: baja siempre tiene exactamente m dígitos y alta conserva sus
// ceros a la izquierda, para que la recursión sea predecible.
// Si A.size() <= m: alta = {0} y baja = A rellenado con ceros hasta tamaño m.
// Pre:  m >= 1 (se verifica con assert), para que baja nunca quede vacía.
// Costo: Θ(max(|A|, m)).
std::pair<Numero, Numero> partir(const Numero& A, std::size_t m);

// Rellena A y B con ceros al final del vector (ceros a la izquierda del
// número) hasta la misma longitud n, donde n es la menor potencia de 2
// mayor o igual que max(A.size(), B.size()). Un dígito da n = 1.
// No normaliza después de rellenar.
// Karatsuba la llama UNA sola vez, al inicio. Dentro de la recursión solo
// iguala longitudes: x1 + x0 puede tener m + 1 dígitos, y rellenarlo a
// potencia de 2 lo llevaría a 2m = n, el mismo tamaño del padre, y se
// perdería el costo Θ(n^log2(3)).
// Costo: Θ(n).
void rellenarPotencia2(Numero& A, Numero& B);

#endif
