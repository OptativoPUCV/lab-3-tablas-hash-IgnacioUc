#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


HashMap * createMap(long capacity) {
    HashMap * map = (HashMap *)malloc(sizeof(HashMap));
    map->buckets = (Pair **)calloc(capacity, sizeof(Pair *));
    map->capacity = capacity;
    map->size = 0;
    map->current = -1;
    return map;
}

void insertMap(HashMap * map, char * key, void * value) {
    // Si se va a insertar un nuevo elemento, verificamos si causaría un load factor >= 0.7
    if ((float)(map->size + 1) / map->capacity >= 0.7) {
        enlarge(map);
    }

    long index = hash(key, map->capacity);
    long start = index;

    while (map->buckets[index] != NULL && map->buckets[index]->key != NULL) {
        if (is_equal(map->buckets[index]->key, key)) return; // Ya existe
        index = (index + 1) % map->capacity;
        if (index == start) return; // Evita loop infinito
    }

    map->buckets[index] = createPair(key, value);
    map->size++;
    map->current = index;
}

void enlarge(HashMap * map) {
    long old_capacity = map->capacity;
    Pair ** old_buckets = map->buckets;

    map->capacity *= 2;
    map->buckets = (Pair **)calloc(map->capacity, sizeof(Pair *));
    map->size = 0;

    for (long i = 0; i < old_capacity; i++) {
        if (old_buckets[i] != NULL && old_buckets[i]->key != NULL) {
            insertMap(map, old_buckets[i]->key, old_buckets[i]->value);
        }
    }
    free(old_buckets);
    enlarge_called = 1;
}

void eraseMap(HashMap * map, char * key) {
    Pair * pair = searchMap(map, key);
    if (pair != NULL) {
        pair->key = NULL;
        map->size--;
    }
}

Pair * searchMap(HashMap * map, char * key) {
    long index = hash(key, map->capacity);
    long start = index;

    while (map->buckets[index] != NULL) {
        if (map->buckets[index]->key != NULL && is_equal(map->buckets[index]->key, key)) {
            map->current = index;
            return map->buckets[index];
        }
        index = (index + 1) % map->capacity;
        if (index == start) break; // Loop completo
    }
    return NULL;
}

Pair * firstMap(HashMap * map) {
    for (long i = 0; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }
    return NULL;
}

Pair * nextMap(HashMap * map) {
    for (long i = map->current + 1; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }
    return NULL;
}
