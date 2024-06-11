// ======================================================================
// \title  bsd_random.h
// \author See bsd_random.c
// \brief  BSD random number generator
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================


#include <sys/cdefs.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void bsd_srandom(unsigned x);

char *bsd_initstate(unsigned seed, char *arg_state, size_t n);

char *bsd_setstate(const char *arg_state);

long bsd_random(void);
