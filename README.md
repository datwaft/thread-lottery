# `[SOA]` - Proyecto 1: Lotería de "*Threads*"

Tarea corta creada por:

- Abreu Cárdenas, Miguel Guillermo
- Guevara Sánchez, David Alberto
- Ortíz Ruiz, Victor Adolfo
- Yip Chen, Michael

## Cómo ejecutar este proyecto

```sh
# Primero se necesita o clonar el repositorio o extraer el tarball. e.g.
tar -xvf Abreu-Guevara-Ortiz-Yip.tgz
# Luego de eso se necesita compilar el proyecto.
# Si clang no está instalado o se desea usar otro compilador ejecutar como: make CC=gcc
make
# Y finalmente se puede ejecutar el proyecto. e.g.
./build/main
```

## Descripción de Interfaz Gráfica de Usuario

### **Configuración para todos los `Threads`**

**Modo Operación:** define el modo de operación con el cual se ejecutara todos los `Threads` dentro del `scheduler`.

**Número de `Threads`**: define la cantidad de `Threads` para ser ejecutado por `scheduler`.

- Cantidad por defecto: 5 `Threads`.
- Cantidad mínima: 5 `Threads`.
- Cantidad máxima: "el adecuado".

**Trabajo Mínimo:**

- **Porcentaje (modo no expropiativo):** define el porcentaje de trabajo a realizar en el `Thread` antes de ceder el procesador. 
  - Porcentaje por defecto: 10 (%)
  - Porcentaje mínimo: 1 (%)
  - Porcentaje máximo: 100 (%)

- **Quantum (modo expropiativo)**: define la cantidad de tiempo en mili-segundos (ms) de ejecución para cada `Thread` antes de ser expropiado por el procesador.
  - Quantum por defecto: 100 (ms).
  - Quantum mínimo: 1 (ms).
  - Quantum máximo: 10000 (ms).

### **Configuración por `Thread`**

- **Cantidad de Boletos:** define la cantidad de boletos que posee el `Thread` para competir en el `scheduler`.

  - Cantidad de boletos por defecto: 5.
  - Cantidad de boletos mínimo: 1.
  - Cantidad de boletos máximo: 1000.

- **Cantidad de Trabajo:** define la unidad de trabajo del `Thread` para realizar el `cálculo de pi`.

  - Cantidad de trabajo por defecto: 100000.

  - Cantidad de trabajo mínimo: 1.

  - Cantidad de trabajo máximo: 10000000.

### Ejecución

En la columna de `Thread #` se desplegara el `id` del `Thread` correspondiente en la configuración. La representación de color es la siguiente:

- Rojo: `Thread` con trabajo pendiente.
- Verde: `Thread` con trabajo completado.

En la columna de `Progreso` se despliega la barra de progreso actualizado en tiempo real con respecto al trabajo realizado.

En la columna de `Resultado` se despliega el resultado actualizado en tiempo real, y acumulado al momento de `cálculo de pi`.

![general-20230326195205884](https://user-images.githubusercontent.com/52944834/227838770-310ad4a0-ecb7-4c38-963d-ab25c9df41f8.png)

## Instrucciones de uso

1. Seleccione el `Modo Operación`.
2. Establezca la cantidad de `Threads` para ejecutar.
3. Establezca por `Thread` la cantidad de boletos y cantidad de trabajo a realizar.
4. Clic en el botón de `Ejecutar` para iniciar el `scheduler`.

## Ejemplo Caso Modo No Expropiativo

En esta configuración 

- `Thread 1` posee la mayoría de boletos (999), por lo tanto, su trabajo es terminado primero.

- `Thread 2` posee la segunda mayoría de boletos (500). Por lo tanto termina de segundo.
- El resto de los `Threads` compiten entre sí hasta completar su trabajo.

![non-preemptive-20230326204830440](https://user-images.githubusercontent.com/52944834/227838774-c88b6b05-eba3-4d21-909a-39cd6ae36b15.png)

![non-preemptive-20230326204852210](https://user-images.githubusercontent.com/52944834/227838775-0207bdd7-2a32-4a8e-ae8a-6e44a3eee82c.png)



## Ejemplo Caso Modo Expropiativo

En esta configuración:

- `Thread 1` posee la mayoría de boletos (500), por lo tanto, su trabajo es terminado primero. 

- `Thread 2` posee la segunda mayoría de boletos (200). Por lo tanto termina de segundo.
- `Thread 3` y `Thread 4` poseen la tercera mayoría de boletos (50). Por lo que compiten entre sí, y tienen un avance de ejecución mayor que el resto pendiente.
- El resto de los `Threads` compiten entre sí hasta completar su trabajo.

![preemptive-20230326214816286](https://user-images.githubusercontent.com/52944834/227838780-053cc419-7458-4775-b2e8-fcbfa23dab79.png)

![preemptive-20230326214826099](https://user-images.githubusercontent.com/52944834/227838781-b7fd5198-92c1-489d-bbdd-0c373470be7c.png)

![preemptive-20230326214835414](https://user-images.githubusercontent.com/52944834/227838782-2d8f9e7d-10a1-4881-a0ee-5512c88a8f98.png)

![preemptive-20230326214849978](https://user-images.githubusercontent.com/52944834/227838784-c24efb23-23e3-4d57-b6d8-bc5515ff0768.png)

![preemptive-20230326214858652](https://user-images.githubusercontent.com/52944834/227838785-b8a5c9f7-beae-45a7-b105-a74621c97434.png)

## Información adicional

El proyecto fue implementado en su totalidad según los términos del enunciado. Todo el proyecto está funcionando.
