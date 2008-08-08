#include "igstkAffineTransformXMLFileReader.h"
#include <itkAffineTransform.h>

namespace igstk
{

void 
AffineTransformXMLFileReader::ProcessTransformation() 
  throw (FileFormatException )
{
  double t41, t42, t43, t44;

  itk::AffineTransform< double,3 >::TranslationType t;
  itk::AffineTransform< double,3 >::MatrixType A;
  
  std::istringstream instr;  
  instr.str( this->m_CurrentTagData );
  instr>>A(0,0)>>A(0,1)>>A(0,2)>>t[0];
  instr>>A(1,0)>>A(1,1)>>A(1,2)>>t[1];
  instr>>A(2,0)>>A(2,1)>>A(2,2)>>t[2];
  instr>>t41>>t42>>t43>>t44;

      //check that we got to the end of the stream, (assumes that the string 
      //m_CurrentTagData has no trailing white spaces)
  if( !instr.eof() )
    throw FileFormatException( "Error in transformation data, possibly non numeric values" );
      //check that we got all 16 values
  if( instr.fail() )
    throw FileFormatException( "Missing transformation data"); 
          //ensure that the last matrix row is [0 0 0 1], not used by us but 
          //important for consistancy
  if( t41!=0 || t42!=0 || t43!=0 || t44!=1 )
  {
    throw FileFormatException( "Last matrix row must be [0 0 0 1]" );
  }
  itk::AffineTransform<double,3>::Pointer affineTransform = 
    itk::AffineTransform<double,3>::New();
  affineTransform->SetMatrix( A );
  affineTransform->SetTranslation( t );
  this->m_Transform = affineTransform;
}

} //namespace
