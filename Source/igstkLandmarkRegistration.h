
#ifndef __igstkLandmarkRegistration_h
#define __igstkLandmarkRegistration_h

#include "igstkMacros.h"
#include "itkImage.h"
#include "itkLandmarkBasedTransformInitializer.h"
#include "itkMacro.h"
#include "igstkMacros.h"
#include "itkObject.h"

namespace igstk
{

/** \class LandmarkRegistration
 *This class is used to register the tracker coordinate system with
 *the image coordinate system. It is templated over the dimension of
 *the land mark coordinates
 */

template <unsigned int Dimension>
class LandmarkRegistration : public itk::Object
{

public:
  /** Typedefs */
  typedef LandmarkRegistration                             Self;
  typedef itk::SmartPointer<Self>                         Pointer;
  typedef itk::SmartPointer<const Self>                   ConstPointer;

  typedef  unsigned char  PixelType;
    
  typedef itk::Image< PixelType, Dimension >    TrackerImageType;
  typedef itk::Image< PixelType, Dimension >    ModalityImageType;
  //typedef itk::Rigid3DTransform< double > TransformType;
  typedef itk::VersorRigid3DTransform< double > TransformType;
  typedef typename TransformType::Pointer       TransformPointerType;

  typedef itk::LandmarkBasedTransformInitializer< TransformType, 
            TrackerImageType, ModalityImageType > TransformInitializerType;

  typedef typename TransformInitializerType::LandmarkPointContainer LandmarkPointContainerType;
  typedef typename TransformInitializerType::LandmarkPointType      LandmarkPointType;
  typedef typename TransformInitializerType::Pointer                TransformInitializerPointerType;
  typedef typename LandmarkPointContainerType::const_iterator       PointsContainerConstIterator;


  typedef typename TrackerImageType::Pointer                        TrackerImagePointerType;
  typedef typename ModalityImageType::Pointer                       ModalityImagePointerType;


  /** Method for creation of a reference counted object. */
  igstkNewMacro( Self );

  /**  Set/Get methods for landmark point containers */  

  igstkSetMacro(TrackerImageLandmarks,LandmarkPointContainerType);
  igstkSetMacro(ModalityImageLandmarks,LandmarkPointContainerType);

  igstkGetMacro(TrackerImageLandmarks,LandmarkPointContainerType);
  igstkGetMacro(ModalityImageLandmarks,LandmarkPointContainerType);

  igstkGetMacro(TransformInitializer,TransformInitializerPointerType);
  igstkGetMacro(Transform,TransformPointerType);

  /**  Set tracker and modality images */
  igstkSetMacro(TrackerImage,TrackerImagePointerType);
  igstkSetMacro(ModalityImage,ModalityImagePointerType);
  
  /* Peform registration */
  void EvaluateTransform();

protected:

  LandmarkRegistration  ( void );
  ~LandmarkRegistration ( void ) ;

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const;

private:
  TrackerImagePointerType                                m_TrackerImage;
  ModalityImagePointerType                               m_ModalityImage;
  TransformPointerType                                   m_Transform;
  TransformInitializerPointerType                        m_TransformInitializer;
  LandmarkPointContainerType                             m_TrackerImageLandmarks;
  LandmarkPointContainerType                             m_ModalityImageLandmarks;
  
};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkLandmarkRegistration.txx"
#endif

#endif // __igstkLandmarkRegistration_h
