//! ======================================================================
//! Printing.hpp
//! Print macros for deframer unit tests
//! @author bocchino 
//! ======================================================================

#define PRINTING

#ifdef PRINTING
#include <cstdio>

#define PRINT(S) printf("[Deframer Tests] " S "\n");
#define PRINT_ARGS(S, ...) printf("[Deframer Tests] " S "\n", __VA_ARGS__);
#else
#define PRINT(S)
#define PRINT_ARGS(S, ...)
#endif
