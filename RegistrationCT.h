
#ifndef __ISIS_RegistrationCT_h__
#define __ISIS_RegistrationCT_h__

#include "RegistrationCTGUI.h"
#include "itkMeanSquaresImageToImageMetric.h"


namespace ISIS
{

template< class TVolumeType >
class RegistrationCT : public RegistrationCTGUI
{
public:

  typedef TVolumeType                           VolumeType;
  typedef typename VolumeType::Pointer          VolumePointer;

  typedef itk::MeanSquaresImageToImageMetric< 
                                        VolumeType, 
                                        VolumeType 
                                            >   MetricType;

  typedef typename MetricType::Pointer          MetricPointer;

public:
  RegistrationCT( void );

  virtual ~RegistrationCT( void );

  void SetMovingImage( const VolumeType * movingImage );
  void SetFixedImage(  const VolumeType * fixedImage  );

  // Get resampled Volume
  const VolumeType * GetOutput();

  void AddObserver( itk::Command * );


protected:

  void ReadVolume();


private:

  VolumePointer             m_FixedImage;
  VolumePointer             m_MovingImage;
 
  itk::Object::Pointer      m_Notifier;

  MetricPointer             m_Metric;
};


}  // end namespace ISIS

  
#ifndef ITK_MANUAL_INSTANTIATION
#include "RegistrationCT.txx"
#endif

#endif



