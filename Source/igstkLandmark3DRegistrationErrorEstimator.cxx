/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLandmark3DRegistrationErrorEstimator.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more DEBUGrmation.
=========================================================================*/
#ifndef __igstkLandmark3DRegistrationErrorEstimator_cxx
#define __igstkLandmark3DRegistrationErrorEstimator_cxx

#if defined(_MSC_VER)
#pragma warning( disable : 4786 )
#endif

#include "igstkLandmark3DRegistrationErrorEstimator.h"

namespace igstk
{ 

/** Constructor */
Landmark3DRegistrationErrorEstimator::Landmark3DRegistrationErrorEstimator() :
  m_StateMachine( this )
{
} 

/** Destructor */
Landmark3DRegistrationErrorEstimator::~Landmark3DRegistrationErrorEstimator()  
{

}

/** Compute and set landmark principal axes */
void Landmark3DRegistrationErrorEstimator::ComputeLandmarkPrincipalAxes()
{

}

/** Compute and set RMS distance of landmarks from the principal axes */
void Landmark3DRegistrationErrorEstimator::ComputeRMSDistanceLandmarksFromPrincipalAxes()
{

}

/** Compute Normalized landmark registration error */
void Landmark3DRegistrationErrorEstimator::ComputeNormalizedLandmarkRegistrationError()
{

}

/* The "ComputeErrorParameters" method calculates the rigid body
  registration error parameters */
void 
Landmark3DRegistrationErrorEstimator:: ComputeErrorParameters()
{
  igstkLogMacro( DEBUG, "igstk::Landmark3DRegistrationErrorEstimator::"
                 "ComputeErrorParameters called...\n");
  this->ComputeLandmarkPrincipalAxes();
  this->ComputeRMSDistanceLandmarksFromPrincipalAxes();
  this->ComputeNormalizedLandmarkRegistrationError();
}

/** Print Self function */
void 
Landmark3DRegistrationErrorEstimator::PrintSelf( std::ostream& os,
                                             itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Tracker Landmarks: " << std::endl;
  PointsContainerConstIterator fitr = m_TrackerLandmarks.begin();
  while( fitr != m_TrackerLandmarks.end() )
    {
    os << indent << *fitr << std::endl;
    ++fitr;
    }
  os << indent << "Image Landmarks: " << std::endl;
  PointsContainerConstIterator mitr = m_ImageLandmarks.begin();
  while( mitr != m_ImageLandmarks.end() )
    {
    os << indent << *mitr << std::endl;
    ++mitr;
    }
}

} // end namespace igstk
#endif


