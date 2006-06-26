/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    FiducialModel.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "FiducialModel.h"


FiducialModel::FiducialModel()
{
  m_ModelImage = NULL;
  m_Size = 4;
  m_BlurSize = 1;
  m_FiducialPoints.clear();

  double const R = sqrt( 3.3 * 3.3 + 8.0 * 8.0 );
  double const W = vnl_math::pi / 10;
  double const Theta = atan( 8 / 3.3 );

  PointType p;
  m_FiducialPoints.clear();
  for ( double z = 0; z <= 45; z += 2.5)
    {
    double x = R * cos( W * z + Theta );
    double y = R * sin( W * z + Theta );
    p[0] = x;
    p[1] = y;
    p[3] = z - 1.25;
    m_FiducialPoints.push_back( p );
    }
}

void FiducialModel::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Size: "
    << m_Size << std::endl;
  os << indent << "Blur Size: "
    << m_BlurSize << std::endl;

}

FiducialModel::ImageType::Pointer
FiducialModel::GetModelImage()
{

  return m_ModelImage;
}
