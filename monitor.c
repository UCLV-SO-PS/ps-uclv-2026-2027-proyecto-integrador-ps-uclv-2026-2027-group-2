#include <stdio.h>
#include <string.h>
#include "monitor.h"

static const Episodio bd_episodios[] = {
    {"S01E01", "The Hedge Knight", 9.2, 45120},
    {"S01E02", "Hard Salt Beef", 8.8, 38900},
    {"S01E03", "The Squire", 9.5,52400},
    {"S01E04", "Seven", 9.1, 41200},
    {"S01E05", "In the Name of the Mother", 9.7, 68300},
    {"S01E06", "The Morrow", 9.4, 49800}
};

#define N_EPISODIOS 6

const Episodio *buscar_episodio(const char *codigo){
    if (codigo == NULL)
        return NULL;

    for (int i = 0; i < N_EPISODIOS; i++) {
        if (strcmp(bd_episodios[i].codigo, codigo) == 0)
            return &bd_episodios[i];
    }

    return NULL;
}



int formatear_episodio(const Episodio *ep, char *buf, int tam)
{
    if(ep ==NULL || buf == NULL || tam <= 0)
        return -1;  

    int n = snprintf(buf, tam,
                    "%s | %s | Puntuacion IMDB: %.1f| Votos: %d\n",
                    ep->codigo, ep->titulo, ep->punt, ep->votos);

    if(n < 0 || n >= tam)
        return -1;
    
    return n;
}