
#include "DicomImageReaderBase.h"

#include <string>


namespace ISIS
{


DicomImageReaderBase
::DicomImageReaderBase()
{
  m_FilenamesGenerator = SeriesFileNamesType::New();
}



DicomImageReaderBase
::~DicomImageReaderBase()
{
}


void
DicomImageReaderBase
::SetDirectory( const char * directory )
{
   std::string directoryName = directory;
   m_FilenamesGenerator->SetDirectory( directoryName );
}



}  // end namespace ISIS


