/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    IterativeClosestPointsBasedRegistration.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __IterativeClosestPointsBasedRegistration_h
#define __IterativeClosestPointsBasedRegistration_h

#include "DeckOfCardRobotRegistrationBase.h"

namespace igstk
{

class IterativeClosestPointsBasedRegistration: 
  public DeckOfCardRobotRegistrationBase
{

public:

  typedef IterativeClosestPointsBasedRegistration              Self;
  typedef DeckOfCardRobotRegistrationBase                      Superclass;
  typedef itk::SmartPointer< Self >                            Pointer;
  typedef itk::SmartPointer< const Self >                      ConstPointer;

  itkNewMacro( Self );
   
  virtual bool Execute();

protected:
  IterativeClosestPointsBasedRegistration();
  virtual ~IterativeClosestPointsBasedRegistration(){};
  void PrintSelf( std::ostream& os, itk::Indent indent );

private:

};

} // end namespace igstk

#endif
