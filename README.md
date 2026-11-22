# Proyecto Final – Programación de Sistemas (2026–2027)

**Servidor concurrente TCP de calificaciones IMDb – "A Knight of the Seven Kingdoms"**

Grupo 2 – repositorio `ps-uclv-2026-2027-proyecto-integrador-ps-uclv-2026-2027-group-2`

## Integrantes

| Nombre y apellidos | Usuario de GitHub |
|---|---|
| José Rafael Sánchez Correa | JoseR-SC |
| Pedro David Hurtado Sarduy | pedroDavid050605 |

## Contenido

| Archivo | Descripción |
|---|---|
| `server.c` | Servidor concurrente TCP (puerto 6666) |
| `client.c` | Cliente de consola |
| `monitor.c` / `monitor.h` | Base de datos de episodios y formato de la respuesta; se empaquetan en `libmonitor.a` |
| `Makefile` | Reglas `all` y `clean` |
| `screenshot/` | Capturas de pantalla del funcionamiento |

## Cómo compilar

```bash
make all
make clean
```

## Cómo ejecutar

Servidor (terminal 1):

```bash
./server
```

Cliente (terminal 2, 3, ...). Recibe como argumento el código de 6 caracteres del episodio:

```bash
./client S01E01
./client S01E05
```

Ejemplo de respuesta:

```
Respuesta del servidor: S01E01 | The Hedge Knight | Puntuacion IMDB: 9.2| Votos: 45120
```

Códigos disponibles: `S01E01` a `S01E06`. Un código con formato correcto pero inexistente (`S09E09`) devuelve
`Episodio no encontrado`. Un código con formato incorrecto (`ABC`) lo rechaza el cliente antes de conectarse;
si llegara igualmente al servidor, este responde `Codigo invalido`.

### Prueba automática (smoke test)

```bash
./server --test-run
echo $?
```

Comprueba que la base de datos contiene los 6 episodios, abre el socket en el puerto 6666 y, para cada código
(`S01E01` a `S01E06`), se conecta a sí mismo y verifica que la respuesta coincide con la esperada. Termina con
código 0 si todo es correcto (1 si algo falla, por ejemplo si el puerto 6666 ya está ocupado). Se puede ejecutar
varias veces seguidas.

## Decisiones de diseño

- **Concurrencia con `fork()`**: el padre solo hace `accept()`; cada conexión se atiende en un proceso hijo
  independiente, de modo que varios clientes se sirven a la vez.
- **IPC con `pipe()`**: antes de atender la solicitud, el hijo escribe la IP del cliente en una tubería sin nombre.
  El padre la lee e imprime `Cliente <IP> atendido por el proceso hijo PID <pid>`. Cada conexión usa su propia
  tubería, que ambos procesos cierran en los extremos que no usan.
- **Señales**:
  - `SIGINT` se ignora (`sigaction` con `SIG_IGN`), por lo que CTRL+C no detiene el servidor.
  - `SIGCHLD` se captura con `sigaction` (`SA_RESTART`) y el manejador llama a `waitpid(-1, NULL, WNOHANG)` en
    bucle, por lo que no quedan procesos zombi.
- **Biblioteca estática `libmonitor.a`**: contiene la tabla `bd_episodios` y las funciones `buscar_episodio()` y
  `formatear_episodio()`. Así la lógica de datos queda separada de la de red.
- **Validación de entrada**: el código debe tener la forma `SddEdd` (6 caracteres). Se valida en el cliente y
  también en el servidor, que no debe confiar en lo que recibe.
- **`SO_REUSEADDR`**: permite reiniciar el servidor (y repetir `--test-run`) sin esperar al estado `TIME_WAIT`.
- **`setvbuf(stdout, _IOLBF)`**: la salida va por líneas para que los hijos no dupliquen mensajes pendientes en
  el búfer al hacer `fork()`, sobre todo si la salida se redirige a un archivo.
- **`--test-run` sin `fork()`**: cada conexión de prueba queda en la cola de `listen()`, por lo que `accept()` no
  bloquea y el test es determinista y rápido.
