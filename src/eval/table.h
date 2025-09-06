#ifndef EVAL_TABLE_H
#define EVAL_TABLE_H

#include <stddef.h>

#include "types.h"

#define TABLE_INITIAL_SIZE 128

typedef enum {
    ENTRY_TYPE_NUMBER,
    ENTRY_TYPE_FUNCTION,
    ENTRY_TYPE_CFUNCTION,
} EntryType;

typedef struct TableEntry {
    const char *name;
    EntryType type;
    union {
        Number num;
        struct {
            union {
                Function func;
                void *cfunc;
            };
            size_t min_args;
            size_t max_args;
        };
    };
    struct TableEntry *next;
} TableEntry;

typedef struct {
    TableEntry **entries;
    size_t count;
    size_t size;
} Table;

typedef struct {
    Table *table;
    size_t idx;
    TableEntry *head;
    TableEntry *next;
} TableIterator;

void table_init(Table *table);
void table_clear(Table *table);
void table_iter_init(Table *table, TableIterator *iterator);
TableEntry *table_iter_next(TableIterator *iterator);
TableEntry *table_lookup(Table *table, const char *name);
TableEntry *table_set_cfunction(Table *table, const char *name, void *func, size_t arg_count);
TableEntry *table_set_function(Table *table, const char *name, Function func, size_t min_args, size_t max_args);
TableEntry *table_set_number(Table *table, const char *name, Number value);

#endif // EVAL_TABLE_H
