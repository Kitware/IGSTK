/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    IterativeClosestPointsBasedRegistration.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISIS Georgetown University. All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "IterativeClosestPointsBasedRegistration.h"

#include "FiducialSegmentation.h"
#include "FiducialModel.h"
#include "ModelBasedClustering.h"
#include "vtkIterativeClosestPointTransform.h"

IterativeClosestPointsBasedRegistration::
  IterativeClosestPointsBasedRegistration()
{
  m_ITKImage = NULL;
  m_Transform.SetToIdentity( 1e300 );
}


void IterativeClosestPointsBasedRegistration::
  PrintSelf(std::ostream& os, itk::Indent indent)
{
  Superclass::PrintSelf(os, indent);
}

bool IterativeClosestPointsBasedRegistration::Execute()
{ 
  // Segment the fiducial points
  FiducialSegmentation::Pointer segmenter = FiducialSegmentation::New();
  segmenter->SetITKImage( m_ITKImage );
  segmenter->SetThreshold( 3000 );
  segmenter->SetMaxSize( 20 );
  segmenter->SetMinSize( 0 );
  segmenter->SetMergeDistance( 1 );
  segmenter->Execute();

  // Generate the model points
  FiducialModel::Pointer model = FiducialModel::New();

  // Cluster the points, eliminate outliers
  ModelBasedClustering::Pointer cluster = ModelBasedClustering::New();
  cluster->SetSamplePoints( segmenter->GetFiducialPoints() );
  cluster->SetModelPoints( model->GetFiducialPoints() );



  return true;
}

