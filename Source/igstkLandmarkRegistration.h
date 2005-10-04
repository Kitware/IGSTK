/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLandmarkRegistration.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkLandmarkRegistration_h
#define __igstkLandmarkRegistration_h

#include "igstkStateMachine.h"
#include "igstkEvents.h"
#include "igstkMacros.h"

#include "itkImage.h"
#include "itkLandmarkBasedTransformInitializer.h"

namespace igstk
{

/** \class LandmarkRegistration
 * \brief This class evaluates the transformation parameters between 
 * the tracker and image coordinate systems.
 *  
 * The Landmark registration class is used to register the tracker
 * coordinate system with the image coordinate system using rigid
 * body transformation. Landmark points are used to solve for the
 * transform parameters. A minimum of three pair of landmarks are
 * required. The class is basically a wrapper around the
 * itk::LandmarkBasedTransformInitializer.
 * The class is templated over the dimension of the landmark coordinates.
 *
 *  \image html  igstkLandmarkRegistration.png  "LandmarkRegistration State Machine Diagram"
 *  \image latex igstkLandmarkRegistration.eps  "LandmarkRegistration State Machine Diagram" 
 *
 * \ingroup Registration 
 */

template <unsigned int Dimension>
class LandmarkRegistration : public ::itk::Object
{

public:
  /** Typedefs */
  typedef LandmarkRegistration                Self;
  typedef itk::SmartPointer<Self>             Pointer;
  typedef itk::SmartPointer<const Self>       ConstPointer;
  typedef ::itk::Object                       Superclass;

  
  /** typedef for LoggerType */
  typedef itk::Logger                         LoggerType;
  
  /** typedefs for image and pixel type */
  typedef  unsigned char  PixelType;
  typedef itk::Image< PixelType, Dimension >  ImageType;

  /** typedefs for the transform types */ 
  typedef itk::VersorRigid3DTransform< double > TransformType;
  typedef typename TransformType::Pointer     TransformPointerType;
  
  typedef itk::LandmarkBasedTransformInitializer< TransformType, 
            ImageType, ImageType > TransformInitializerType;
  
  typedef typename TransformInitializerType::LandmarkPointContainer 
                                              LandmarkPointContainerType;
  
  typedef typename TransformInitializerType::LandmarkPointType      
                                              LandmarkImagePointType;
  
  typedef typename TransformInitializerType::LandmarkPointType    
                                              LandmarkTrackerPointType;
  
  typedef typename TransformInitializerType::Pointer  
                                              TransformInitializerPointerType;
  
  typedef typename LandmarkPointContainerType::const_iterator
                                              PointsContainerConstIterator;

  /** Method for defining the name of the class */ 
  igstkTypeMacro( LandmarkRegistration, Object ); 

  /** Method for creation of a reference counted object. */
  igstkNewMacro( Self );

  /** The "RequestAddImageLandmarkPoint" will be used to add point 
   * to the image landmark point container */
  void RequestAddImageLandmarkPoint( const LandmarkImagePointType & pt );
  
  /** The "RequestAddTrackerLandmarkPoint" will be used to add a point
   * to the tracker landmark point container */
  void RequestAddTrackerLandmarkPoint( const LandmarkImagePointType & pt );

  /** The "RequestResetRegistration" function will be used to start the
   * registration process from the scratch. This method empties both the
   * image and tracker point containers  and reset the system to idle
   * state*/
  void RequestResetRegistration();

  /** The "RequestComputeTransform" method will be used to request
  * transform parameter calculation */
  void RequestComputeTransform();

  /** The "RequesteGetTransform" method will be used to request to get
      transform */
   void RequestGetTransform();
  
  /** Declarations needed for the State Machine */
  igstkStateMachineTemplatedMacro();

  /** Declarations needed for the Logger */
  igstkLoggerMacro();

  /** Landmark registration events..they have to be eventually added
   * to the igstkEvents class */
  itkEventMacro( TransformInitializerEvent,        IGSTKEvent );
  itkEventMacro( TransformComputationFailureEvent, TransformInitializerEvent );
  itkEventMacro( TransformComputationSuccessEvent,         TransformInitializerEvent);
  itkEventMacro( InvalidRequestErrorEvent,         TransformInitializerEvent );

protected:

  LandmarkRegistration  ( void );
  ~LandmarkRegistration ( void ) ;

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const;

private:
  
  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  LandmarkRegistration(const Self&);    //purposely not implemented
  void operator=(const Self&);          //purposely not implemented


  TransformPointerType                     m_Transform;
  TransformInitializerPointerType          m_TransformInitializer;
  LandmarkPointContainerType               m_TrackerLandmarks;
  LandmarkPointContainerType               m_ImageLandmarks;
  LandmarkImagePointType                   m_ImageLandmarkPoint;
  LandmarkTrackerPointType                 m_TrackerLandmarkPoint;
  
  
  /** List of States */
  StateType                                m_IdleState;
  StateType                                m_ImageLandmark1AddedState;
  StateType                                m_TrackerLandmark1AddedState;
  StateType                                m_ImageLandmark2AddedState;
  StateType                                m_TrackerLandmark2AddedState;
  StateType                                m_ImageLandmark3AddedState;
  StateType                                m_TrackerLandmark3AddedState;
  StateType                                m_TransformComputedState; 


  /** List of Inputs */
  InputType                                m_ImageLandmarkInput;
  InputType                                m_TrackerLandmarkInput;
  InputType                                m_ComputeTransformInput;
  InputType                                m_GetTransformInput;
  InputType                                m_ResetRegistrationInput;
  InputType                                m_TransformComputationFailureInput;
  InputType                                m_TransformComputationSuccessInput;

  /** The "AddImageLandmark" method adds landmark points to the image
   * landmark point container */

  void AddImageLandmarkPoint();
  
  /** The "AddTrackerLandmark" method adds landmark points to the
   * tracker landmark point container */
  void AddTrackerLandmarkPoint();

  /** The "ResetRegsitration" method empties the landmark point
   * containers to start the process again */
  void ResetRegistration();

  /** The "ComputeTransform" method calculates the rigid body
   * transformation parameters */
  void ComputeTransform();

  /** The "GetTransform" method throws an event containing the transform parameters*/
  void GetTransform();

  /** The "ReportInvalidRequest" method throws InvalidRequestErrorEvent
   * when invalid requests are made */
  void ReportInvalidRequest();

  /** The "ReportSuccessInTransformComputation" method throws TransformComputationSuccessEvent*/
  void ReportSuccessInTransformComputation();

  /** The "ReportFailureInTransformComputation" method throws TransformComputationFailureEvent*/
  void ReportFailureInTransformComputation();
};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkLandmarkRegistration.txx"
#endif

#endif // __igstkLandmarkRegistration_h

