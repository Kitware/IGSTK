#ifndef __igstkLandmarkRegistration_txx
#define __igstkLandmarkRegistration_txx

#include "igstkLandmarkRegistration.h"

namespace igstk
{ 

/** Constructor */
template <unsigned int TDimension>
LandmarkRegistration<TDimension>::LandmarkRegistration()
{
  m_Transform = TransformType::New();
  m_TransformInitializer = TransformInitializerType::New();
} 

/** Destructor */
template <unsigned int TDimension>
LandmarkRegistration<TDimension>::~LandmarkRegistration()  
{

}

/** Register the fixed and moving coordinates */
template <unsigned int TDimension>
void LandmarkRegistration<TDimension>::EvaluateTransform()  
{
  m_TransformInitializer->SetFixedLandmarks(m_TrackerImageLandmarks);
  m_TransformInitializer->SetMovingLandmarks(m_ModalityImageLandmarks);
  m_TransformInitializer->SetTransform( m_Transform );
  m_TransformInitializer->InitializeTransform(); 
}

/** Print Self function */
template <unsigned int TDimension>
void LandmarkRegistration<TDimension>::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "TrackerImage Landmarks: " << std::endl;
  PointsContainerConstIterator fitr = m_TrackerImageLandmarks.begin();
  while( fitr != m_TrackerImageLandmarks.end() )
    {
    os << indent << *fitr << std::endl;
    ++fitr;
    }
  os << indent << "ModalityImage Landmarks: " << std::endl;
  PointsContainerConstIterator mitr = m_ModalityImageLandmarks.begin();
  while( mitr != m_ModalityImageLandmarks.end() )
    {
    os << indent << *mitr << std::endl;
    ++mitr;
    }
}

} // end namespace igstk

#endif


