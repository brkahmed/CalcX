#ifndef EVAL_TABLE_H
#define EVAL_TABLE_H

#include <stdlib.h>

#include "eval.h"

typedef enum {
    ENTRY_TYPE_NUMBER,
    ENTRY_TYPE_CONST,
    ENTRY_TYPE_FUNCTION,
    ENTRY_TYPE_CFUNCTION,
} EntryType;

typedef struct {
    const char *name;
    EntryType type;
    union {
        Number *num;
        Number constnum;
        struct {
            union {
                Function func;
                void *cfunc;
            };
            size_t min_args;
            size_t max_args;
        };
    };
} TableEntry;

typedef struct {
    const char **keys;
    TableEntry **entries;
    size_t len;
    size_t size;
} Table;

void table_init(Table *table);
TableEntry *table_set_number(Table *table, const char *name, Number *value);
TableEntry *table_set_function(Table *table, const char *name, Function func, size_t min_args, size_t max_args);
TableEntry *table_set_cfunction(Table *table, const char *name, void *func, size_t arg_count);
TableEntry *table_set_const(Table *table, const char *name, Number value);
TableEntry *table_lookup(Table *table, const char *name);

#endif // EVAL_TABLE_H
