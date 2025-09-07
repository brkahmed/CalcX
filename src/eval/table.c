#include "table.h"

#include <string.h>

#include <xmalloc.h>

static TableEntry *table_entry_new_cfunction(const char *name, void *func, size_t arg_count) {
    TableEntry *entry = xmalloc(TableEntry);
    entry->name       = strdup(name);
    entry->type       = ENTRY_TYPE_CFUNCTION;
    entry->cfunc      = func;
    entry->next       = NULL;
    entry->min_args = entry->max_args = arg_count;
    return entry;
}

static TableEntry *table_entry_new_function(const char *name, Function func, size_t min_args, size_t max_args) {
    TableEntry *entry = xmalloc(TableEntry);
    entry->name       = strdup(name);
    entry->type       = ENTRY_TYPE_FUNCTION;
    entry->func       = func;
    entry->min_args   = min_args;
    entry->max_args   = max_args;
    entry->next       = NULL;
    return entry;
}

static TableEntry *table_entry_new_number(const char *name, Number value) {
    TableEntry *entry = xmalloc(TableEntry);
    entry->name       = strdup(name);
    entry->type       = ENTRY_TYPE_NUMBER;
    entry->num        = value;
    entry->next       = NULL;
    return entry;
}

static inline void table_entry_free(TableEntry *entry) {
    free(entry->name);
    free(entry);
}

void table_init(Table *table) {
    table->entries = NULL;
    table->count   = 0;
    table->size    = 0;
}

void table_clear(Table *table) {
    for (size_t i = 0; i < table->size; i++) {
        TableEntry *curr = table->entries[i];
        while (curr) {
            TableEntry *next = curr->next;
            table_entry_free(curr);
            curr = next;
        }
    }
    free(table->entries);
    table->size  = 0;
    table->count = 0;
}

void table_iter_init(Table *table, TableIterator *iterator) {
    iterator->table = table;
    iterator->idx   = 0;
    iterator->next  = NULL;
}

TableEntry *table_iter_next(TableIterator *iterator) {
    TableEntry *result;
    if ((result = iterator->next)) {
        iterator->next = result->next;
        return result;
    }
    while (iterator->idx < iterator->table->size && !result) result = iterator->table->entries[iterator->idx++];
    if (result) iterator->next = result->next;
    return result;
}

static inline unsigned long hash_str(const char *str) { // ? Found online
    unsigned long hash = 1469598103934665603ULL;        // FNV offset basis
    while (*str) {
        hash ^= (unsigned char)*str++;
        hash *= 1099511628211ULL; // FNV prime
    }
    return hash;
}

TableEntry *table_lookup(Table *table, const char *name) {
    if (table->count == 0) return NULL;
    unsigned long hash = hash_str(name);
    TableEntry *entry  = table->entries[hash & (table->size - 1)];
    while (entry) {
        if (strcmp(entry->name, name) == 0) return entry;
        entry = entry->next;
    }
    return NULL;
}

static TableEntry *table_add_entry(Table *table, TableEntry *entry) {
    if (table->count >= table->size) {
        TableEntry **old_entries = table->entries;
        size_t old_size          = table->size;
        table->count             = 0;
        table->size              = table->size == 0 ? TABLE_INITIAL_SIZE : table->size * 2;
        table->entries           = xcalloc(table->size, TableEntry *);
        for (size_t i = 0; i < old_size; i++) {
            TableEntry *e = old_entries[i];
            while (e) {
                TableEntry *next = e->next;
                e->next          = NULL;
                table_add_entry(table, e);
                e = next;
            }
        }
        free(old_entries);
    }
    unsigned long hash     = hash_str(entry->name);
    TableEntry **entry_ptr = table->entries + (hash & (table->size - 1));
    entry->next            = *entry_ptr;
    *entry_ptr             = entry;
    table->count++;
    return entry;
}

TableEntry *table_set_cfunction(Table *table, const char *name, void *func, size_t arg_count) {
    TableEntry *entry = table_lookup(table, name);
    if (entry) {
        entry->type     = ENTRY_TYPE_CFUNCTION;
        entry->cfunc    = func;
        entry->min_args = entry->max_args = arg_count;
    } else {
        entry = table_entry_new_cfunction(name, func, arg_count);
        table_add_entry(table, entry);
    }
    return entry;
}

TableEntry *table_set_function(Table *table, const char *name, Function func, size_t min_args, size_t max_args) {
    TableEntry *entry = table_lookup(table, name);
    if (entry) {
        entry->type     = ENTRY_TYPE_FUNCTION;
        entry->func     = func;
        entry->min_args = min_args;
        entry->max_args = max_args;
    } else {
        entry = table_entry_new_function(name, func, min_args, max_args);
        table_add_entry(table, entry);
    }
    return entry;
}

TableEntry *table_set_number(Table *table, const char *name, Number value) {
    TableEntry *entry = table_lookup(table, name);
    if (entry) {
        entry->type = ENTRY_TYPE_NUMBER;
        entry->num  = value;
    } else {
        entry = table_entry_new_number(name, value);
        table_add_entry(table, entry);
    }
    return entry;
}
