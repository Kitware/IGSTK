/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    ModelBasedRegistration.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __ModelBasedRegistration_h
#define __ModelBasedRegistration_h

#include "DeckOfCardRobotRegistrationBase.h"

class ModelBasedRegistration: 
  public DeckOfCardRobotRegistrationBase
{

public:

  typedef ModelBasedRegistration              Self;
  typedef DeckOfCardRobotRegistrationBase                      Superclass;
  typedef itk::SmartPointer< Self >                            Pointer;
  typedef itk::SmartPointer< const Self >                      ConstPointer;

  itkNewMacro( Self );
   
  virtual bool Execute();

protected:
  ModelBasedRegistration();
  virtual ~ModelBasedRegistration(){};
  void PrintSelf( std::ostream& os, itk::Indent indent );

private:

};

#endif
