/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    FiducialModel.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __FiducialModel_h
#define __FiducialModel_h

#include "itkImage.h"
#include "igstkCTImageSpatialObject.h"

/** \class FiducialModel
*  \brief This class stores the fiducial model information.
*
* This class will generate points thats on the fiducial model
*
*/

class FiducialModel: public itk::Object
{

public:

  typedef FiducialModel                     Self;
  typedef itk::Object                       Superclass;
  typedef itk::SmartPointer< Self >         Pointer;
  typedef itk::SmartPointer< const Self >   ConstPointer;

  itkNewMacro( Self );

  typedef igstk::CTImageSpatialObject::ImageType            ImageType;
  typedef itk::Point< double, 3 >           PointType;
  typedef std::vector< PointType >          PointsListType;

  itkGetMacro( Size, int);
  itkSetMacro( Size, int);

  itkGetMacro( BlurSize, int);
  itkSetMacro( BlurSize, int);
  
  virtual ImageType::Pointer GetModelImage();

  virtual PointsListType GetFiducialPoints()
    {
    return m_FiducialPoints;
    };

protected:
  FiducialModel();
  virtual ~FiducialModel(){};
  void PrintSelf( std::ostream& os, itk::Indent indent ) const;
  
private:

  // Generate the fiducial point list
  void GenerateFiducialPointList();

  // Generate model image containing the fiducial points
  void GenerateModelImage();

  ImageType::Pointer                   m_ModelImage;
  
  int                                  m_Size;
  int                                  m_BlurSize;

  PointsListType                       m_FiducialPoints;              

};

#endif
