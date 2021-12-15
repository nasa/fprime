#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib_crc.h"



    /*******************************************************************\
    *                                                                   *
    *   Library         : lib_crc                                       *
    *   File            : tst_crc.c                                     *
    *   Author          : Lammert Bies  1999-2008                       *
    *   E-mail          : info@lammertbies.nl                           *
    *   Language        : ANSI C                                        *
    *                                                                   *
    *                                                                   *
    *   Description                                                     *
    *   ===========                                                     *
    *                                                                   *
    *   The file tst_crc.c contains a small  sample  program  which     *
    *   demonstrates  the  use of the functions for calculating the     *
    *   CRC-CCITT, CRC-16 and CRC-32 values of data. The file  cal-     *
    *   culates  the three different CRC's for a file whose name is     *
    *   either provided at the command  line,  or  typed  in  right     *
    *   after the program has started.                                  *
    *                                                                   *
    *                                                                   *
    *   Dependencies                                                    *
    *   ============                                                    *
    *                                                                   *
    *   lib_crc.h       CRC definitions and prototypes                  *
    *   lib_crc.c       CRC routines                                    *
    *                                                                   *
    *                                                                   *
    *   Modification history                                            *
    *   ====================                                            *
    *                                                                   *
    *   Date        Version Comment                                     *
    *                                                                   *
    *   2008-04-20  1.16    Added CRC-CCITT calculation for Kermit.     *
    *                                                                   *
    *   2007-05-01  1.15    Added CRC16 calculation for Modbus.         *
    *                                                                   *
    *   2007-03-28  1.14    Added CRC16 routine for  Sick  devices,     *
    *                       electronic devices used for measurement     *
    *                       and detection in industrial situations.     *
    *                                                                   *
    *   2005-12-17  1.13    Added CRC-CCITT with initial 0x1D0F         *
    *                                                                   *
    *   2005-02-14  1.12    Added CRC-CCITT with initial 0x0000         *
    *                                                                   *
    *   2005-02-05  1.11    Fixed post processing bug in CRC-DNP.       *
    *                                                                   *
    *   2005-02-04  1.10    Added the CRC calculation for  DNP 3.0,     *
    *                       a protocol used  in  the  communication     *
    *                       between remote units and masters in the     *
    *                       electric utility industry.  The  method     *
    *                       of  calculation  is the same as CRC-16,     *
    *                       but with a different polynomial.            *
    *                                                                   *
    *   2005-01-07  1.02    Changed way program  is  used.  When  a     *
    *                       commandline  parameter  is present, the     *
    *                       program assumes it is a file, but  when     *
    *                       invoked without a parameter the entered     *
    *                       string is used to calculate the CRC.        *
    *                                                                   *
    *                       CRC's are now  printed  in  hexadecimal     *
    *                       decimal format.                             *
    *                                                                   *
    *                       Let  CRC-CCITT  calculation  start with     *
    *                       0xffff as this is used in  most  imple-     *
    *                       mentations.                                 *
    *                                                                   *
    *   1999-02-21  1.01    none                                        *
    *                                                                   *
    *   1999-01-22  1.00    Initial source                              *
    *                                                                   *
    \*******************************************************************/

#define MAX_STRING_SIZE	2048



void main( int argc, char *argv[] ) {

    char input_string[MAX_STRING_SIZE];
    char *ptr, *dest, hex_val, prev_byte;
    unsigned short crc_16, crc_16_modbus, crc_ccitt_ffff, crc_ccitt_0000, crc_ccitt_1d0f, crc_dnp, crc_sick, crc_kermit;
    unsigned short low_byte, high_byte;
    unsigned long crc_32;
    int a, ch, do_ascii, do_hex;
    FILE *fp;

    do_ascii = CRC_FALSE;
    do_hex   = CRC_FALSE;

    printf( "\nCRC algorithm sample program\nLammert Bies,  Version " CRC_VERSION "\n\n" );

    if ( argc < 2 ) {

        printf( "Usage: tst_crc [-a|-x] file1 ...\n\n" );
        printf( "    -a Program asks for ASCII input. Following parameters ignored.\n" );
        printf( "    -x Program asks for hexadecimal input. Following parameters ignored.\n" );
        printf( "       All other parameters are treated like filenames. The CRC values\n" );
        printf( "       for each separate file will be calculated.\n" );

        exit( 0 );
    }

    if ( ! strcmp( argv[1], "-a" )  ||  ! strcmp( argv[1], "-A" ) ) do_ascii = CRC_TRUE;
    if ( ! strcmp( argv[1], "-x" )  ||  ! strcmp( argv[1], "-X" ) ) do_hex   = CRC_TRUE;

    if ( do_ascii  ||  do_hex ) {

        printf( "Input: " );
        fgets( input_string, MAX_STRING_SIZE-1, stdin );
    }

    if ( do_ascii ) {

        ptr = input_string;
        while ( *ptr  &&  *ptr != '\r'  &&  *ptr != '\n' ) ptr++;
        *ptr = 0;
    }

    if ( do_hex ) {

        ptr  = input_string;
        dest = input_string;

        while( *ptr  &&  *ptr != '\r'  &&  *ptr != '\n' ) {

            if ( *ptr >= '0'  &&  *ptr <= '9' ) *dest++ = (char) ( (*ptr) - '0'      );
            if ( *ptr >= 'A'  &&  *ptr <= 'F' ) *dest++ = (char) ( (*ptr) - 'A' + 10 );
            if ( *ptr >= 'a'  &&  *ptr <= 'f' ) *dest++ = (char) ( (*ptr) - 'a' + 10 );

            ptr++;
	}

        * dest    = '\x80';
        *(dest+1) = '\x80';
    }



    a = 1;

    do {

        crc_16         = 0;
        crc_16_modbus  = 0xffff;
        crc_dnp        = 0;
        crc_sick       = 0;
        crc_ccitt_0000 = 0;
        crc_ccitt_ffff = 0xffff;
        crc_ccitt_1d0f = 0x1d0f;
        crc_kermit     = 0;
        crc_32         = 0xffffffffL;



        if ( do_ascii ) {

            prev_byte = 0;
            ptr       = input_string;

            while ( *ptr ) {

                crc_16         = update_crc_16(     crc_16,         *ptr            );
                crc_16_modbus  = update_crc_16(     crc_16_modbus,  *ptr            );
                crc_dnp        = update_crc_dnp(    crc_dnp,        *ptr            );
                crc_sick       = update_crc_sick(   crc_sick,       *ptr, prev_byte );
                crc_ccitt_0000 = update_crc_ccitt(  crc_ccitt_0000, *ptr            );
                crc_ccitt_ffff = update_crc_ccitt(  crc_ccitt_ffff, *ptr            );
                crc_ccitt_1d0f = update_crc_ccitt(  crc_ccitt_1d0f, *ptr            );
                crc_kermit     = update_crc_kermit( crc_kermit,     *ptr            );
                crc_32         = update_crc_32(     crc_32,         *ptr            );

                prev_byte = *ptr;
                ptr++;
            }
        }



        else if ( do_hex ) {

            prev_byte = 0;
            ptr       = input_string;

            while ( *ptr != '\x80' ) {

                hex_val  = (char) ( ( * ptr     &  '\x0f' ) << 4 );
                hex_val |= (char) ( ( *(ptr+1)  &  '\x0f' )      );

                crc_16         = update_crc_16(     crc_16,         hex_val            );
                crc_16_modbus  = update_crc_16(     crc_16_modbus,  hex_val            );
                crc_dnp        = update_crc_dnp(    crc_dnp,        hex_val            );
                crc_sick       = update_crc_sick(   crc_sick,       hex_val, prev_byte );
                crc_ccitt_0000 = update_crc_ccitt(  crc_ccitt_0000, hex_val            );
                crc_ccitt_ffff = update_crc_ccitt(  crc_ccitt_ffff, hex_val            );
                crc_ccitt_1d0f = update_crc_ccitt(  crc_ccitt_1d0f, hex_val            );
                crc_kermit     = update_crc_kermit( crc_kermit,     hex_val            );
                crc_32         = update_crc_32(     crc_32,         hex_val            );

                prev_byte = hex_val;
                ptr      += 2;
            }

            input_string[0] = 0;
        }



        else {

            prev_byte = 0;
            fp        = fopen( argv[a], "rb" );

            if ( fp != nullptr ) {

                while( ( ch=fgetc( fp ) ) != EOF ) {

                    crc_16         = update_crc_16(     crc_16,         (char) ch            );
                    crc_16_modbus  = update_crc_16(     crc_16_modbus,  (char) ch            );
                    crc_dnp        = update_crc_dnp(    crc_dnp,        (char) ch            );
                    crc_sick       = update_crc_sick(   crc_sick,       (char) ch, prev_byte );
                    crc_ccitt_0000 = update_crc_ccitt(  crc_ccitt_0000, (char) ch            );
                    crc_ccitt_ffff = update_crc_ccitt(  crc_ccitt_ffff, (char) ch            );
                    crc_ccitt_1d0f = update_crc_ccitt(  crc_ccitt_1d0f, (char) ch            );
                    crc_kermit     = update_crc_kermit( crc_kermit,     (char) ch            );
                    crc_32         = update_crc_32(     crc_32,         (char) ch            );

                    prev_byte = (char) ch;
                }

                fclose( fp );
            }

            else printf( "%s : cannot open file\n", argv[a] );
        }



        crc_32    ^= 0xffffffffL;

        crc_dnp    = ~crc_dnp;
        low_byte   = (crc_dnp & 0xff00) >> 8;
        high_byte  = (crc_dnp & 0x00ff) << 8;
        crc_dnp    = low_byte | high_byte;

        low_byte   = (crc_sick & 0xff00) >> 8;
        high_byte  = (crc_sick & 0x00ff) << 8;
        crc_sick   = low_byte | high_byte;

        low_byte   = (crc_kermit & 0xff00) >> 8;
        high_byte  = (crc_kermit & 0x00ff) << 8;
        crc_kermit = low_byte | high_byte;

        printf( "%s%s%s :\nCRC16              = 0x%04X      /  %u\n"
                          "CRC16 (Modbus)     = 0x%04X      /  %u\n"
                          "CRC16 (Sick)       = 0x%04X      /  %u\n"
                          "CRC-CCITT (0x0000) = 0x%04X      /  %u\n"
                          "CRC-CCITT (0xffff) = 0x%04X      /  %u\n"
                          "CRC-CCITT (0x1d0f) = 0x%04X      /  %u\n"
                          "CRC-CCITT (Kermit) = 0x%04X      /  %u\n"
                          "CRC-DNP            = 0x%04X      /  %u\n"
                          "CRC32              = 0x%08lX  /  %lu\n"
                    , (   do_ascii  ||    do_hex ) ? "\""    : ""
                    , ( ! do_ascii  &&  ! do_hex ) ? argv[a] : input_string
                    , (   do_ascii  ||    do_hex ) ? "\""    : ""
                    , crc_16,         crc_16
                    , crc_16_modbus,  crc_16_modbus
                    , crc_sick,       crc_sick
                    , crc_ccitt_0000, crc_ccitt_0000
                    , crc_ccitt_ffff, crc_ccitt_ffff
                    , crc_ccitt_1d0f, crc_ccitt_1d0f
                    , crc_kermit,     crc_kermit
                    , crc_dnp,        crc_dnp
                    , crc_32,         crc_32     );

        a++;

    } while ( a < argc );

}  /* main (tst_crc.c) */
