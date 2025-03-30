#include <stdio.h>
#include <stdlib.h>

#include "hashmap.h"
#include "numtypes.h"

// see https://cp-algorithms.com/string/string-hashing.html for more information
size_t compute_hash(String_t s) {
    const int p = 31;
    const int m = 1e9 + 9;
    size_t hash_value = 0;
    size_t p_pow = 1;

    for (size_t i = 0; i < s.len; i++) {
        const char c = s.chars[i];
        hash_value = (hash_value + (c - 'a' + 1) * p_pow) % m;
        p_pow = (p_pow * p) % m;
    }

    return hash_value;
}

HashMap_t new_hashmap() {
    Entry_t* data = (Entry_t*)malloc(sizeof(Entry_t) * HASHMAP_INITIAL_LENGTH);

    for (size_t i = 0; i < HASHMAP_INITIAL_LENGTH; i++) {
        data[i].active = false;
        data[i].was_taken = false;
    }

    return (HashMap_t) {
        .data = data,
        .capacity = HASHMAP_INITIAL_LENGTH,
        .len = 0,
    };
}

int hashmap_realloc(HashMap_t* hashmap) {
    size_t new_capacity = hashmap->capacity * HASHMAP_GROWTH_RATE;
    Entry_t* new_data = (Entry_t*)malloc(sizeof(Entry_t) * new_capacity);
    if (new_data == NULL) { 
        printf("malloc failed!\n");
        return 1; 
    }

    for (size_t i = 0; i < new_capacity; i++) {
        new_data[i].active = false;
        new_data[i].was_taken = false;
    }

    for (size_t i = 0; i < hashmap->capacity; i++) {
        Entry_t entry = hashmap->data[i];
        if (!entry.active) continue;

        size_t index = compute_hash(entry.key) % hashmap->capacity;
        Entry_t* hm_entry = &hashmap->data[index];
        while (hm_entry->was_taken && !string_cmp(hm_entry->key, entry.key)) {
            index = (index + 1) % hashmap->capacity;
            hm_entry = &hashmap->data[index];
        }
        
        new_data[index] = entry;
    }

    hashmap->data = new_data;
    hashmap->capacity = new_capacity;

    return 0;
}

int hashmap_insert(HashMap_t* hashmap, String_t key, size_t value, TokenType_t type) {
    // reallocate if hashmap is too full
    float load_factor = ((float)hashmap->len + 1.0) / (float)hashmap->capacity;
    if (load_factor >= HASHMAP_MAX_LOAD_FACTOR) {
        int res = hashmap_realloc(hashmap);
        if (res == 1) { return res; }
    }

    // insert the value
    size_t index = compute_hash(key) % hashmap->capacity;
    Entry_t* entry = &hashmap->data[index];

    while (entry->active && !string_cmp(entry->key, key)) {
        index = (index + 1) % hashmap->capacity;
        entry = &hashmap->data[index];
    }

    entry->key = key;
    entry->value = value;
    entry->type = type;
    entry->active = true;
    entry->was_taken = true;

    hashmap->len += 1;

    return 0;
}

void hashmap_remove(HashMap_t* hashmap, String_t key) {
    Entry_t* entry = hashmap_get_entry(hashmap, key);
    entry->active = false;
}

HM_GetResult_t hashmap_get(HashMap_t* hashmap, String_t key) {
    Entry_t* entry = hashmap_get_entry(hashmap, key);
    if (!entry->active) {
        return (HM_GetResult_t) {
            .value = 0,
            .type = 0,
            .had_error = true,
        };
    }

    return (HM_GetResult_t) {
        .value = entry->value,
        .type = entry->type,
        .had_error = false,
    };
}

Entry_t* hashmap_get_entry(HashMap_t* hashmap, String_t key) {
    size_t index = compute_hash(key) % hashmap->capacity;
    Entry_t* entry = &hashmap->data[index];

    while (entry->was_taken && !string_cmp(entry->key, key)) {
        index = (index + 1) % hashmap->capacity;
        entry = &hashmap->data[index];
    }

    return entry;
}
