
#ifndef __ISIS_RegistrationCT_h__
#define __ISIS_RegistrationCT_h__

#include "RegistrationCTGUI.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkResampleImageFilter.h"


namespace ISIS
{

template< class TVolumeType >
class RegistrationCT : public RegistrationCTGUI
{
public:

  typedef TVolumeType                           VolumeType;
  typedef typename VolumeType::Pointer          VolumePointer;
  typedef typename VolumeType::ConstPointer     VolumeConstPointer;

  typedef itk::MeanSquaresImageToImageMetric< 
                                        VolumeType, 
                                        VolumeType 
                                            >   MetricType;

  typedef typename MetricType::Pointer          MetricPointer;

  typedef itk::ResampleImageFilter< 
                              VolumeType, 
                              VolumeType 
                                          >  ResamplerType;

  typedef typename ResamplerType::Pointer    ResamplerPointer;

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

  VolumeConstPointer        m_FixedImage;
  VolumeConstPointer        m_MovingImage;
 
  itk::Object::Pointer      m_Notifier;

  MetricPointer             m_Metric;
  
  ResamplerPointer          m_Resampler;

};


}  // end namespace ISIS

  
#ifndef ITK_MANUAL_INSTANTIATION
#include "RegistrationCT.txx"
#endif

#endif



