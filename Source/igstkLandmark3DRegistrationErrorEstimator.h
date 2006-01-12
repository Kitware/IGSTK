/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLandmark3DRegistrationErrorEstimator.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkLandmark3DRegistrationErrorEstimator_h
#define __igstkLandmark3DRegistrationErrorEstimator_h

#include "igstkStateMachine.h"
#include "igstkMacros.h"
#include "igstkObject.h"
#include "itkPoint.h"
#include "itkVersor.h"
#include <vector>

namespace igstk
{
/** \class Landmark3DRegistrationErrorEstimator
 * \brief This class computes landmark registration error.
 * 
 * The error computation is based on the work by West et al.
 *
 * J.B West et al, "Fiducial Point Placement and the Accuracy of Point-based,
 * Rigid Body Registration", Neurosurgery, pp 810-816, Vol.48, No.4, April 2001.  
 *
 *
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
  typedef itk::Point<double, 3>                           PointType;
  typedef itk::Point<double, 3>                           LandmarkPointType;
  typedef itk::Vector<double, 3>                          VectorType;
  typedef std::vector< LandmarkPointType >                LandmarkPointContainerType;
  typedef LandmarkPointContainerType::const_iterator      PointsContainerConstIterator;

  /** Error parameter typedefs */
  typedef double                                          ErrorType;
  typedef double                                          DistanceType;
  typedef itk::Versor<double>                             VersorType;

  /** Set image landmark points */
  igstkSetMacro ( ImageLandmarks , LandmarkPointContainerType );

  /** Set tracker landmark points*/
  igstkSetMacro ( TrackerLandmarks , LandmarkPointContainerType );

  /** Set the normalized landmark registration error. This is computed by 
   *  the landmarkRegistration class */
  igstkSetMacro ( LandmarkRegistrationError, double );

  /** Compute Landmarks centroid */
  void ComputeLandmarksCentroid();

  /** Compute and set landmark principal axes */
  void ComputeLandmarkPrincipalAxes();

  /** Compute and set RMS distance of landmarks from the principal axes */
  void ComputeRMSDistanceLandmarksFromPrincipalAxes();

  /** Compute all error parameters in rigid-body landmark registration */
  void ComputeErrorParameters();

  /** Estimate target registration error */
  ErrorType  EstimateTargetRegistrationError( PointType );

protected:

  Landmark3DRegistrationErrorEstimator  ( void );
  ~Landmark3DRegistrationErrorEstimator ( void ) ;

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const;

private:
 
  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  Landmark3DRegistrationErrorEstimator(const Self&);    //purposely not implemented
  void operator=(const Self&);          //purposely not implemented

  LandmarkPointContainerType               m_TrackerLandmarks;
  LandmarkPointContainerType               m_ImageLandmarks;
 
  /** Landmark configuration */
  VersorType                               m_LandmarkPrincipalAxes;

  /** RMS distance from the landmarks to the principal axes of the landmark configuration */
  VectorType                               m_RMSDistanceFromLandmarkToPrincipalAxes;

  /** Landmarks centroid */ 
  LandmarkPointType                        m_LandmarkCentroid;

  /** landmark registration error */
  ErrorType                                m_LandmarkRegistrationError;


  /** List of States */

  /** List of Inputs */

};
} // end namespace igstk

#endif // __igstkLandmark3DRegistrationErrorEstimator_h

