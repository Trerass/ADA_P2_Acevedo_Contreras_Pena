# Sección B — Recurrencia de Karatsuba y por qué el exponente es log₂3

> Borrador (rol B). Las cifras experimentales salen de `resultados/umbral.tsv`
> (Apple M4, Apple clang 21, `-std=c++17 -O2`, `steady_clock`, repeticiones
> hasta superar 200 ms por medición).

## 1. Planteamiento de la recurrencia

Sea $T(n)$ el costo de `karatsuba` sobre dos operandos de $n$ dígitos, con $n$
potencia de 2 (los operandos se rellenan con ceros a la izquierda una sola vez,
al entrar). Cada llamada hace:

- **Trabajo recursivo:** tres productos de mitades,
  $P_1 = x_1 y_1$, $P_2 = x_0 y_0$ y $P_3 = (x_1 + x_0)(y_1 + y_0)$. Cada uno
  recibe operandos de $n/2$ dígitos (los de $P_3$ pueden tener $n/2 + 1$; ver
  la sección 5).
- **Trabajo no recursivo, lineal:** dos llamadas a `partir`, dos `sumar` para
  formar $x_1 + x_0$ y $y_1 + y_0$, dos `restar` para el término central
  $(P_3 - P_1) - P_2$, dos `desplazar` y dos `sumar` para recomponer. Son
  diez operaciones, cada una $\Theta(n)$ porque recorre los dígitos una vez
  con trabajo constante por posición. En total, $cn$ para alguna constante $c$.

Por lo tanto:

$$
T(n) = 3\,T\!\left(\frac{n}{2}\right) + cn, \qquad T(1) = c_0 .
$$

Como comparación, el divide y vencerás ingenuo (los cuatro productos
$x_1y_1$, $x_1y_0$, $x_0y_1$, $x_0y_0$) da $T(n) = 4T(n/2) + cn$.

## 2. Solución por el Teorema Maestro

Para $T(n) = a\,T(n/b) + f(n)$ se compara $f(n)$ con $n^{\log_b a}$.

| | Karatsuba | DyV ingenuo (4 productos) |
|---|---|---|
| $a$ (subproblemas) | 3 | 4 |
| $b$ (factor de reducción) | 2 | 2 |
| $f(n)$ | $\Theta(n)$ | $\Theta(n)$ |
| $n^{\log_b a}$ | $n^{\log_2 3} \approx n^{1.585}$ | $n^{\log_2 4} = n^2$ |

**Caso que aplica.** Para Karatsuba, $f(n) = n = n^{\log_2 3 - \varepsilon}$
con $\varepsilon = \log_2 3 - 1 \approx 0.585 > 0$, es decir,
$f(n) = O(n^{\log_b a - \varepsilon})$: el trabajo lineal de cada nodo crece
polinómicamente más despacio que el número de hojas. Es el **caso 1**, y la
solución la dominan las hojas:

$$
T(n) = \Theta\!\left(n^{\log_2 3}\right) \approx \Theta\!\left(n^{1.585}\right).
$$

El DyV ingenuo también cae en el caso 1, pero con $n^{\log_2 4} = n^2$: dividir
por sí solo no mejora el método del colegio. La ganancia viene únicamente de
bajar $a$ de 4 a 3.

## 3. Solución por el árbol de recurrencia

El árbol muestra de dónde sale el costo nivel por nivel.

| Nivel $i$ | Nodos | Tamaño de cada nodo | Costo por nodo | Costo del nivel |
|---|---|---|---|---|
| 0 | 1 | $n$ | $cn$ | $cn$ |
| 1 | 3 | $n/2$ | $cn/2$ | $\tfrac{3}{2}\,cn$ |
| 2 | 9 | $n/4$ | $cn/4$ | $\left(\tfrac{3}{2}\right)^2 cn$ |
| $i$ | $3^i$ | $n/2^i$ | $cn/2^i$ | $\left(\tfrac{3}{2}\right)^i cn$ |
| $h = \log_2 n$ (hojas) | $3^{\log_2 n}$ | 1 | $c_0$ | $c_0\, 3^{\log_2 n}$ |

- **Altura:** el tamaño se divide entre 2 en cada nivel hasta llegar a 1, así
  que $h = \log_2 n$.
- **Número de hojas:** $3^{\log_2 n} = 2^{\log_2 3 \cdot \log_2 n} = n^{\log_2 3}$.
- **Suma de los niveles internos** (serie geométrica de razón $3/2 > 1$):

$$
\sum_{i=0}^{h-1} cn\left(\tfrac{3}{2}\right)^i
= cn\,\frac{(3/2)^{h} - 1}{3/2 - 1}
= 2cn\left(\frac{n^{\log_2 3}}{n} - 1\right)
= 2c\left(n^{\log_2 3} - n\right),
$$

usando $(3/2)^{\log_2 n} = 3^{\log_2 n} / 2^{\log_2 n} = n^{\log_2 3}/n$.

- **Total:**

$$
T(n) = 2c\left(n^{\log_2 3} - n\right) + c_0\, n^{\log_2 3}
= (2c + c_0)\,n^{\log_2 3} - 2cn = \Theta\!\left(n^{\log_2 3}\right).
$$

**Qué aporta el árbol que no da el Teorema Maestro.** El costo por nivel
*crece* con razón $3/2$: cada nivel triplica los nodos pero solo divide a la
mitad su tamaño. Una serie geométrica creciente queda dominada por su último
término, el de las hojas, y eso es justo lo que el caso 1 afirma sin mostrarlo.
Además, el árbol da la constante: como la razón es $3/2$, la suma de todos los
niveles internos es a lo sumo 3 veces el último de ellos
($1 + \tfrac{2}{3} + \tfrac{4}{9} + \dots \le 3$).

## 4. Por qué el exponente es log₂3

El exponente es $\log_b a$: el logaritmo del número de subproblemas en la base
del factor en que se reduce el tamaño. Al duplicar $n$ se agrega un nivel al
árbol, y ese nivel triplica las hojas. Por eso:

$$
\frac{T(2n)}{T(n)} \to 3 = 2^{\log_2 3}
\quad\Longleftrightarrow\quad T(n) = \Theta(n^{\log_2 3}).
$$

Con cuatro productos la razón sería $4 = 2^2$, de ahí $n^2$. Un solo producto
ahorrado por nivel (3 en vez de 4), repetido en los $\log_2 n$ niveles, baja el
exponente de 2 a $\log_2 3 \approx 1.585$.

**Comprobación experimental.** La tabla da la razón de tiempos al duplicar $n$
(datos de `resultados/umbral.tsv`):

| $n \to 2n$ | Clásica | Karatsuba (umbral 1) | Karatsuba (umbral 32) |
|---|---|---|---|
| 256 → 512 | 4.29 | 3.02 | 3.09 |
| 512 → 1024 | 3.82 | 2.98 | 3.01 |
| 1024 → 2048 | 4.05 | 3.03 | 3.05 |
| 2048 → 4096 | 4.04 | 3.16 | 3.06 |

La clásica se acerca a $4 = 2^2$ y Karatsuba a $3 = 2^{\log_2 3}$, como predice
el análisis. Con 1024 → 2048, el exponente estimado es
$\log_2 3.05 \approx 1.61$, contra el teórico $1.585$.

## 5. Detalles de implementación que no cambian el orden

**$P_3$ puede recibir $n/2 + 1$ dígitos.** $x_1 + x_0$ puede generar un acarreo
y tener un dígito más. La recurrencia real es
$T(n) \le 2T(n/2) + T(n/2 + 1) + cn$, que tiene la misma solución
$\Theta(n^{\log_2 3})$: un sumando constante en el argumento no cambia el orden
(el mismo argumento que usa CLRS para pisos y techos).

Por eso el relleno a potencia de 2 se hace **una sola vez**, al entrar. Dentro
de la recursión solo se igualan las longitudes. Si se rellenara otra vez a
potencia de 2 en cada llamada, el operando de $m + 1$ dígitos saltaría a
$2m = n$ dígitos: $P_3$ tendría el mismo tamaño que el problema padre y se
perdería la cota.

**Umbral del caso base.** La implementación usa la multiplicación clásica
cuando $n \le n_0$, con $n_0 = 32$. El árbol se corta a altura
$\log_2(n/n_0)$, y entonces tiene $(n/n_0)^{\log_2 3}$ hojas de costo
$\Theta(n_0^2)$ cada una:

$$
\text{costo de las hojas} = \Theta\!\left(n_0^{2}\left(\frac{n}{n_0}\right)^{\log_2 3}\right)
= \Theta\!\left(n_0^{\,2 - \log_2 3}\, n^{\log_2 3}\right).
$$

Como $n_0$ es una constante, el orden sigue siendo $\Theta(n^{\log_2 3})$: el
umbral solo cambia la constante. La tabla de la sección 4 lo confirma, porque
con umbral 1 y con umbral 32 la razón es ≈ 3.

Tiempo promedio por multiplicación, en microsegundos:

| $n$ | Clásica | $n_0=1$ | $n_0=8$ | $n_0=16$ | $n_0=32$ | $n_0=64$ | $n_0=256$ |
|---|---|---|---|---|---|---|---|
| 32 | 1.7 | 114.1 | 5.7 | 2.4 | **1.7** | 1.7 | 1.6 |
| 64 | 6.3 | 336.0 | 18.2 | 8.4 | **6.2** | 6.3 | 6.4 |
| 128 | 28.4 | 957.8 | 63.3 | 29.8 | **21.2** | 20.9 | 28.7 |
| 512 | 521.2 | 8741.2 | 549.6 | 286.5 | **206.5** | 201.6 | 338.8 |
| 1024 | 1989.5 | 26084.6 | 1598.2 | 828.4 | **622.1** | 612.4 | 1038.2 |
| 4096 | 32568.4 | 249868.4 | 14814.6 | 7780.2 | **5791.1** | 5587.5 | 9047.1 |

**Justificación de $n_0 = 32$.** Conviene dividir mientras un nivel de Karatsuba
cueste menos que la clásica directa:

- Con $n = 32$ gana la clásica: 1.7 µs, contra 2.4 µs de un nivel de Karatsuba
  sobre mitades de 16.
- Con $n = 64$ empatan: 6.3 µs contra 6.2 µs.
- Con $n_0 = 64$ los tiempos quedan a menos de 4 % de los de $n_0 = 32$, dentro
  del ruido. Se elige el menor valor del rango óptimo.
- Con umbrales muy grandes ($n_0 = 256$) el tiempo vuelve a subir, porque se
  usa la clásica $\Theta(n^2)$ en tamaños donde ya no conviene.

Con $n_0 = 1$, Karatsuba es 43 veces más lenta que con $n_0 = 32$ en
$n = 4096$, y nunca le gana a la clásica en el rango medido. En los nodos
pequeños el costo lo dominan las copias y las asignaciones de memoria de
`std::vector`, no las operaciones con dígitos. Son las constantes ocultas que
el análisis asintótico ignora. Con $n_0 = 32$, Karatsuba supera a la clásica
desde $n = 128$ (21.2 µs contra 28.4 µs) y es 5.6 veces más rápida en
$n = 4096$.
