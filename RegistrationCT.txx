
#include "RegistrationCT.h"

#include <Fl/fl_ask.H>

namespace ISIS
{


template < class TVolumeType >
RegistrationCT<TVolumeType>
::RegistrationCT()
{
  m_Metric   = MetricType::New();
  m_Notifier = itk::Object::New();
  m_Resampler = ResamplerType::New();
}



template < class TVolumeType >
RegistrationCT<TVolumeType>
::~RegistrationCT()
{
}



template < class TVolumeType >
const typename RegistrationCT<TVolumeType>::VolumeType *
RegistrationCT<TVolumeType>
::GetOutput()
{
  return m_Resampler->GetOutput();
}




template < class TVolumeType >
void
RegistrationCT<TVolumeType>
::AddObserver( itk::Command * command ) 
{
  m_Notifier->AddObserver( itk::EndEvent(), command );
}



template < class TVolumeType >
void
RegistrationCT<TVolumeType>
::SetFixedImage( const VolumeType * fixedImage )
{
  m_FixedImage = fixedImage;
  m_Metric->SetFixedImage( m_FixedImage );
}




template < class TVolumeType >
void
RegistrationCT<TVolumeType>
::SetMovingImage( const VolumeType * movingImage )
{
  m_MovingImage = movingImage;
  m_Metric->SetMovingImage( m_MovingImage );
}





}  // end namespace ISIS


