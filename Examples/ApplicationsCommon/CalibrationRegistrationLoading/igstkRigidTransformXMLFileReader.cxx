#include "igstkRigidTransformXMLFileReader.h"

#include "igstkTransform.h"

namespace igstk
{

void 
RigidTransformXMLFileReader::ProcessTransformation() 
  throw ( FileFormatException )
{
  double qx, qy, qz, qw;
  igstk::Transform::VectorType t;
  igstk::Transform::VersorType q;
  
  const double eps = 0.1;

  std::istringstream instr;  
  instr.str( this->m_CurrentTagData );
  instr>>qx>>qy>>qz>>qw>>t[0]>>t[1]>>t[2];
              
      //check that we got to the end of the stream, (assumes that the string 
      //m_CurrentTagData has no trailing white spaces)
  if( !instr.eof() )
    {
    throw FileFormatException( 
      "Error in transformation data, possibly non numeric values" );
    }
  //check that we got all seven values
  if( instr.fail() )
    throw FileFormatException( "Missing transformation data" ); 
          //check that the quaternion is a versor (unit norm)
  if( fabs( qx*qx + qy*qy + qz*qz + qw*qw - 1 ) > eps )
    {
    throw FileFormatException( 
      "Quaternion entries do not define a rotation (norm not equal one)." );
    }

  q.Set(qx, qy, qz, qw);

  delete this->m_Transform;
  Transform *rigidTransform = new Transform();  
                 //we already have the estimation error
  rigidTransform->SetTranslationAndRotation( 
    t, q, 
    this->m_EstimationError, 
    itk::NumericTraits< TimeStamp::TimePeriodType >::max() );
  this->m_Transform = rigidTransform;
}

} //namespace
