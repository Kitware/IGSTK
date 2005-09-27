
#ifndef __igstkLandmarkRegistration_h
#define __igstkLandmarkRegistration_h

#include "igstkStateMachine.h"
#include "igstkMacros.h"
#include "itkImage.h"
#include "itkLandmarkBasedTransformInitializer.h"
#include "itkMacro.h"
#include "igstkMacros.h"
#include "itkObject.h"
#include "itkLogger.h"
#include "itkEventObject.h"
#include "igstkEvents.h"

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
  typedef LandmarkRegistration                            Self;
  typedef itk::SmartPointer<Self>                         Pointer;
  typedef itk::SmartPointer<const Self>                   ConstPointer;
  typedef itk::Object                                     Superclass;

  /** typedef for LoggerType */
  typedef itk::Logger                    LoggerType;

  typedef  unsigned char  PixelType;
    
  typedef itk::Image< PixelType, Dimension >    ImageType;

  //typedef itk::Rigid3DTransform< double > TransformType;
  typedef itk::VersorRigid3DTransform< double > TransformType;
  typedef typename TransformType::Pointer       TransformPointerType;

  typedef itk::LandmarkBasedTransformInitializer< TransformType, 
            ImageType, ImageType > TransformInitializerType;

  typedef typename TransformInitializerType::LandmarkPointContainer LandmarkPointContainerType;
  typedef typename TransformInitializerType::LandmarkPointType      LandmarkImagePointType;
  typedef typename TransformInitializerType::LandmarkPointType      LandmarkTrackerPointType;
  typedef typename TransformInitializerType::Pointer                TransformInitializerPointerType;
  typedef typename LandmarkPointContainerType::const_iterator       PointsContainerConstIterator;

  /** Method for defining the name of the class */ 
  igstkTypeMacro(LandmarkRegistration, Object); 

  /** Method for creation of a reference counted object. */
  igstkNewMacro( Self );

  /** The "RequestAddImageLandmarkPoint" should be used to add point to the image landmark point container */
  void RequestAddImageLandmarkPoint(LandmarkImagePointType pt);
  
  /** The "RequestAddTrackerLandmarkPoint" should be used to add a point
  to the tracker landmark point container */
  void RequestAddTrackerLandmarkPoint(LandmarkImagePointType pt);

/** The "RequestResetRegistration" function should be used to start the
* registration process from the scratch. This method empties both the
* image and tracker point containers  and reset the system to idle
* state*/
  void RequestResetRegistration();

/** The "RequestComputeTransform" method should be used to request
* transform parameter calculation */
  void RequestComputeTransform();
  
  /** Declarations needed for the State Machine */
  igstkStateMachineTemplatedMacro();

/** The "GetTransform()" method returns the transform  */
  TransformPointerType GetTransform();

  /** Declarations needed for the Logger */
  igstkLoggerMacro();

  /** Set methods for the Tracker and Image landmarks */
  igstkSetMacro(TrackerLandmarks,LandmarkPointContainerType);
  igstkSetMacro(ImageLandmarks,LandmarkPointContainerType);

  /** Get methods for the Tracker and Image landmarks */
  igstkGetMacro(TrackerLandmarks,LandmarkPointContainerType);
  igstkGetMacro(ImageLandmarks,LandmarkPointContainerType);

  /** Get the Transform */
  igstkGetMacro(TransformInitializer,TransformInitializerPointerType);
  igstkGetMacro(Transform,TransformPointerType);

  /** Landmark registration events..they have to be eventually added
     to the igstkEvents class */

  itkEventMacro( TransformInitializerEvent,                IGSTKEvent);
  itkEventMacro( TransformComputationFailureEvent,         TransformInitializerEvent);
  itkEventMacro( InvalidRequestErrorEvent,                 TransformInitializerEvent );

protected:

  LandmarkRegistration  ( void );
  ~LandmarkRegistration ( void ) ;

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const;

private:
  TransformPointerType                                   m_Transform;
  TransformInitializerPointerType                        m_TransformInitializer;
  LandmarkPointContainerType                             m_TrackerLandmarks;
  LandmarkPointContainerType                             m_ImageLandmarks;
  LandmarkImagePointType                                 m_ImageLandmarkPoint;
  LandmarkTrackerPointType                               m_TrackerLandmarkPoint;
  
  
/** List of States */
  StateType                                              m_IdleState;
  StateType                                              m_ImageLandmark1AddedState;
  StateType                                              m_TrackerLandmark1AddedState;
  StateType                                              m_ImageLandmark2AddedState;
  StateType                                              m_TrackerLandmark2AddedState;
  StateType                                              m_ImageLandmark3AddedState;
  StateType                                              m_TrackerLandmark3AddedState;
  

/** List of Inputs */
  InputType                                              m_ImageLandmarkInput;
  InputType                                              m_TrackerLandmarkInput;
  InputType                                              m_ComputeTransformInput;
  InputType                                              m_ResetRegistrationInput;

/** The "AddImageLandmark" method adds landmark points to the image
 landmark point container */

  void AddImageLandmarkPoint();
  
/** The "AddTrackerLandmark" method adds landmark points to the
 tracker landmark point container */
  void AddTrackerLandmarkPoint();

/** The "ResetRegsitration" method empties the landmark point
containers to start the process again */
  void ResetRegistration();

/** The "ComputeTransform" method calculates the rigid body
  transformation parameters */
  void ComputeTransform();

/** The "ReportInvalidRequest" method throws InvalidRequestErrorEvent
 when invalid requests are made */
  void ReportInvalidRequest();
};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkLandmarkRegistration.txx"
#endif

#endif // __igstkLandmarkRegistration_h
