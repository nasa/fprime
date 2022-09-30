
#include <getopt.h>
#include <cstdlib>
#include <ctype.h>
#include <stdint.h>
#include <cstdio>
namespace Ref {
    void Initialize(const char *hostname, uint32_t port_number);
    void Deinitialize(); 
    void run_one_cycle(); // Allows to emulate an interrupt to obtain a reference clock.
}