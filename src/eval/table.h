#ifndef EVAL_TABLE_H
#define EVAL_TABLE_H

#include <stdbool.h>
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

typedef struct Table {
    TableEntry **entries;
    size_t count;
    size_t size;
    struct Table *enclosing;
} Table;

typedef struct {
    Table *table;
    size_t idx;
    TableEntry *next;
    bool iter_enclosing;
} TableIterator;

void table_init(Table *table);
void table_clear(Table *table);
void table_iter_init(Table *table, TableIterator *iterator, bool iter_enclosing);
TableEntry *table_iter_next(TableIterator *iterator);
TableEntry *table_lookup(Table *table, const char *name, bool look_enclosing);
TableEntry *table_set_cfunction(Table *table, const char *name, void *func, size_t arg_count);
TableEntry *table_set_function(Table *table, const char *name, Function func, size_t min_args, size_t max_args);
TableEntry *table_set_number(Table *table, const char *name, Number value);

#endif // EVAL_TABLE_H
