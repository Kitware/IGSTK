#include "igstkPerspectiveTransformXMLFileReader.h"
#include "igstkPerspectiveTransform.h"

namespace igstk
{

void 
PerspectiveTransformXMLFileReader::ProcessTransformation() 
  throw (FileFormatException )
{
  igstk::PerspectiveTransform::IntrinsicMatrixType K;
  igstk::PerspectiveTransform::ExtrinsicMatrixType Rt;
  
  std::istringstream instr;  
  instr.str( this->m_CurrentTagData );
  instr>>K(0,0)>>K(0,1)>>K(0,2);
  instr>>K(1,0)>>K(1,1)>>K(1,2);
  instr>>K(2,0)>>K(2,1)>>K(2,2);
  instr>>Rt(0,0)>>Rt(0,1)>>Rt(0,2)>>Rt(0,3);
  instr>>Rt(1,0)>>Rt(1,1)>>Rt(1,2)>>Rt(1,3);
  instr>>Rt(2,0)>>Rt(2,1)>>Rt(2,2)>>Rt(2,3);

  //check that we got to the end of the stream, (assumes that the string 
  //m_CurrentTagData has no trailing white spaces)
  if( !instr.eof() )
    {
    throw FileFormatException( 
      "Error in transformation data, possibly non numeric values" );
    }
  //check that we got all 12 values
  if( instr.fail() )
    {
    throw FileFormatException( "Missing transformation data"); 
    }
  delete this->m_Transform;
  PerspectiveTransform *perspectiveTransform = new PerspectiveTransform();

  perspectiveTransform->SetTransform( 
    Rt, K,
    this->m_EstimationError, 
    itk::NumericTraits< TimeStamp::TimePeriodType >::max() );
  this->m_Transform = perspectiveTransform;
}

} //namespace
