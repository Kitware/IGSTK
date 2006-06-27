/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    DeckOfCardRobotRegistrationBase.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __DeckOfCardRobotRegistrationBase_h
#define __DeckOfCardRobotRegistrationBase_h

#include "itkImage.h"
#include "igstkCTImageSpatialObject.h"
#include "igstkTransform.h"

class DeckOfCardRobotRegistrationBase: public itk::Object
{

public:

  typedef DeckOfCardRobotRegistrationBase   Self;
  typedef itk::Object                       Superclass;
  typedef itk::SmartPointer< Self >         Pointer;
  typedef itk::SmartPointer< const Self >   ConstPointer;

  typedef igstk::CTImageSpatialObject       ImageSOType;
  typedef ImageSOType::ImageType            ImageType;

  typedef igstk::Transform                  TransformType;

  itkGetMacro( ITKImage, ImageType::ConstPointer );
  itkSetMacro( ITKImage, ImageType::ConstPointer );
   
  virtual bool Execute() = 0;

  itkGetMacro( Transform, TransformType );  

protected:

  void PrintSelf( std::ostream& os, itk::Indent indent );
  
  ImageType::ConstPointer              m_ITKImage;
  TransformType                        m_Transform;

private:

};

#endif
