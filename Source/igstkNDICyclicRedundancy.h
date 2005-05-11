#ifndef __CyclicRedundancy_h__
#define __CyclicRedundancy_h__

#include <stdio.h>
#include <string.h>

namespace igstk
{
/** \class NDICyclicRedundancy
    \brief Computation of Cyclic Redundancy

*/

class NDICyclicRedundancy
{
#define CARRIAGE_RETURN    0xD

public:

  NDICyclicRedundancy( void );

  ~NDICyclicRedundancy( void );

  // This method checks if the input string "str" has the correct cyclic redundancy.
  // Return "true" if found correct.
  bool ValidateCyclicRedundancyOfReplyMessage(char *str, const int maxStringLength);

  int AppendCyclicRedundancyTermToCommand( char * pszCommandString, const unsigned int maxCommandLength );

  inline unsigned int ComputeCyclicRedundancyOfTextData( unsigned int crc, int data );

  unsigned int ComputeCyclicRedundancyOfStringInBinaryMode( const char *str, int len );

  // This method gets the cyclic redundancy value for the "length" of the string "str".
  unsigned int ComputeCyclicRedundancyOfStringInTextMode(const char *str, int length);

private:

  unsigned int CyclicRedundancyTable[256];
  
  unsigned char OddParity[16] ; 
};

}
#endif
