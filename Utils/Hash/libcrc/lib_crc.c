#include "lib_crc.h"



    /*******************************************************************\
    *                                                                   *
    *   Library         : lib_crc                                       *
    *   File            : lib_crc.c                                     *
    *   Author          : Lammert Bies  1999-2008                       *
    *   E-mail          : info@lammertbies.nl                           *
    *   Language        : ANSI C                                        *
    *                                                                   *
    *                                                                   *
    *   Description                                                     *
    *   ===========                                                     *
    *                                                                   *
    *   The file lib_crc.c contains the private  and  public  func-     *
    *   tions  used  for  the  calculation of CRC-16, CRC-CCITT and     *
    *   CRC-32 cyclic redundancy values.                                *
    *                                                                   *
    *                                                                   *
    *   Dependencies                                                    *
    *   ============                                                    *
    *                                                                   *
    *   lib_crc.h       CRC definitions and prototypes                  *
    *                                                                   *
    *                                                                   *
    *   Modification history                                            *
    *   ====================                                            *
    *                                                                   *
    *   Date        Version Comment                                     *
    *                                                                   *
    *   2008-04-20  1.16    Added CRC-CCITT calculation for Kermit      *
    *                                                                   *
    *   2007-04-01  1.15    Added CRC16 calculation for Modbus          *
    *                                                                   *
    *   2007-03-28  1.14    Added CRC16 routine for Sick devices        *
    *                                                                   *
    *   2005-12-17  1.13    Added CRC-CCITT with initial 0x1D0F         *
    *                                                                   *
    *   2005-05-14  1.12    Added CRC-CCITT with start value 0          *
    *                                                                   *
    *   2005-02-05  1.11    Fixed bug in CRC-DNP routine                *
    *                                                                   *
    *   2005-02-04  1.10    Added CRC-DNP routines                      *
    *                                                                   *
    *   1999-02-21  1.01    Added FALSE and TRUE mnemonics              *
    *                                                                   *
    *   1999-01-22  1.00    Initial source                              *
    *                                                                   *
    \*******************************************************************/



    /*******************************************************************\
    *                                                                   *
    *   #define P_xxxx                                                  *
    *                                                                   *
    *   The CRC's are computed using polynomials. The  coefficients     *
    *   for the algorithms are defined by the following constants.      *
    *                                                                   *
    \*******************************************************************/

#define                 P_16        0xA001
#define                 P_32        0xEDB88320L
#define                 P_CCITT     0x1021
#define                 P_DNP       0xA6BC
#define                 P_KERMIT    0x8408
#define                 P_SICK      0x8005



    /*******************************************************************\
    *                                                                   *
    *   static int crc_tab...init                                       *
    *   static unsigned ... crc_tab...[]                                *
    *                                                                   *
    *   The algorithms use tables with precalculated  values.  This     *
    *   speeds  up  the calculation dramatically. The first time the    *
    *   CRC function is called, the table for that specific  calcu-     *
    *   lation  is set up. The ...init variables are used to deter-     *
    *   mine if the initialization has taken place. The  calculated     *
    *   values are stored in the crc_tab... arrays.                     *
    *                                                                   *
    *   The variables are declared static. This makes them  invisi-     *
    *   ble for other modules of the program.                           *
    *                                                                   *
    \*******************************************************************/

static int              crc_tab16_init          = CRC_FALSE;
static int              crc_tab32_init          = CRC_FALSE;
static int              crc_tabccitt_init       = CRC_FALSE;
static int              crc_tabdnp_init         = CRC_FALSE;
static int              crc_tabkermit_init      = CRC_FALSE;

static unsigned short   crc_tab16[256];
static unsigned long    crc_tab32[256];
static unsigned short   crc_tabccitt[256];
static unsigned short   crc_tabdnp[256];
static unsigned short   crc_tabkermit[256];



    /*******************************************************************\
    *                                                                   *
    *   static void init_crc...tab();                                   *
    *                                                                   *
    *   Three local functions are used  to  initialize  the  tables     *
    *   with values for the algorithm.                                  *
    *                                                                   *
    \*******************************************************************/

static void             init_crc16_tab( void );
static void             init_crc32_tab( void );
static void             init_crcccitt_tab( void );
static void             init_crcdnp_tab( void );
static void             init_crckermit_tab( void );



    /*******************************************************************\
    *                                                                   *
    *   unsigned short update_crc_ccitt( unsigned long crc, char c );   *
    *                                                                   *
    *   The function update_crc_ccitt calculates  a  new  CRC-CCITT     *
    *   value  based  on the previous value of the CRC and the next     *
    *   byte of the data to be checked.                                 *
    *                                                                   *
    \*******************************************************************/

unsigned short update_crc_ccitt( unsigned short crc, char c ) {

    unsigned short tmp, short_c;

    short_c  = 0x00ff & (unsigned short) c;

    if ( ! crc_tabccitt_init ) init_crcccitt_tab();

    tmp = (crc >> 8) ^ short_c;
    crc = (unsigned short)((crc << 8) ^ crc_tabccitt[tmp]);

    return crc;

}  /* update_crc_ccitt */



    /*******************************************************************\
    *                                                                   *
    *   unsigned short update_crc_sick(                                 *
    *             unsigned long crc, char c, char prev_byte );          *
    *                                                                   *
    *   The function  update_crc_sick  calculates  a  new  CRC-SICK     *
    *   value  based  on the previous value of the CRC and the next     *
    *   byte of the data to be checked.                                 *
    *                                                                   *
    \*******************************************************************/

unsigned short update_crc_sick( unsigned short crc, char c, char prev_byte ) {

    unsigned short short_c, short_p;

    short_c  =   0x00ff & (unsigned short) c;
    short_p  =  (unsigned short)(( 0x00ff & (unsigned short) prev_byte ) << 8);

    if ( crc & 0x8000 ) crc = (unsigned short)(( crc << 1 ) ^ P_SICK);
    else                crc =   (unsigned short)(crc << 1);

    crc &= 0xffff;
    crc ^= ( short_c | short_p );

    return crc;

}  /* update_crc_sick */



    /*******************************************************************\
    *                                                                   *
    *   unsigned short update_crc_16( unsigned short crc, char c );     *
    *                                                                   *
    *   The function update_crc_16 calculates a  new  CRC-16  value     *
    *   based  on  the  previous value of the CRC and the next byte     *
    *   of the data to be checked.                                      *
    *                                                                   *
    \*******************************************************************/

unsigned short update_crc_16( unsigned short crc, char c ) {

    unsigned short tmp, short_c;

    short_c = 0x00ff & (unsigned short) c;

    if ( ! crc_tab16_init ) init_crc16_tab();

    tmp =  crc       ^ short_c;
    // Note: when masking by 0xff, range is limited to unsigned char
    //       which fits within unsigned int.
    crc = (crc >> 8) ^ crc_tab16[ (unsigned int)(tmp & 0xff) ];

    return crc;

}  /* update_crc_16 */



    /*******************************************************************\
    *                                                                   *
    *   unsigned short update_crc_kermit( unsigned short crc, char c ); *
    *                                                                   *
    *   The function update_crc_kermit calculates a  new  CRC value     *
    *   based  on  the  previous value of the CRC and the next byte     *
    *   of the data to be checked.                                      *
    *                                                                   *
    \*******************************************************************/

unsigned short update_crc_kermit( unsigned short crc, char c ) {

    unsigned short tmp, short_c;

    short_c = 0x00ff & (unsigned short) c;

    if ( ! crc_tabkermit_init ) init_crckermit_tab();

    tmp =  crc       ^ short_c;
    crc = (crc >> 8) ^ crc_tabkermit[ tmp & 0xff ];

    return crc;

}  /* update_crc_kermit */



    /*******************************************************************\
    *                                                                   *
    *   unsigned short update_crc_dnp( unsigned short crc, char c );    *
    *                                                                   *
    *   The function update_crc_dnp calculates a new CRC-DNP  value     *
    *   based  on  the  previous value of the CRC and the next byte     *
    *   of the data to be checked.                                      *
    *                                                                   *
    \*******************************************************************/

unsigned short update_crc_dnp( unsigned short crc, char c ) {

    unsigned short tmp, short_c;

    short_c = 0x00ff & (unsigned short) c;

    if ( ! crc_tabdnp_init ) init_crcdnp_tab();

    tmp =  crc       ^ short_c;
    crc = (crc >> 8) ^ crc_tabdnp[ tmp & 0xff ];

    return crc;

}  /* update_crc_dnp */



    /*******************************************************************\
    *                                                                   *
    *   unsigned long update_crc_32( unsigned long crc, char c );       *
    *                                                                   *
    *   The function update_crc_32 calculates a  new  CRC-32  value     *
    *   based  on  the  previous value of the CRC and the next byte     *
    *   of the data to be checked.                                      *
    *                                                                   *
    \*******************************************************************/

unsigned long update_crc_32( unsigned long crc, char c ) {

    unsigned long tmp, long_c;

    long_c = 0x000000ffL & (unsigned long) c;

    if ( ! crc_tab32_init ) init_crc32_tab();

    tmp = crc ^ long_c;
    crc = (crc >> 8) ^ crc_tab32[ tmp & 0xff ];

    return crc;

}  /* update_crc_32 */



    /*******************************************************************\
    *                                                                   *
    *   static void init_crc16_tab( void );                             *
    *                                                                   *
    *   The function init_crc16_tab() is used  to  fill  the  array     *
    *   for calculation of the CRC-16 with values.                      *
    *                                                                   *
    \*******************************************************************/

static void init_crc16_tab( void ) {

    int i, j;
    unsigned short crc, c;

    for (i=0; i<256; i++) {

        crc = 0;
        c   = (unsigned short) i;

        for (j=0; j<8; j++) {

            if ( (crc ^ c) & 0x0001 ) crc = ( crc >> 1 ) ^ P_16;
            else                      crc =   crc >> 1;

            c = c >> 1;
        }

        crc_tab16[i] = crc;
    }

    crc_tab16_init = CRC_TRUE;

}  /* init_crc16_tab */



    /*******************************************************************\
    *                                                                   *
    *   static void init_crckermit_tab( void );                         *
    *                                                                   *
    *   The function init_crckermit_tab() is used to fill the array     *
    *   for calculation of the CRC Kermit with values.                  *
    *                                                                   *
    \*******************************************************************/

static void init_crckermit_tab( void ) {

    int i, j;
    unsigned short crc, c;

    for (i=0; i<256; i++) {

        crc = 0;
        c   = (unsigned short) i;

        for (j=0; j<8; j++) {

            if ( (crc ^ c) & 0x0001 ) crc = ( crc >> 1 ) ^ P_KERMIT;
            else                      crc =   crc >> 1;

            c = c >> 1;
        }

        crc_tabkermit[i] = crc;
    }

    crc_tabkermit_init = CRC_TRUE;

}  /* init_crckermit_tab */



    /*******************************************************************\
    *                                                                   *
    *   static void init_crcdnp_tab( void );                            *
    *                                                                   *
    *   The function init_crcdnp_tab() is used  to  fill  the  array    *
    *   for calculation of the CRC-DNP with values.                     *
    *                                                                   *
    \*******************************************************************/

static void init_crcdnp_tab( void ) {

    int i, j;
    unsigned short crc, c;

    for (i=0; i<256; i++) {

        crc = 0;
        c   = (unsigned short) i;

        for (j=0; j<8; j++) {

            if ( (crc ^ c) & 0x0001 ) crc = ( crc >> 1 ) ^ P_DNP;
            else                      crc =   crc >> 1;

            c = c >> 1;
        }

        crc_tabdnp[i] = crc;
    }

    crc_tabdnp_init = CRC_TRUE;

}  /* init_crcdnp_tab */



    /*******************************************************************\
    *                                                                   *
    *   static void init_crc32_tab( void );                             *
    *                                                                   *
    *   The function init_crc32_tab() is used  to  fill  the  array     *
    *   for calculation of the CRC-32 with values.                      *
    *                                                                   *
    \*******************************************************************/

static void init_crc32_tab( void ) {

    int i, j;
    unsigned long crc;

    for (i=0; i<256; i++) {

        crc = (unsigned long) i;

        for (j=0; j<8; j++) {

            if ( crc & 0x00000001L ) crc = ( crc >> 1 ) ^ P_32;
            else                     crc =   crc >> 1;
        }

        crc_tab32[i] = crc;
    }

    crc_tab32_init = CRC_TRUE;

}  /* init_crc32_tab */



    /*******************************************************************\
    *                                                                   *
    *   static void init_crcccitt_tab( void );                          *
    *                                                                   *
    *   The function init_crcccitt_tab() is used to fill the  array     *
    *   for calculation of the CRC-CCITT with values.                   *
    *                                                                   *
    \*******************************************************************/

static void init_crcccitt_tab( void ) {

    int i, j;
    unsigned short crc, c;

    for (i=0; i<256; i++) {

        crc = 0;
        c   = (unsigned short)(((unsigned short) i) << 8);

        for (j=0; j<8; j++) {

            if ( (crc ^ c) & 0x8000 ) crc = (unsigned short)(( crc << 1 ) ^ P_CCITT);
            else                      crc =   (unsigned short)(crc << 1);

            c = (unsigned short)(c << 1);
        }

        crc_tabccitt[i] = crc;
    }

    crc_tabccitt_init = CRC_TRUE;

}  /* init_crcccitt_tab */
