#ifndef __igstkAffineTransformXMLFileWriter_h
#define __igstkAffineTransformXMLFileWriter_h

#include "igstkTransformXMLFileWriterBase.h"


namespace igstk
{
/** \class AffineTransformXMLFileWriter
 * 
 *  \brief This class writes a 3D affine transformation to an xml file.
 *
 *  This class is used for writing a precomputed 3D Affine transformation to an 
 *  xml file. The end result is a file with the following format:
     @verbatim
    <precomputed_transform>
   
    <description>
       US calibration
    </description>
   
    <computation_date>
       Thursday July 4 12:00:00 1776
    </computation_date>
   
    <transformation estimation_error="0.5">
       a b c d
       e f g h
       i j k l
    </transformation>
  
   </precomputed_transform>
   @endverbatim
*/

class AffineTransformXMLFileWriter : public TransformXMLFileWriterBase 
{
public:

    //standard typedefs
  typedef AffineTransformXMLFileWriter    Self;
  typedef TransformXMLFileWriterBase     Superclass;
  typedef itk::SmartPointer<Self>        Pointer;

           //run-time type information (and related methods)
  itkTypeMacro( AffineTransformXMLFileWriter, TransformXMLFileWriterBase );

         //method for creation through the object factory
  itkNewMacro( Self );

  virtual bool IsCompatible( PrecomputedTransformData::Pointer transformation );

protected:

  AffineTransformXMLFileWriter() : TransformXMLFileWriterBase() {}
  virtual ~AffineTransformXMLFileWriter() {}
  virtual void WriteTransformation( std::ofstream &out );

private:

  AffineTransformXMLFileWriter( 
    const AffineTransformXMLFileWriter & other );

  const AffineTransformXMLFileWriter & operator=( 
    const AffineTransformXMLFileWriter & right );
};


}
#endif //__igstkAffineTransformXMLFileWriter_h
