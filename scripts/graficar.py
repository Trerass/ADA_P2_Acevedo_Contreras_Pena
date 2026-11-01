#!/usr/bin/env python3
"""Genera las graficas del analisis experimental a partir de resultados/tiempos.csv.


"""
import csv
import math

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

filas = list(csv.DictReader(open("resultados/tiempos.csv")))
n = [int(f["n"]) for f in filas]
cl = [float(f["clasica_us"]) for f in filas]
k1 = [float(f["karatsuba_u1_us"]) for f in filas]
k32 = [float(f["karatsuba_u32_us"]) for f in filas]


def pendiente(xs, ys):
    """Pendiente de log2(y) contra log2(x) (minimos cuadrados)."""
    lx = [math.log2(x) for x in xs]
    ly = [math.log2(y) for y in ys]
    mx, my = sum(lx) / len(lx), sum(ly) / len(ly)
    return sum((a - mx) * (b - my) for a, b in zip(lx, ly)) / sum((a - mx) ** 2 for a in lx)


# Ajuste solo en n >= 256, donde ya domina el termino de mayor orden.
idx = [i for i, v in enumerate(n) if v >= 256]
sel = lambda v: [v[i] for i in idx]
with open("resultados/exponentes.txt", "w") as f:
    f.write("Pendiente log-log (ajuste con n >= 256)\n")
    f.write(f"clasica          : {pendiente(sel(n), sel(cl)):.3f}  (teorico 2)\n")
    f.write(f"karatsuba U=1    : {pendiente(sel(n), sel(k1)):.3f}  (teorico {math.log2(3):.3f})\n")
    f.write(f"karatsuba U=32   : {pendiente(sel(n), sel(k32)):.3f}  (teorico {math.log2(3):.3f})\n")
    f.write("\nRazon de tiempos al duplicar n\n")
    f.write("n->2n\tclasica\tU=1\tU=32\n")
    for i in range(len(n) - 1):
        f.write(f"{n[i]}->{n[i+1]}\t{cl[i+1]/cl[i]:.2f}\t{k1[i+1]/k1[i]:.2f}\t{k32[i+1]/k32[i]:.2f}\n")
print(open("resultados/exponentes.txt").read())

# Grafica 1: tiempo vs n, log-log, con referencias n^2 y n^1.585
fig, ax = plt.subplots(figsize=(7, 4.6))
ax.loglog(n, cl, "o-", label="Clásica")
ax.loglog(n, k1, "s-", label="Karatsuba (umbral 1)")
ax.loglog(n, k32, "^-", label="Karatsuba (umbral 32)")
ref = n.index(1024)
ax.loglog(n, [cl[ref] * (x / 1024) ** 2 for x in n], "k:", lw=1, label="referencia $n^2$")
ax.loglog(n, [k32[ref] * (x / 1024) ** math.log2(3) for x in n], "k--", lw=1,
          label="referencia $n^{\\log_2 3}$")
ax.set_xlabel("n (dígitos)")
ax.set_ylabel("tiempo por multiplicación (µs)")
ax.set_title("Tiempo de multiplicación vs tamaño (escala log-log)")
ax.set_xticks(n)
ax.set_xticklabels([str(v) for v in n], rotation=45, fontsize=8)
ax.grid(True, which="both", alpha=0.3)
ax.legend(fontsize=8)
fig.tight_layout()
fig.savefig("resultados/grafica_tiempos.png", dpi=170)

# Grafica 2: razon clasica / karatsuba (>1 => Karatsuba gana)
fig, ax = plt.subplots(figsize=(7, 3.8))
ax.semilogx(n, [a / b for a, b in zip(cl, k1)], "s-", label="vs Karatsuba (umbral 1)")
ax.semilogx(n, [a / b for a, b in zip(cl, k32)], "^-", label="vs Karatsuba (umbral 32)")
ax.axhline(1.0, color="k", lw=1)
ax.set_xlabel("n (dígitos)")
ax.set_ylabel("t_clásica / t_Karatsuba")
ax.set_title("Ventaja de Karatsuba (>1: Karatsuba es más rápida)")
ax.set_xticks(n)
ax.set_xticklabels([str(v) for v in n], rotation=45, fontsize=8)
ax.grid(True, which="both", alpha=0.3)
ax.legend(fontsize=8)
fig.tight_layout()
fig.savefig("resultados/grafica_razon.png", dpi=170)
