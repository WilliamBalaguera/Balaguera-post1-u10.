# Balaguera-Post1-U10 — Benchmark de Jerarquía de Caché

**Asignatura:** Arquitectura de Computadores — Unidad 10: Memorias del Computador  
**Actividad:** Post-Contenido 1  


---

## Objetivo

Implementar y ejecutar un benchmark en C que mide la latencia de acceso a memoria para arrays de distintos tamaños, observando empíricamente el efecto de la jerarquía de caché (L1 → L2 → L3 → RAM) sobre el rendimiento, e interpretar los resultados en función de localidad espacial, líneas de caché y cache miss.

---

## Entorno de Compilación

| Elemento | Detalle |
|---|---|
| Sistema operativo | Ubuntu 20.04+ (o equivalente Linux) |
| Compilador | GCC 9.0 o superior |
| Flags de compilación | `-O0` (sin optimización — evita que el compilador elimine los accesos) |
| Herramienta de tiempo | `clock_gettime(CLOCK_MONOTONIC)` |

---

## Topología de Caché del Sistema (Checkpoint 1)

Detectada con:
```bash
for i in 0 1 2 3; do
  echo -n "Cache index$i: L$(cat /sys/devices/system/cpu/cpu0/cache/index${i}/level) "
  echo -n "$(cat /sys/devices/system/cpu/cpu0/cache/index${i}/type) = "
  cat /sys/devices/system/cpu/cpu0/cache/index${i}/size
done
```

| Nivel | Tipo | Tamaño |
|---|---|---|
| L1 | Data | 32 KB |
| L1 | Instruction | 32 KB |
| L2 | Unified | 1024 KB |
| L3 | Unified | 33792 KB (~33 MB) |

> Estos valores determinan los puntos de inflexión esperados en el benchmark.

---

## Pasos de Ejecución

```bash
# 1. Crear directorio de trabajo
mkdir -p ~/u10post1 && cd ~/u10post1

# 2. Compilar (sin optimización)
gcc -O0 -o cache_bench cache_bench.c

# 3. Ejecutar
./cache_bench
```

---

## Resultados (Checkpoint 2 y Checkpoint 3)

Salida del benchmark con `REPEAT=10`:

```
Tamano(KB)    seq(ns/byte)  rand(ns/elem)
--------------------------------------------------
4                    3.279          2.173
8                    2.917          2.160
16                   2.488          2.177
32                   2.654          4.597   ← salto aleatorio: sale de L1
64                   2.824          2.299
128                  2.842          2.361
256                  2.773          2.469
512                  2.734          2.734
1024                 2.626          3.887   ← latencia rand empieza a crecer
4096                 2.685          4.793
16384                2.696          8.862   ← fuera de L3: RAM
65536                2.710         20.344   ← TLB miss + cache miss masivos
```

---

## Análisis de Resultados

### Acceso Secuencial (`bench_seq`)

El acceso secuencial mantiene una latencia **casi constante** (~2.5–3.3 ns/byte) en todos los tamaños. Esto se debe a:

- **Localidad espacial:** los accesos consecutivos cargan líneas de caché completas (típicamente 64 bytes). Tras la primera carga, los siguientes 63 bytes tienen latencia casi nula.
- **Hardware prefetcher:** el procesador detecta el patrón lineal y pre-carga las líneas antes de que sean solicitadas, ocultando la latencia real de L2/L3/RAM.

El acceso secuencial **no revela** la jerarquía de caché porque el prefetcher enmascara los misses.

### Acceso Aleatorio (`bench_rand`)

El acceso aleatorio destruye la localidad espacial. Los resultados muestran tres saltos claros:

| Tamaño | Latencia rand | Causa |
|---|---|---|
| ≤ 16 KB | ~2.2 ns | Array en **L1**: cache hit constante |
| 32 KB | 4.6 ns | **Transición L1→L2**: empieza a haber L1 misses |
| 1–4 MB | ~4–5 ns | Array en **L3**: L2 misses frecuentes |
| 16 MB | ~9 ns | **Transición L3→RAM**: L3 misses, latencia de DRAM |
| 64 MB | ~20 ns | **RAM + TLB miss**: el TLB no puede mapear todas las páginas, penalización doble |

### ¿A qué nivel se atribuye cada salto?

1. **Salto en ~32 KB (2.2 → 4.6 ns/elem):** El array supera el L1d cache (32 KB). Cada acceso aleatorio ya no encuentra el dato en L1 y debe ir a L2 → **L1 cache miss**.

2. **Crecimiento gradual 128 KB–4 MB (2.4 → 4.8 ns/elem):** El array supera L2 (1 MB). Los accesos van a L3, que tiene mayor latencia → **L2 cache miss**.

3. **Salto en ~16 MB (4.8 → 8.9 ns/elem):** El array supera L3 (~33 MB comienza a notarse antes por asociatividad). Los accesos van directamente a DRAM → **L3 cache miss**.

4. **Salto en 64 MB (8.9 → 20.3 ns/elem):** Además del L3 miss, el array requiere miles de páginas virtuales que saturan el TLB. Cada acceso exige un **TLB miss** y un page-walk adicional, duplicando aproximadamente la penalización.

---

## Conclusiones

- El acceso **secuencial** es prácticamente inmune a la jerarquía de caché gracias al prefetcher y la localidad espacial.
- El acceso **aleatorio** expone brutalmente cada nivel de la jerarquía: la latencia se multiplica por ~9x al pasar de L1 a RAM.
- Diseñar estructuras de datos con **localidad espacial** (arrays sobre listas enlazadas, matrices row-major, etc.) es la optimización de rendimiento más impactante en sistemas modernos.
- El **TLB** es un cuello de botella adicional para conjuntos de trabajo grandes con acceso aleatorio.

