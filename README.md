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

## Algoritmos de multiplicación (`src/multiplicacion.h`)

| Función | Devuelve | Notas |
| --- | --- | --- |
| `multiplicacionClasica(A, B)` | A · B | Método del colegio. Θ(n²). Sirve de referencia para validar Karatsuba. |
| `karatsuba(A, B, umbral = 32)` | A · B | Recursivo con tres productos. Θ(n^log₂3). Si los operandos tienen `umbral` dígitos o menos, usa la clásica; con `umbral = 1` parte hasta un dígito. |

`karatsuba` rellena los operandos a potencia de 2 una sola vez, al entrar. Dentro de la recursión solo iguala longitudes, porque x₁ + x₀ puede tener m + 1 dígitos y rellenarlo otra vez lo llevaría al tamaño del problema padre. El umbral de 32 dígitos se eligió midiendo (`resultados/umbral.tsv`).

En Karatsuba, el término central se calcula como `restar(restar(P3, P1), P2)`, en ese orden, para que ninguna resta dé un resultado negativo (P3 − P1 = P2 + x₁y₀ + x₀y₁ ≥ P2).

## Requisitos

- Un compilador de C++17: g++ 7 o superior, o clang. Se probó con g++ 10.3 (TDM-GCC-64) en Windows 11, con Apple clang 21 en macOS y con g++ 13.3 en Linux.
- `make` en Linux/macOS, o `mingw32-make` (incluido en MinGW / TDM-GCC) en Windows.
- Solo para las gráficas: Python 3 con `matplotlib`.

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
g++ -std=c++17 -O2 -Wall -Wextra -o test_correctitud tests/test_correctitud.cpp src/aritmetica.cpp src/multiplicacion.cpp
g++ -std=c++17 -O2 -Wall -Wextra -o benchmark bench/benchmark.cpp src/aritmetica.cpp src/multiplicacion.cpp
./test_aritmetica          # en Windows: test_aritmetica.exe
./test_correctitud
./benchmark resultados/tiempos.csv
```

Si todo funciona, las pruebas imprimen `OK (N verificaciones)`. Si algo falla, imprimen cada caso que falló y terminan con un código de salida distinto de 0.

> No compile con `-DNDEBUG`: desactivaría el `assert` que protege la precondición de `restar`.

## Medición y gráficas

```
make bench                    # compila y corre bench/benchmark.cpp -> resultados/tiempos.csv (~40 s)
python3 scripts/graficar.py   # genera resultados/grafica_*.png y resultados/exponentes.txt (requiere matplotlib)
```

Protocolo: `std::chrono::steady_clock`, repeticiones hasta superar 200 ms por ensayo, mediana de 5 ensayos, resultado consumido (suma de dígitos en un `volatile`) y verificación de que clásica y Karatsuba coinciden. Se compara la clásica contra Karatsuba con umbral 1 (pura) y umbral 32 (por defecto). El informe está en `informe/informe.pdf` (fuente: `informe/informe.md`).

## Estructura del repositorio

```
src/
  aritmetica.h            declaraciones y contratos (pre/postcondiciones, costo)
  aritmetica.cpp          implementación de la base aritmética
  multiplicacion.h        interfaz de multiplicacionClasica y karatsuba
  multiplicacion.cpp      implementación de los dos algoritmos
tests/
  test_aritmetica.cpp     pruebas unitarias de la base aritmética
  test_correctitud.cpp    Karatsuba contra la clásica (casos de borde y aleatorios)
bench/
  benchmark.cpp           medición de tiempos con steady_clock
resultados/
  tiempos.csv             tiempos de la clásica y de Karatsuba (umbral 1 y 32)
  exponentes.txt          pendientes log-log y razones al duplicar n
  grafica_tiempos.png     tiempo contra tamaño (log-log)
  grafica_razon.png       razón clásica / Karatsuba
  umbral.tsv              medición del umbral del caso base
scripts/
  graficar.py             genera las gráficas y exponentes.txt
informe/
  informe.pdf             informe de la práctica
  informe.md              fuente del informe
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

## Pruebas de correctitud

`tests/test_correctitud.cpp` multiplica cada caso con la clásica, con Karatsuba (umbral 32) y con Karatsuba puro (umbral 1), y exige que los tres coincidan. La clásica se verifica además contra `unsigned long long` en números de hasta 9 dígitos y contra la fórmula exacta de (10ⁿ − 1)² en números grandes. Cubre:

- los 100 pares de un dígito, y multiplicar por 0 y por 1;
- operandos con muchos ceros a la izquierda y factores con ceros intercalados (`100200300 × 4000005`);
- longitudes impares y distintas antes del relleno;
- productos grandes, hasta 4096 × 4096 dígitos;
- 3 000 pares aleatorios de 1 a 300 dígitos.
