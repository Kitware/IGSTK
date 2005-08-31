
#ifndef __igstkCTImageReader_h
#define __igstkCTImageReader_h

#include "igstkMacros.h"
#include "igstkImageReader.h"
#include "igstkDICOMImageReader.h"
#include "itkObject.h"

namespace igstk
{

/** \class CTImageReader
 * \ingroup Object
 */

class CTImageReader : public DICOMImageReader<unsigned short>
{

public:

  /** Typedefs */
  typedef CTImageReader                         Self;
  typedef DICOMImageReader<unsigned short>       Superclass;
  typedef itk::SmartPointer<Self>               Pointer;
  typedef itk::SmartPointer<const Self>         ConstPointer;


  itkStaticConstMacro(ImageDimension, unsigned int, 3);

  /**  Run-time type information (and related methods). */
  igstkTypeMacro( CTImageReader, DICOMImageReader );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( Self );

protected:

  CTImageReader( void );
  virtual ~CTImageReader( void ) {};

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

};

} // end namespace igstk

#endif // __igstkCTImageReader_h
