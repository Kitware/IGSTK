
#include "RegistrationCTBase.h"

#include <string>


namespace ISIS
{


RegistrationCTBase
::RegistrationCTBase()
{
  m_Optimizer = OptimizerType::New();
  m_Transform = TransformType::New();
}



RegistrationCTBase
::~RegistrationCTBase()
{
}



}  // end namespace ISIS


