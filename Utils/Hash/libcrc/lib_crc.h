    /*******************************************************************\
    *                                                                   *
    *   Library         : lib_crc                                       *
    *   File            : lib_crc.h                                     *
    *   Author          : Lammert Bies  1999-2008                       *
    *   E-mail          : info@lammertbies.nl                           *
    *   Language        : ANSI C                                        *
    *                                                                   *
    *                                                                   *
    *   Description                                                     *
    *   ===========                                                     *
    *                                                                   *
    *   The file lib_crc.h contains public definitions  and  proto-     *
    *   types for the CRC functions present in lib_crc.c.               *
    *                                                                   *
    *                                                                   *
    *   Dependencies                                                    *
    *   ============                                                    *
    *                                                                   *
    *   none                                                            *
    *                                                                   *
    *                                                                   *
    *   Modification history                                            *
    *   ====================                                            *
    *                                                                   *
    *   Date        Version Comment                                     *
    *                                                                   *
    *   2008-04-20  1.16    Added CRC-CCITT routine for Kermit          *
    *                                                                   *
    *   2007-04-01  1.15    Added CRC16 calculation for Modbus          *
    *                                                                   *
    *   2007-03-28  1.14    Added CRC16 routine for Sick devices        *
    *                                                                   *
    *   2005-12-17  1.13    Added CRC-CCITT with initial 0x1D0F         *
    *                                                                   *
    *   2005-02-14  1.12    Added CRC-CCITT with initial 0x0000         *
    *                                                                   *
    *   2005-02-05  1.11    Fixed bug in CRC-DNP routine                *
    *                                                                   *
    *   2005-02-04  1.10    Added CRC-DNP routines                      *
    *                                                                   *
    *   2005-01-07  1.02    Changes in tst_crc.c                        *
    *                                                                   *
    *   1999-02-21  1.01    Added FALSE and TRUE mnemonics              *
    *                                                                   *
    *   1999-01-22  1.00    Initial source                              *
    *                                                                   *
    \*******************************************************************/

#ifndef UTILS_HASH_LIB_CRC_HPP
#define UTILS_HASH_LIB_CRC_HPP

#ifdef  __cplusplus
extern "C" {
#endif

#define CRC_VERSION     "1.16"



#define CRC_FALSE           0
#define CRC_TRUE            1



unsigned short          update_crc_16(     unsigned short crc, char c                 );
unsigned long           update_crc_32(     unsigned long  crc, char c                 );
unsigned short          update_crc_ccitt(  unsigned short crc, char c                 );
unsigned short          update_crc_dnp(    unsigned short crc, char c                 );
unsigned short          update_crc_kermit( unsigned short crc, char c                 );
unsigned short          update_crc_sick(   unsigned short crc, char c, char prev_byte );

#ifdef  __cplusplus
}
#endif

#endif // UTILS_HASH_LIB_CRC_HPP
