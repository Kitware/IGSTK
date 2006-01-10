/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLandmark3DRegistrationError.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkLandmark3DRegistrationError_h
#define __igstkLandmark3DRegistrationError_h

#include "igstkStateMachine.h"
#include "igstkMacros.h"
#include "igstkObject.h"
#include "itkPoint.h"
#include "itkVersor.h"
#include <vector>

namespace igstk
{
/** \class Landmark3DRegistrationError
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

class Landmark3DRegistrationError : public Object
{

public:
  
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( Landmark3DRegistrationError, Object )

public:

  /** Typedefs */
  typedef itk::Point<double, 3>                           LandmarkPointType;
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

  /** Compute and set landmark principal axes */
  void ComputeLandmarkPrincipalAxes();

  /** Compute and set RMS distance of landmarks from the principal axes */
  void ComputeRMSDistanceLandmarksFromPrincipalAxes();

  /** Compute Normalized landmark registration error */
  void ComputeNormalizedLandmarkRegistrationError();

  /** Compute all error parameters in rigid-body landmark registration */
  void ComputeErrorParameters();

protected:

  Landmark3DRegistrationError  ( void );
  ~Landmark3DRegistrationError ( void ) ;

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const;

private:
 
  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  Landmark3DRegistrationError(const Self&);    //purposely not implemented
  void operator=(const Self&);          //purposely not implemented

  LandmarkPointContainerType               m_TrackerLandmarks;
  LandmarkPointContainerType               m_ImageLandmarks;
 
  /** Landmark configuration */
  VersorType                               m_LandmarkPrincipalAxes;

  /** RMS distance from the landmarks to the principal axes of the landmark configuration */
  DistanceType                             m_RMSDistanceFromLandmarkToPrincipalAxes1;
  DistanceType                             m_RMSDistanceFromLandmarkToPrincipalAxes2;
  DistanceType                             m_RMSDistanceFromLandmarkToPrincipalAxes3;
 
  /** Landmarks center point */ 
  LandmarkPointType                        m_LandmarkCenterPoint;

  /** Normalized landmark registration error */
  ErrorType                                m_NormalizedLandmarkRegistrationError;


  /** List of States */

  /** List of Inputs */

};
} // end namespace igstk

#endif // __igstkLandmark3DRegistrationError_h

