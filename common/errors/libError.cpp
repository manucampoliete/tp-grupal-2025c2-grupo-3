/*
 * Este es un hack alrededor de un bug de G++ que, a pesar de decirle
 * que use un estándar (véase el Makefile), igualmente usa código
 * que es especifico de GNU (y no POSIX).
 *
 * En particular esto afecta a la funcion `strerror_r`.
 * Cuando se usa el estándar POSIX y `GNU_SOURCE` no esta definido, `strerror_r`
 * retorna un `int`.
 *
 * En cambio, con `GNU_SOURCE`, la función retornar un `char*` y no
 * necesariamente podrán el mensaje de error en el buffer sino como retorno.
 * (véase más abajo), lo cual esta claramente mal.
 *
 * Estos "un-define" están para forzar el uso de POSIX y sacar `GNU_SOURCE`
 * al menos en este `.cpp`.
 * */
#undef _GNU_SOURCE
#undef GNU_SOURCE

#include "libError.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

#include <errno.h>

LibError::LibError(int errorCode, const char* fmt, ...) noexcept {
    /* Aquí empieza la magia arcana proveniente de C.
     *
     * En C (y en C++) las funciones y métodos pueden recibir un número
     * arbitrario de argumentos. Esto se especifica con las elipsis
     * en la firma de la función.
     *
     * Tanto `errorCode` como `fmt` son argumentos formales. A continuación
     * le siguen cero o más argumentos, los llamados variadicos.
     *
     * Internamente los argumentos variadicos son cargados en el stack
     * y para marcar el principio de estos debemos llamar `va_start`
     * con el nombre del ultimó parámetro formal conocido, `fmt` en
     * este caso.
     * */
    va_list args;
    va_start(args, fmt);

    /*
     * `vsnprintf` es una función similar a `printf` que guarda en
     * un buffer `msgError` el string `fmt` formateado con los
     * argumentos variadicos `args`.
     *
     * Para evitar overflows le pasamos el límite del buffer.
     * La documentación oficial indica que `vsnprintf` escribirá
     * a lo sumo esa cantidad de bytes incluyendo el `\0`.
     * */
    int s = vsnprintf(msgError, sizeof(msgError), fmt, args);

    /* Una vez que hemos usado los argumentos variadicos hay que liberarlos.
     * Conceptualmente es como si estuvieran guardados en una lista
     * aunque internamente están en el stack del programa.
     * */
    va_end(args);

    if (s < 0) {
        /* Algo falló al llamar a `vsnprintf` pero no podemos hacer nada.
         *
         * Lanzar una excepción no es una opción: `LibError` es una excepción
         * en sí y es altamente probable que este constructor se este llamando
         * por que hay una excepción en curso y lanzar otra excepción cuando
         * hay otra en curso hara que el programa de C++ aborte.
         *
         * Otros lenguajes no son tan catastróficos: Java y Python simplemente
         * encadenan la excepción original y la nueva y continúan la propagación
         * del error.
         *
         * Pero en C++ no es así. Prefiero entonces simplemente ignorar el error
         * poniendo algún mensaje dummy.
         * */
        msgError[0] = msgError[1] = msgError[2] = '?';
        msgError[3] = ' ';
        msgError[4] = '\0';

        /*
         * `vsnprintf` retorna la cantidad de bytes escritos sin incluir el `\0`
         * Por lo tanto, si nosotros escribimos artificialmente `"??? \0"`
         * debemos indicar 4 bytes y no 5 ya que no debemos contar el `\0`
         * */
        s = 4;
    } else if (s == sizeof(msgError)) {
        /* Esto también técnicamente es un error ya que el mensaje formateado
         * fue más grande que el buffer `msgError`.
         * No hubo un overflow pero el mensaje en `msgError` esta truncado.
         *
         * En otros contextos yo lanzaría una excepción pero por lo mencionado
         * anteriormente simplemente ignorare el error.
         * */
    }

    /*
     * `strerror_r` toma el `errorCode` y lo traduce a un mensaje entendible
     * por el humano y lo escribe en el buffer. A diferencia de `strerror`,
     * `strerror_r` es thread safe ya que usa un buffer local (`msgError`)
     * y no uno `static` (aka, global).
     *
     * Nótese como `msgError+s` apunta justo al `\0` escrito por `vsnprintf`
     * y es exactamente lo que queremos: queremos escribir a continuación
     * de lo escrito por `vsnprintf` pisándole el `\0`.
     * */
    strerror_r(errorCode, msgError + s, sizeof(msgError) - s);

    /*
     * `strerror_r` garantiza que el string termina siempre en un `\0`
     * sin embargo permitime ser un poco paranoico y asegurarme que
     * realmente hay un `\0` al final.
     * */
    msgError[sizeof(msgError) - 1] = 0;
}

const char* LibError::what() const noexcept { return msgError; }

LibError::~LibError() {}
