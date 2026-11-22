#ifndef MONITOR_H
#define MONITOR_H

typedef struct {
    char   codigo[7];      
    char   titulo[40];      
    double punt;               
    int    votos;               
} Episodio;

//busca el episodio, si no lo encuentra devuelve null, si lo encuentra un puntero a ese episodio
const Episodio *buscar_episodio(const char *codigo);

//escribe el texto para el cliente, devuelve la cant de caracteres que se escribieron en buf, si no hay espacio suficiente devuelve -1
int formatear_episodio(const Episodio *ep, char *buf, int tam);

#endif