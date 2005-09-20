#include "igstkDICOMImageReader.h"
#include "igstkMRImageReader.h"
#include "igstkEvents.h"

namespace igstk
{ 

/** Constructor */
MRImageReader::MRImageReader()
{

} 

/** Print Self function */
void MRImageReader::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace igstk
