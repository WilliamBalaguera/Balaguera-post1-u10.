# 🧪 Testing Suite — Task Manager API

> Pruebas automatizadas con JUnit 5, Mockito y cobertura JaCoCo sobre una aplicación Spring Boot de gestión de tareas.

---

## 👤 Información del proyecto

| Campo        | Detalle                              |
|--------------|--------------------------------------|
| **Autor**    | William Balaguera                    |
| **Materia**  | Programación Web                     |
| **Unidad**   | 10                                   |
| **Año**      | 2026                                 |
| **Universidad** | Francisco de Paula Santander      |

---

## 📌 ¿Qué hace este proyecto?

Este repositorio contiene una **suite completa de pruebas automatizadas** para una API REST de gestión de tareas construida con Spring Boot. El objetivo es validar el comportamiento correcto de las capas de servicio, controlador y repositorio, garantizando una cobertura de código mínima del **70%** mediante JaCoCo.

---

## 🛠️ Stack tecnológico

| Tecnología | Versión | Rol |
|------------|---------|-----|
| Java | 23 | Lenguaje base |
| Spring Boot | 3.5.14 | Framework principal |
| JUnit 5 | — | Motor de pruebas |
| Mockito | — | Mocking de dependencias |
| JaCoCo | 0.8.12 | Medición de cobertura |
| H2 Database | — | BD en memoria para tests |
| Maven | Wrapper incluido | Gestión de dependencias |

---

## 📁 Estructura del proyecto

```
balaguera-post1-u10/
├── src/
│   ├── main/
│   │   └── java/         # Código fuente principal
│   └── test/
│       └── java/         # Clases de prueba
├── capturas/             # Evidencias del reporte JaCoCo
├── .mvn/wrapper/         # Maven Wrapper
├── pom.xml               # Configuración Maven
└── README.md
```

---

## 🔬 Clases de prueba

### `TareaServiceTest` — Pruebas Unitarias
- **Tipo:** Unitaria con Mockito
- **Propósito:** Verifica la lógica de negocio aislando dependencias mediante mocks
- **Tests:** 4 casos de prueba

### `TareaControllerTest` — Pruebas de Capa Web
- **Tipo:** `@WebMvcTest`
- **Propósito:** Prueba los endpoints REST usando `MockMvc` sin levantar el servidor completo
- **Tests:** Valida respuestas HTTP, serialización JSON y rutas

### `TareaRepositoryTest` — Pruebas de Persistencia
- **Tipo:** `@DataJpaTest`
- **Propósito:** Verifica consultas JPA contra una base de datos H2 en memoria
- **Tests:** Valida operaciones CRUD y queries personalizadas

---

## ▶️ Cómo ejecutar

### 1. Correr todos los tests
```bash
./mvnw test
```

### 2. Verificar umbral de cobertura (≥ 70%)
```bash
./mvnw clean test jacoco:check
```

### 3. Ver reporte de cobertura en el navegador
```bash
# Después de correr los tests, abrir:
target/site/jacoco/index.html
```

> **Windows:** usar `mvnw.cmd` en lugar de `./mvnw`

---

## ✅ Resultados de las pruebas

```
Tests run: 9
Failures:  0
Errors:    0
Skipped:   0

Cobertura de líneas: 71% ✔ (umbral mínimo: 70%)
```

---

## 📋 Checkpoints de la unidad

- [x] **Checkpoint 1** — 4 pruebas unitarias pasan correctamente con Mockito
- [x] **Checkpoint 2** — Tests de integración `@WebMvcTest` y `@DataJpaTest` exitosos
- [x] **Checkpoint 3** — JaCoCo reporta **71%** de cobertura de líneas (≥ 70% requerido)

---

## 📸 Evidencias del reporte JaCoCo

Las capturas del reporte de cobertura se encuentran en la carpeta [`/capturas`](./capturas/):

| Captura | Descripción |
|---------|-------------|
| `img.png` | Vista general del reporte |
| `img_1.png` | Cobertura por clase |
| `img_2.png` | Detalle de líneas cubiertas |
| `img_3.png` | Resultado de la verificación del umbral |

---

## 📄 Licencia

Proyecto académico — Universidad Francisco de Paula Santander · 2026
