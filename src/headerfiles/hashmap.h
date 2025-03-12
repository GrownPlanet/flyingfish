/*
 * Hashmap
 *
 * A simple hashmap for with String_t for the key and size_t for the value
 *
 */

#ifndef HASHMAP_H
#define HASHMAP_H

#include "string_utils.h"
#include "stddef.h"

#define HASHMAP_INITIAL_LENGTH 16
#define HASHMAP_MAX_LOAD_FACTOR 0.75
#define HASHMAP_GROWTH_RATE 2

typedef struct {
    String_t key;
    size_t value;
    bool taken;
} Entry_t;

typedef struct {
    Entry_t* data;
    size_t capacity;
    size_t len;
} HashMap_t;

HashMap_t new_hashmap();
int hashmap_insert(HashMap_t* hashmap, String_t key, size_t value);
void hashmap_remove(HashMap_t* hashmap, String_t key);

typedef struct {
    size_t value;
    bool had_error;
} HM_GetResult_t;
HM_GetResult_t hashmap_get(HashMap_t* hashmap, String_t key);

#endif // HASHMAP_H
