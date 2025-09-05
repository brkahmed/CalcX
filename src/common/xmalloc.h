#ifndef X_MALLOC_H
#define X_MALLOC_H

#include <stdio.h>
#include <stdlib.h>

#define xmalloc(type)                                                                                                  \
    ({                                                                                                                 \
        type *ptr = (type *)malloc(sizeof(type));                                                                      \
        if (!ptr) {                                                                                                    \
            perror("Memory allocation failed");                                                                        \
            exit(EXIT_FAILURE);                                                                                        \
        }                                                                                                              \
        ptr;                                                                                                           \
    })

#define xrealloc(ptr, type, new_size)                                                                                  \
    ({                                                                                                                 \
        type *new_ptr = (type *)realloc(ptr, sizeof(type) * (new_size));                                               \
        if (!new_ptr) {                                                                                                \
            perror("Memory reallocation failed");                                                                      \
            exit(EXIT_FAILURE);                                                                                        \
        }                                                                                                              \
        new_ptr;                                                                                                       \
    })

#endif // X_MALLOC_H
