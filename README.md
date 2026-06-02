# Memory Access Benchmark — Cache Hierarchy Profiler

> Herramienta en C para medir y comparar la latencia de acceso **secuencial vs. aleatorio** a memoria, revelando los límites de L1, L2, L3 y RAM del sistema.

---

## 👤 Información del proyecto

| Campo           | Detalle                               |
|-----------------|---------------------------------------|
| **Autor**       | William Balaguera                     |
| **Materia**     | Arquitectura de computadores          |
| **Unidad**      | 10                                    |
| **Año**         | 2026                                  |
| **Universidad** | Francisco de Paula Santander          |
| **Lenguaje**    | C (estándar C99)                      |

---

## 📌 ¿Qué hace este proyecto?

Mide la **latencia real de acceso a memoria** en nanosegundos sobre arrays de distintos tamaños, cubriendo toda la jerarquía de caché del procesador:

```
L1 cache  →  L2 cache  →  L3 cache  →  RAM principal
  4 KB            64 KB         512 KB        16–64 MB
```

El benchmark ejecuta dos estrategias de acceso:

- **Secuencial (`bench_seq`)** — recorre el array linealmente, aprovechando la localidad espacial y el prefetcher de hardware. Sirve como línea base de rendimiento óptimo.
- **Aleatorio (`bench_rand`)** — accede al array en orden mezclado con Fisher-Yates, destruyendo la localidad. Expone el costo real de los cache misses y TLB misses.

La diferencia entre ambas curvas revela dónde están los límites de cada nivel de caché.

---

## ⚙️ Parámetros configurables

| Macro | Valor actual | Descripción |
|-------|-------------|-------------|
| `REPEAT` | `10` | Repeticiones por benchmark (aumentar para mayor precisión) |

> ⚠️ Con `REPEAT=100` y arrays de 64 MB el tiempo total puede superar los 10 minutos.

---

## 📐 Tamaños evaluados

| Tamaño | Región esperada |
|--------|----------------|
| 4 KB   | L1 cache |
| 8 KB   | L1 cache |
| 16 KB  | L1 cache |
| 32 KB  | Límite típico L1d |
| 64 KB  | L2 cache |
| 128 KB | L2 cache |
| 256 KB | Límite típico L2 |
| 512 KB | L3 cache |
| 1 MB   | L3 cache |
| 4 MB   | Posible límite L3 |
| 16 MB  | RAM principal |
| 64 MB  | RAM principal |

---

## 🔧 Compilación y ejecución

### Compilar
```bash
gcc -O2 -o benchmark benchmark.c
```

### Ejecutar
```bash
./benchmark
```

### Salida esperada
```
Tamano(KB)    seq(ns/byte)  rand(ns/elem)
--------------------------------------------------
4                    0.312         1.204
32                   0.318         2.891
256                  0.401        15.330
1024                 1.205        52.780
16384                3.980       180.450
65536                4.100       195.200
```

> Los valores exactos dependen del hardware. El salto brusco en `rand(ns/elem)` marca el límite de cada nivel de caché.

---

## 🧩 Estructura del código

```
benchmark.c
├── bench_seq()   — Benchmark de acceso secuencial
├── bench_rand()  — Benchmark de acceso aleatorio (Fisher-Yates)
└── main()        — Ejecuta ambos sobre 12 tamaños y muestra resultados
```

### Detalles de implementación

- Usa `clock_gettime(CLOCK_MONOTONIC)` para medición de alta precisión
- El array se inicializa con `memset` para forzar la asignación real de páginas (evita zero-pages del SO)
- Los accesos se declaran `volatile` para que el compilador no los optimice
- La mezcla Fisher-Yates usa semilla fija (`srand(42)`) para reproducibilidad

---

## 📊 Cómo interpretar los resultados

- **Latencia baja y estable** → el array cabe en ese nivel de caché
- **Salto abrupto en `rand`** → el array superó el caché y los accesos van a un nivel más lento
- **`seq` casi constante** → el prefetcher compensa aunque el dato no esté en caché

---

## 📄 Licencia

Proyecto académico — Universidad Francisco de Paula Santander · 2026
