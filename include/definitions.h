#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <stdint.h>

/*! \defgroup rules Game rules */
/*!@{*/
//! Rule set encoded as a bit pattern
/*! 18 bits are used to indicate Born/Stay transitions */
/*! \todo Explain */
typedef uint32_t rule; // at least 18 bits

#define CONWAY ((rule) 6152) // "b3/s23";
/*!@}*/

char digit_to_char(int d);
int itoa(char *dest, int src, int base);

#define ALIVE_CELL_CHAR 'O'
#define  DEAD_CELL_CHAR '.'

#endif
