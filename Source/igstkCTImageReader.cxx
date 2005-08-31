#include "igstkDICOMImageReader.h"
#include "igstkCTImageReader.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
CTImageReader::CTImageReader()
{

} 

/** Print Self function */
void CTImageReader::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk
