/*
 * PrmDblImplCfg.hpp
 *
 *  Created on: Mar 13, 2015
 *      Author: tcanham
 */

#ifndef PRMDB_PRMDBLIMPLCFG_HPP_
#define PRMDB_PRMDBLIMPLCFG_HPP_

// Anonymous namespace for configuration parameters
namespace {

    enum {
        PRMDB_NUM_DB_ENTRIES = 25, // !< Number of entries in the parameter database
        PRMDB_ENTRY_DELIMETER = 0xA5 // !< Byte value that should precede each parameter in file; sanity check against file integrity. Should match ground system.
    };

}



#endif /* PRMDB_PRMDBLIMPLCFG_HPP_ */
