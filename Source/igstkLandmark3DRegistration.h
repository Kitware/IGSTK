/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLandmark3DRegistration.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

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
#include "igstkTransform.h"
#include "igstkCoordinateSystem.h"

#include "itkImage.h"
#include "itkLandmarkBasedTransformInitializer.h"


namespace igstk
{
/** \class Landmark3DRegistration
 * \brief This class computes rigid body transformation parameters between 
 * two 3D coordinate systems. 
 *  
 * The Landmark registration class is used to compute rigid body transformation
 * parameters between two 3D coordinate system. For example, transformation 
 * between an image coordinate system and a tracker system. The registration
 * algorithm is based on a closed-form solution to the least-squares problem of
 * computing transformation parameters between two coordinate sytems.
 * A minimum of three pair of landmarks are required. The class is basically 
 * a wrapper around the itk::LandmarkBasedTransformInitializer.
 *
 * Before computing the transformation parameters,  the algorithm checks if
 * the landmarks are collinear by analyzing the eigen values of the geometric 
 * distribution of the landmark points. For this purpose, a tolerance metric is 
 * defined. The tolerance metric is given by the ratio of the square sum of the
 * two smallest eigen values to the square of the largest eigen value. 
 * By default, the tolerance value is set to 0.01. However, the user can modify
 * the tolerance using RequestSetCollinearityTolerance() method.
 *  
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
   * state */
  void RequestResetRegistration();

  /** The "RequestComputeTransform" method will be used to request
   * transform parameter calculation */
  void RequestComputeTransform();

  /** The "RequesteGetTransformFromTrackerToImage" method will be used
   *  to request to get the transform from the Tracker to the Image. */
  void RequestGetTransformFromTrackerToImage();

  /** The "RequesteGetTransformFromImageToTracker" method will be used
   *  to request to get the transform from the Image to the Tracker. */
  void RequestGetTransformFromImageToTracker();
 
  /** The "RequestGetRMSError" method will be used to get the RMS error
   *  value */
  void RequestGetRMSError();

  /** RequestSetCollinearityTolerance method will be used to set collinearity
      tolerance */
  void RequestSetCollinearityTolerance( const double & tolerance ); 
  
  /** Landmark registration events */
  igstkEventMacro( TransformInitializerEvent,       IGSTKEvent );
  igstkEventMacro( TransformInitializerErrorEvent,  IGSTKErrorEvent );

  /** TransformComputationFailureEvent event will be invoked if the transform
      computation fails */
  igstkEventMacro( TransformComputationFailureEvent,
                                              TransformInitializerErrorEvent );

  /** TransformComputationSuccessEvent event will be invoked if the transform
      computation is succesful */ 
  igstkEventMacro( TransformComputationSuccessEvent,TransformInitializerEvent);

protected:

  Landmark3DRegistration  ( void );
  ~Landmark3DRegistration ( void );

  /** The "ComputeRMSError" method calculates and returns RMS error */
  void ComputeRMSError();

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
  
  Transform::ErrorType                     m_RMSError;

  /** Collinearity tolerance */
  double                                   m_CollinearityTolerance;
  
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
  igstkDeclareInputMacro( GetTransformFromTrackerToImage );
  igstkDeclareInputMacro( GetTransformFromImageToTracker );
  igstkDeclareInputMacro( GetRMSError );
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

  /** The "GetTransformFromTrackerToImageProcessing" method throws
   * an event containing the transform parameters from tracker to
   * image */
  void GetTransformFromTrackerToImageProcessing();

  /** The "GetTransformFromImageToTrackerProcessing" method throws
   * an event containing the transform parameters from image to
   * tracker */
  void GetTransformFromImageToTrackerProcessing();

  /** The "GetRMSErrorProcessing" method throws an event containing the 
   *  RMS error value */
  void GetRMSErrorProcessing();

  /** The "ReportInvalidRequest" method throws InvalidRequestErrorEvent
   *  when invalid requests are made */
  void ReportInvalidRequestProcessing();

  /** The "ReportSuccessInTransformComputation" method throws 
   *  TransformComputationSuccessEvent*/
  void ReportSuccessInTransformComputationProcessing();

  /** The "ReportFailureInTransformComputation" method throws 
   *  TransformComputationFailureEvent*/
  void ReportFailureInTransformComputationProcessing();

  /** Coordinate systems for the Image and Tracker that are being registered */ 
  CoordinateSystem::Pointer     m_TrackerCoordinateSystem;
  CoordinateSystem::Pointer     m_ImageCoordinateSystem;
};

} // end namespace igstk

#endif // __igstkLandmark3DRegistration_h
