#ifndef DATAPRODUCTSSUBTOPOLOGY_DEFS_HPP
#define DATAPRODUCTSSUBTOPOLOGY_DEFS_HPP 

namespace DataProducts {
    struct DataProductsState {
        /* include any variables that are needed for 
        configuring/starting/tearing down the topology */
    };
}

  namespace PingEntries {
    namespace DataProducts_dpCat          {enum { WARN = 3, FATAL = 5 };}
  }

#endif