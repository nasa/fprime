// deprecate.hpp:
//
// A minor implementation of compile-time deprecation for the fprime framework.

#ifndef FW_DEPRECATE_HPP
#define FW_DEPRECATE_HPP

#ifndef DEPRECATED

#ifdef __GNUC__
    #define DEPRECATED(func, message) func __attribute__ ((deprecated(message)))
#else
  #warning "No implementation of DEPRECATED for given compiler. Please check for use of DEPRECATED() functions"
  #define DEPRECATED(func) func
#endif

#endif

#endif // FW_DEPRECATE_HPP
