/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    PCAOnPoints.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __PCAOnPoints_h
#define __PCAOnPoints_h

#include "itkImage.h"

class PCAOnPoints: public itk::Object
{

public:

  typedef PCAOnPoints              Self;
  typedef itk::Object                       Superclass;
  typedef itk::SmartPointer< Self >         Pointer;
  typedef itk::SmartPointer< const Self >   ConstPointer;

  itkNewMacro( Self );

  typedef itk::Point< double, 3 >           PointType;
  typedef std::vector< PointType >          PointsListType;
  typedef vnl_vector< double >              VectorType;

  bool Execute();

  void SetSamplePoints( PointsListType pList )
    {
    m_SamplePoints = pList;
    };
  
  PointsListType GetSortedPoints( void )
    {
    return m_SortedPoints;
    };

  itkGetMacro( Center, PointType);
  itkGetMacro( PrincipleAxis, VectorType);

protected:
  PCAOnPoints();
  virtual ~PCAOnPoints(){};
  void PrintSelf( std::ostream & os, itk::Indent indet );

private:

  PointsListType                       m_SamplePoints;
  PointsListType                       m_SortedPoints;
  PointType                            m_Center;
  VectorType                           m_PrincipleAxis;

};

#endif
