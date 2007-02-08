/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    LandmarkBasedRegistration.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __LandmarkBasedRegistration_h
#define __LandmarkBasedRegistration_h

#include "DeckOfCardRobotRegistrationBase.h"

namespace igstk 
{

class LandmarkBasedRegistration: 
  public DeckOfCardRobotRegistrationBase
{

public:

  typedef LandmarkBasedRegistration              Self;
  typedef DeckOfCardRobotRegistrationBase        Superclass;
  typedef itk::SmartPointer< Self >              Pointer;
  typedef itk::SmartPointer< const Self >        ConstPointer;

  itkNewMacro( Self );
  
  igstkObserverMacro( Transform, 
                            igstk::TransformModifiedEvent, igstk::Transform );
  virtual bool Execute();

protected:
  LandmarkBasedRegistration();
  virtual ~LandmarkBasedRegistration(){};
  void PrintSelf( std::ostream& os, itk::Indent indent );

private:

};

} // end namespace igstk

#endif
