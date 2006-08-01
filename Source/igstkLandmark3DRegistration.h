/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLandmark3DRegistration.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkLandmark3DRegistration_h
#define __igstkLandmark3DRegistration_h

#include "igstkStateMachine.h"
#include "igstkEvents.h"
#include "igstkMacros.h"
#include "igstkObject.h"

#include "itkImage.h"
#include "itkLandmarkBasedTransformInitializer.h"


namespace igstk
{
/** \class Landmark3DRegistration
 * \brief This class evaluates the transformation parameters between 
 * the tracker and image coordinate systems.
 *  
 * The Landmark registration class is used to register the tracker
 * coordinate system with the image coordinate system using rigid
 * body transformation. Landmark points are used to solve for the
 * transform parameters. A minimum of three pair of landmarks are
 * required. The class is basically a wrapper around the
 * itk::LandmarkBasedTransformInitializer.
 *
 *\image html igstkLandmark3DRegistration.png "State Machine Diagram"
 *
 *\image latex igstkLandmark3DRegistration.eps "State Machine Diagram" 
 *
 * \ingroup Registration 
 */

class Landmark3DRegistration : public Object
{

public:
  
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( Landmark3DRegistration, Object )

public:

  /** Typedefs */
  itkStaticConstMacro(Dimension,unsigned int,3);

  /** typedefs for image and pixel type */
  typedef  unsigned char                      PixelType;
  typedef itk::Image< PixelType, Dimension >  ImageType;

  /** typedefs for the transform types */ 
  typedef itk::VersorRigid3DTransform< double > TransformType;
  typedef TransformType::Pointer                TransformPointerType;
  
  typedef itk::LandmarkBasedTransformInitializer< TransformType, 
            ImageType, ImageType > TransformInitializerType;
  
  typedef TransformInitializerType::LandmarkPointContainer 
                                              LandmarkPointContainerType;
  
  typedef TransformInitializerType::LandmarkPointType LandmarkImagePointType;
  
  typedef TransformInitializerType::LandmarkPointType LandmarkTrackerPointType;
  
  typedef TransformInitializerType::Pointer   TransformInitializerPointerType;
  
  typedef LandmarkPointContainerType::const_iterator
                                              PointsContainerConstIterator;

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
   *   transform */
  void RequestGetTransform();
  
  /** The "ComputeRMSError" method calculates and returns RMS error of
   *  the transformation. This function should be moved into the 
   *  state machine: FIXME */
  double ComputeRMSError();

  
  /** Landmark registration events..they have to be eventually added
   * to the igstkEvents class */
  itkEventMacro( TransformInitializerEvent,       IGSTKEvent );
  itkEventMacro( TransformComputationFailureEvent,TransformInitializerEvent );
  itkEventMacro( TransformComputationSuccessEvent,TransformInitializerEvent);
  itkEventMacro( InvalidRequestErrorEvent,        TransformInitializerEvent );

protected:

  Landmark3DRegistration  ( void );
  ~Landmark3DRegistration ( void );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const;

private:
 
  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  Landmark3DRegistration(const Self&);    //purposely not implemented
  void operator=(const Self&);          //purposely not implemented


  TransformPointerType                     m_Transform;
  TransformInitializerPointerType          m_TransformInitializer;
  LandmarkPointContainerType               m_TrackerLandmarks;
  LandmarkPointContainerType               m_ImageLandmarks;
  LandmarkImagePointType                   m_ImageLandmarkPoint;
  LandmarkTrackerPointType                 m_TrackerLandmarkPoint;
   
  
  /** List of States */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( ImageLandmark1Added );
  igstkDeclareStateMacro( TrackerLandmark1Added );
  igstkDeclareStateMacro( ImageLandmark2Added );
  igstkDeclareStateMacro( TrackerLandmark2Added );
  igstkDeclareStateMacro( ImageLandmark3Added );
  igstkDeclareStateMacro( TrackerLandmark3Added );
  igstkDeclareStateMacro( AttemptingToComputeTransform ); 
  igstkDeclareStateMacro( TransformComputed ); 


  /** List of Inputs */
  igstkDeclareInputMacro( ImageLandmark );
  igstkDeclareInputMacro( TrackerLandmark );
  igstkDeclareInputMacro( ComputeTransform );
  igstkDeclareInputMacro( GetTransform );
  igstkDeclareInputMacro( ResetRegistration );
  igstkDeclareInputMacro( TransformComputationFailure );
  igstkDeclareInputMacro( TransformComputationSuccess );

  /**  The "CheckCollinearity" method checks whether the landmark points
   *   are colliner or not */
  bool CheckCollinearity();
   
  /** The "AddImageLandmark" method adds landmark points to the image
   * landmark point container */

  void AddImageLandmarkPointProcessing();
  
  /** The "AddTrackerLandmark" method adds landmark points to the
   * tracker landmark point container */
  void AddTrackerLandmarkPointProcessing();

  /** The "ResetRegsitration" method empties the landmark point
   * containers to start the process again */
  void ResetRegistrationProcessing();

  /** The "ComputeTransform" method calculates the rigid body
   * transformation parameters */
  void ComputeTransformProcessing();

 
  /** The "GetTransform" method throws an event containing the 
   *  transform parameters*/
  void GetTransformProcessing();

  /** The "ReportInvalidRequest" method throws InvalidRequestErrorEvent
   *  when invalid requests are made */
  void ReportInvalidRequestProcessing();

  /** The "ReportSuccessInTransformComputation" method throws 
   *  TransformComputationSuccessEvent*/
  void ReportSuccessInTransformComputationProcessing();

  /** The "ReportFailureInTransformComputation" method throws 
   *  TransformComputationFailureEvent*/
  void ReportFailureInTransformComputationProcessing();
};

} // end namespace igstk

#endif // __igstkLandmark3DRegistration_h
