/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    ModelBasedClustering.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __ModelBasedClustering_h
#define __ModelBasedClustering_h

#include "itkImage.h"

namespace igstk
{

class ModelBasedClustering: public itk::Object
{

public:

  typedef ModelBasedClustering              Self;
  typedef itk::Object                       Superclass;
  typedef itk::SmartPointer< Self >         Pointer;
  typedef itk::SmartPointer< const Self >   ConstPointer;

  itkNewMacro( Self );

  typedef itk::Point< double, 3 >           PointType;
  typedef std::vector< PointType >          PointsListType;

  bool Execute();

  void SetSamplePoints( PointsListType pList )
    {
    m_SamplePoints = pList;
    }

  void SetModelPoints( PointsListType pList )
    {
    m_ModelPoints = pList;
    }

  PointsListType GetClusteredPoints()
    {
    return m_ClusteredPoints;
    }

protected:
  ModelBasedClustering();
  virtual ~ModelBasedClustering(){};
  void PrintSelf( std::ostream & os, itk::Indent indet );

private:

  double Distance( PointType p1, PointType p2 );

  double DistanceToSimilarity( double dis );
  
  PointsListType                       m_SamplePoints;
  PointsListType                       m_ModelPoints;
  PointsListType                       m_ClusteredPoints;

};

} // end namespace igstk

#endif
