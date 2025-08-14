#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <setjmp.h>

typedef jmp_buf error_handler;


#define _ERROR(msg, ...) fprintf(stderr,"Error: " msg "\n",  ##__VA_ARGS__)

#ifdef DEBUG
#define ERROR(msg, ...) _ERROR(msg "\nLINE: %d in function: %s", ##__VA_ARGS__, __LINE__, __func__);
#else
#define ERROR(msg, ...) _ERROR(msg, ##__VA_ARGS__)
#endif


#define RAISE(handler, error_type, msg, ...) do { \
    ERROR(msg, ##__VA_ARGS__); \
    longjmp(handler, error_type); \
} while (0) \


#define TRY(handler) \
    int __##handler##_code = setjmp(handler);\
    if (__##handler##_code == 0)


#define GET_MACRO(_0, _1, name, ...) name
#define EXCEPT(...) GET_MACRO(__VA_ARGS__, EXCEPT_ERROR, EXCEPT_ALL)(__VA_ARGS__)
#define EXCEPT_ERROR(error_type) else if (__##handler##_code == error_type)
#define EXCEPT_ALL() else


#endif