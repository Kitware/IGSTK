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
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"

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
  cluster->Execute();

  // Build the vtk data structure for ICP registration
  vtkPoints    * sourcePoints = vtkPoints::New();
  vtkPoints    * targetPoints = vtkPoints::New();
  vtkCellArray * sourceCell    = vtkCellArray::New();
  vtkCellArray * targetCell    = vtkCellArray::New();

  sourceCell->InsertNextCell( cluster->GetClusteredPoints().size() );
  for ( int i=0; i<cluster->GetClusteredPoints().size(); i++)
    {
    double p[3];
    p[0] = cluster->GetClusteredPoints()[i][0];
    p[1] = cluster->GetClusteredPoints()[i][1];
    p[2] = cluster->GetClusteredPoints()[i][2];
    sourcePoints->InsertNextPoint( p );
    sourceCell->InsertCellPoint( i );
    }

  for ( int i=0; i<model->GetFiducialPoints().size(); i++)
    {
    double p[3];
    p[0] = model->GetFiducialPoints()[i][0];
    p[1] = model->GetFiducialPoints()[i][1];
    p[2] = model->GetFiducialPoints()[i][2];
    targetPoints->InsertNextPoint( p );
    targetCell->InsertCellPoint( i );
    }

  vtkPolyData * source = vtkPolyData::New();
  vtkPolyData * target = vtkPolyData::New();

  source->SetPoints( sourcePoints );
  source->SetVerts( sourceCell );
  target->SetPoints( targetPoints );
  target->SetVerts( targetCell );

  vtkIterativeClosestPointTransform * ICPTransform = 
                                      vtkIterativeClosestPointTransform::New();

  ICPTransform->SetSource( source );
  ICPTransform->SetTarget( target );
  ICPTransform->SetMeanDistanceModeToRMS();
  ICPTransform->CheckMeanDistanceOn();
  ICPTransform->StartByMatchingCentroidsOn();
  ICPTransform->SetMaximumNumberOfIterations( 50 );
  ICPTransform->SetMaximumMeanDistance( 0.5 );  
  ICPTransform->SetMaximumNumberOfLandmarks( 50 );

  ICPTransform->Update();

  vtkIndent indent;
  ICPTransform->PrintSelf( std::cout, indent);

  return true;
}
