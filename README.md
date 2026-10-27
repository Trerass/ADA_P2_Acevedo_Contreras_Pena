# ADA_P2_Acevedo_Contreras_Pena

Práctica 2 de **Análisis y Diseño de Algoritmos**: multiplicación rápida de enteros con el algoritmo de Karatsuba (Divide y Vencerás, recurrencias y Teorema Maestro).

El proyecto implementa en C++ la multiplicación de enteros grandes por el método clásico, Θ(n²), y por el algoritmo de Karatsuba, Θ(n^log₂3) ≈ Θ(n^1.585). Los números se representan como arreglos de dígitos en base 10 y no se usa ninguna biblioteca externa de enteros grandes.

## Representación de los números

```cpp
using Numero = std::vector<int>;
```

- **Little-endian:** el índice 0 es el dígito **menos** significativo. `1234` se guarda como `{4,3,2,1}`.
- Cada elemento está en el rango `0..9`.
- El cero normalizado es `{0}`. Ninguna función devuelve un vector vacío.
- Los "ceros a la izquierda" del número son ceros al **final** del vector.
- No se usa `long long`, `__int128` ni otro tipo nativo para guardar o calcular el número completo. `int` solo se usa para operar dígitos individuales (un dígito más un acarreo o un préstamo).

Se eligió little-endian porque el acarreo de la suma y el préstamo de la resta avanzan en el mismo sentido que el índice (de `i` a `i + 1`), y porque multiplicar por 10^k equivale a insertar k ceros al inicio del vector.

**Convención de normalización:** toda función que devuelve un `Numero` lo devuelve normalizado, excepto `partir` y `rellenarPotencia2`, que conservan la longitud exacta para que la recursión de Karatsuba sea predecible. Todas las funciones aceptan operandos sin normalizar y de longitudes distintas.

## Interfaz de la base aritmética (`src/aritmetica.h`)

| Función | Devuelve | Notas |
| --- | --- | --- |
| `desdeString(s)` | `Numero` | `"001234"` → `{4,3,2,1}`. Lanza `std::invalid_argument` si la cadena está vacía o tiene un carácter que no es dígito. |
| `aString(A)` | `std::string` | Sin ceros a la izquierda. El cero se imprime `"0"`. |
| `normalizar(A)` | — | Quita los ceros del final del vector. `{}` pasa a `{0}`. |
| `comparar(A, B)` | `-1`, `0`, `1` | Ignora los ceros a la izquierda: `{5,0,0}` es igual a `{5}`. |
| `sumar(A, B)` | A + B | Puede tener un dígito más que el operando mayor. Θ(n). |
| `restar(A, B)` | A − B | Precondición A ≥ B, verificada con `assert`. Θ(n). |
| `desplazar(A, k)` | A · 10^k | Inserta k ceros al inicio del vector. Θ(n + k). |
| `partir(A, m)` | `{alta, baja}` | `baja` son los m dígitos menos significativos. No normaliza. Requiere m ≥ 1. |
| `rellenarPotencia2(A, B)` | — | Rellena ambos con ceros hasta la misma potencia de 2. |

En Karatsuba, el término central se calcula como `restar(restar(P3, P1), P2)`, en ese orden, para que ninguna resta dé un resultado negativo (P3 − P1 = P2 + x₁y₀ + x₀y₁ ≥ P2).

## Requisitos

- Un compilador de C++17: g++ 7 o superior. Se probó con g++ 10.3 (TDM-GCC-64) en Windows 11.
- `make` en Linux/macOS, o `mingw32-make` (incluido en MinGW / TDM-GCC) en Windows.

## Compilar y ejecutar

### Windows (MinGW / TDM-GCC, desde cmd o PowerShell)

```
mingw32-make          # compila
mingw32-make test     # compila y ejecuta las pruebas
mingw32-make clean    # borra los ejecutables
```

### Linux / macOS

```
make
make test
make clean
```

### Sin make

```
g++ -std=c++17 -O2 -Wall -Wextra -o test_aritmetica tests/test_aritmetica.cpp src/aritmetica.cpp
./test_aritmetica          # en Windows: test_aritmetica.exe
```

Si todo funciona, las pruebas imprimen `OK (N verificaciones)`. Si algo falla, imprimen cada caso que falló y terminan con un código de salida distinto de 0.

> No compile con `-DNDEBUG`: desactivaría el `assert` que protege la precondición de `restar`.

## Estructura del repositorio

```
src/
  aritmetica.h          declaraciones y contratos (pre/postcondiciones, costo)
  aritmetica.cpp        implementación de la base aritmética
tests/
  test_aritmetica.cpp   pruebas unitarias sin frameworks externos
bench/                  benchmark de tiempos
resultados/             datos de las mediciones (CSV)
scripts/                scripts auxiliares (gráficas)
informe/                informe de la práctica
Makefile
README.md
```

## Pruebas de la base aritmética

`tests/test_aritmetica.cpp` cubre:

- conversiones ida y vuelta, incluido un número de 500 dígitos, y entradas inválidas;
- normalización, comparación con ceros a la izquierda y con longitudes distintas;
- sumas y restas con acarreo y préstamos encadenados, operandos de longitudes distintas y sin normalizar, y una suma de 300 dígitos verificada contra una suma por columnas independiente;
- `desplazar`, `partir` (incluida la propiedad A = alta · 10^m + baja) y `rellenarPotencia2`;
- 20 000 casos aleatorios de hasta 18 dígitos comparados contra `unsigned long long`, que se usa **solo dentro del test** como oráculo de verificación;
- 500 casos aleatorios de hasta 600 dígitos que verifican A + B = B + A y (A + B) − B = A.
