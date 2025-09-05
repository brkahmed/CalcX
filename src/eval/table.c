#include "table.h"

#include <string.h>

#include <xmalloc.h>

TableEntry *table_entry_new_number(const char *name, Number *value) {
    TableEntry *entry = xmalloc(TableEntry);
    entry->name       = name;
    entry->type       = ENTRY_TYPE_NUMBER;
    entry->num        = value;
    return entry;
}

TableEntry *table_entry_new_const(const char *name, Number value) {
    TableEntry *entry = xmalloc(TableEntry);
    entry->name       = name;
    entry->type       = ENTRY_TYPE_CONST;
    entry->constnum   = value;
    return entry;
}

TableEntry *table_entry_new_function(const char *name, Function func, size_t min_args, size_t max_args) {
    TableEntry *entry = xmalloc(TableEntry);
    entry->name       = name;
    entry->type       = ENTRY_TYPE_FUNCTION;
    entry->func       = func;
    entry->min_args   = min_args;
    entry->max_args   = max_args;
    return entry;
}

TableEntry *table_entry_new_cfunction(const char *name, void *func, size_t arg_count) {
    TableEntry *entry = xmalloc(TableEntry);
    entry->name       = name;
    entry->type       = ENTRY_TYPE_CFUNCTION;
    entry->cfunc      = func;
    entry->min_args = entry->max_args = arg_count;
    return entry;
}

void table_init(Table *table) {
    table->keys    = NULL;
    table->entries = NULL;
    table->len     = 0;
    table->size    = 0;
}

TableEntry *table_add_entry(Table *table, TableEntry *entry) {
    if (table->len >= table->size) {
        size_t new_size = table->size == 0 ? 8 : table->size * 2;
        table->keys     = xrealloc(table->keys, const char *, new_size);
        table->entries  = xrealloc(table->entries, TableEntry *, new_size);
        table->size     = new_size;
    }
    table->keys[table->len]    = entry->name;
    table->entries[table->len] = entry;
    table->len++;
    return entry;
}

TableEntry *table_set_number(Table *table, const char *name, Number *value) {
    TableEntry *entry = table_lookup(table, name);
    if (entry) {
        if (entry->type != ENTRY_TYPE_NUMBER) entry->type = ENTRY_TYPE_NUMBER;
        entry->num = value;
    } else {
        entry = table_entry_new_number(name, value);
        table_add_entry(table, entry);
    }
    return entry;
}

TableEntry *table_set_const(Table *table, const char *name, Number value) {
    TableEntry *entry = table_lookup(table, name);
    if (entry) {
        if (entry->type != ENTRY_TYPE_CONST) entry->type = ENTRY_TYPE_CONST;
        entry->constnum = value;
    } else {
        entry = table_entry_new_const(name, value);
        table_add_entry(table, entry);
    }
    return entry;
}

TableEntry *table_set_function(Table *table, const char *name, Function func, size_t min_args, size_t max_args) {
    TableEntry *entry = table_lookup(table, name);
    if (entry) {
        if (entry->type != ENTRY_TYPE_FUNCTION) entry->type = ENTRY_TYPE_FUNCTION;
        entry->func     = func;
        entry->min_args = min_args;
        entry->max_args = max_args;
    } else {
        entry = table_entry_new_function(name, func, min_args, max_args);
        table_add_entry(table, entry);
    }
    return entry;
}

TableEntry *table_set_cfunction(Table *table, const char *name, void *func, size_t arg_count) {
    TableEntry *entry = table_lookup(table, name);
    if (entry) {
        if (entry->type != ENTRY_TYPE_CFUNCTION) entry->type = ENTRY_TYPE_CFUNCTION;
        entry->cfunc     = func;
        entry->min_args = entry->max_args = arg_count;
    } else {
        entry = table_entry_new_cfunction(name, func, arg_count);
        table_add_entry(table, entry);
    }
    return entry;
}

TableEntry *table_lookup(Table *table, const char *name) {
    for (size_t i = 0; i < table->len; i++) {
        if (strcmp(table->entries[i]->name, name) == 0) return table->entries[i];
    }
    return NULL;
}
