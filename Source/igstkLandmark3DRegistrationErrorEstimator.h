/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLandmark3DRegistrationErrorEstimator.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkLandmark3DRegistrationErrorEstimator_h
#define __igstkLandmark3DRegistrationErrorEstimator_h

#include "igstkStateMachine.h"
#include "igstkMacros.h"
#include "igstkObject.h"
#include "igstkEvents.h"
#include "itkPoint.h"
#include "itkVersor.h"
#include <vector>

namespace igstk
{
/** \class Landmark3DRegistrationErrorEstimator
 * 
 * \brief This class estimates error in point-based 
 *   rigid-body landmark registration .
 * 
 *  The error estimation is based on the work by West et al.
 *
 * J.B West et al, "Fiducial Point Placement and the Accuracy of Point-based,
 * Rigid Body Registration", Neurosurgery, pp 810-816, Vol.48, No.4, 
 * April 2001.  
 * 
 *\image html igstkLandmark3DRegistrationErrorEstimator.png "State Machine"
 *
 *\image latex igstkLandmark3DRegistrationErrorEstimator.eps "State Machine"
 *
 * \ingroup Registration 
 */

class Landmark3DRegistrationErrorEstimator : public Object
{

public:
  
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( Landmark3DRegistrationErrorEstimator, Object )

public:

  /** Typedefs */
  typedef itk::Point<double, 3>                           TargetPointType;
  typedef itk::Point<double, 3>                           LandmarkPointType;
  typedef std::vector< LandmarkPointType >                LandmarkContainerType;

  /** Error parameter Typedefs */
  typedef double                                          ErrorType;

  /** Method to set the landmark container */
  void RequestSetLandmarkContainer( const LandmarkContainerType & );

  /** Method to set the target point */
  void RequestSetTargetPoint ( const TargetPointType & );

  /** Method to set the landmark registration error */
  void RequestSetLandmarkRegistrationError ( const ErrorType & );

  /** Method to request target registration estimation */
  void RequestEstimateTargetPointRegistrationError( );

  /** Request target point registration error event */
  void RequestGetTargetPointRegistrationErrorEstimate();
   
protected:

  Landmark3DRegistrationErrorEstimator  ( void );
  ~Landmark3DRegistrationErrorEstimator ( void );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const;

private:
 
  typedef itk::Vector<double, 3>                 VectorType;
  typedef LandmarkContainerType::const_iterator  PointsContainerConstIterator;
  typedef double                                 DistanceType;
  typedef itk::Versor<double>                    VersorType;

  
  /** Compute Landmarks centroid */
  void ComputeLandmarksCentroid();

  /** Compute landmark principal axes */
  void ComputeLandmarkPrincipalAxes();

  /** Compute RMS distance of landmarks from the principal axes */
  void ComputeRMSDistanceLandmarksFromPrincipalAxes();

  /** Set landmark container */
  void SetLandmarkContainerProcessing();

  /** Set target point */
  void SetTargetPointProcessing();

  /** Set landmark registration error */
  void SetLandmarkRegistrationErrorProcessing( );
    
  void ComputeErrorParameters( );

  /** Estimate target point registration error */
  void EstimateTargetPointRegistrationErrorProcessing( );

  /** Report any invalid request to the logger */
  void ReportInvalidRequestProcessing();

  /** Report success in target point registration error estimation */  
  void ReportSuccessInTargetPointRegistrationErrorEstimationProcessing();

  /** Report failure in target point registration error estimation */  
  void ReportFailureInTargePointRegistrationErrorEstimationProcessing();

  /** This method throws an event loaded with the landmark registration 
   *  error */
  void GetTargetPointRegistrationErrorEstimateProcessing();

  /** List of States */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( LandmarkContainerSet );
  igstkDeclareStateMacro( LandmarkRegistrationErrorSet );
  igstkDeclareStateMacro( TargetPointSet );
  igstkDeclareStateMacro( AttemptingToEstimateTargetRegstirationError );
  igstkDeclareStateMacro( TargetRegistrationErrorEstimated );


  /** List of Inputs */
  igstkDeclareInputMacro( LandmarkContainer );
  igstkDeclareInputMacro( LandmarkRegistrationError );
  igstkDeclareInputMacro( TargetPoint );
  igstkDeclareInputMacro( EstimateTargetPointRegistrationError );
  igstkDeclareInputMacro( TargetPointRegistrationErrorEstimationSuccess );
  igstkDeclareInputMacro( TargetPointRegistrationErrorEstimationFailure );
  igstkDeclareInputMacro( GetTargetPointRegistrationErrorEstimate );

  
  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  Landmark3DRegistrationErrorEstimator(const Self&); //purposely not implemented
  void operator=(const Self&);          //purposely not implemented

  LandmarkContainerType             m_LandmarkContainer;
  LandmarkContainerType             m_LandmarkContainerToBeSet;

  TargetPointType                   m_TargetPoint;
  TargetPointType                   m_TargetPointToBeSet;

  ErrorType                         m_LandmarkRegistrationError;
  ErrorType                         m_LandmarkRegistrationErrorToBeSet;

  ErrorType                         m_TargetPointRegistrationError;
  
  /** Landmark configuration */
  itk::Matrix<double,3,3>           m_LandmarkPrincipalAxes;

  /** RMS distance from the landmarks to the principal axes of the 
   *  landmark configuration */
  VectorType                        m_RMSDistanceFromLandmarkToPrincipalAxes;

  /** Landmarks centroid */ 
  LandmarkPointType                 m_LandmarkCentroid;

};
} // end namespace igstk

#endif // __igstkLandmark3DRegistrationErrorEstimator_h
