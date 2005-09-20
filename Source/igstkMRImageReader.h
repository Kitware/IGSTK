
#ifndef __igstkMRImageReader_h
#define __igstkMRImageReader_h

#include "igstkMacros.h"
#include "igstkImageReader.h"
#include "igstkDICOMImageReader.h"
#include "itkObject.h"

namespace igstk
{

/** \class MRImageReader:
*  MRI image reader
*/

class MRImageReader : public DICOMImageReader<unsigned short>
{

public:

  /** Typedefs */
  typedef MRImageReader                         Self;
  typedef DICOMImageReader<unsigned short>       Superclass;
  typedef itk::SmartPointer<Self>               Pointer;
  typedef itk::SmartPointer<const Self>         ConstPointer;


  itkStaticConstMacro(ImageDimension, unsigned int, 3);

  /**  Run-time type information (and related methods). */
  igstkTypeMacro( MRImageReader, DICOMImageReader );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( Self );

protected:

  MRImageReader( void );
  virtual ~MRImageReader( void ) {};

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

};

} // end namespace igstk

#endif // __igstkMRImageReader_h
