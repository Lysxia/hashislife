#ifndef DEFINITIONS_H
#define DEFINITIONS_H
#include <stdint.h>
/*! \defgroup rules Game rules */
/*!@{*/
//! Rule set encoded as a bit pattern
/*! 18 bits are used to indicate Born/Stay transitions */
/*! \todo Explain */
typedef uint32_t rule; // at least 18 bits
/*!@}*/

int itoa(char *dest, int src);

#endif
