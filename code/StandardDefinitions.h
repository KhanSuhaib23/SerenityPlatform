// TODO(Suhaib): Do proper line endings
// TODO(Suhaib): Move to our own file io system

#ifndef STANDARD_DEFINITIONS_H
#define STANDARD_DEFINITIONS_H

#include <stdio.h>


#define KiloBytes(value) (value * 1024LL)
#define MegaBytes(value) (KiloBytes(value) * 1024LL)
#define GigaBytes(value) (MegaBytes(value) * 1024LL)
#define TeraBytes(value) (GigaBytes(value) * 1024LL)

#define internal static
#define global static
#define persist static

typedef unsigned char boolean;

typedef char int8;
typedef unsigned char uint8;

typedef short int16;
typedef unsigned short uint16;

typedef int int32;
typedef unsigned int uint32;

typedef long long int64;
typedef unsigned long long uint64;

typedef float float32;

typedef double float64;


#endif

