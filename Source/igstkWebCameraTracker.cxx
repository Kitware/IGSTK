/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkWebCameraTracker.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#if defined(_MSC_VER)
// Warning about: identifier was truncated to '255' characters in
// the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#include "igstkWebCameraTracker.h"
#include "vtkWin32VideoSource.h"
#include "vtkImageViewer2.h"
#include "vtkImageData.h"
#include "vtkImageIterator.h"

namespace igstk
{

WebCameraTracker::WebCameraTracker():m_StateMachine(this)
{
  this->m_VideoSource = vtkWin32VideoSource::New();
  this->m_ImageViewer = vtkImageViewer2::New();
  this->m_X = 0.0;
}

WebCameraTracker::~WebCameraTracker()
{
  this->m_VideoSource->ReleaseSystemResources();
  this->m_VideoSource->Delete();
  this->m_ImageViewer->Delete();
}

WebCameraTracker::ResultType WebCameraTracker::InternalOpen( void )
{
  this->m_VideoSource->SetOutputFormatToRGB();
  this->m_VideoSource->SetFrameRate( 30 );
  this->m_VideoSource->SetFrameSize( 960, 720, 1 );
  this->m_VideoSource->Initialize();
  this->m_Position.Fill( 0.0 );
  this->m_ImageViewer->SetInputConnection(
    this->m_VideoSource->GetOutputPort() );
  this->m_X = 0.0;
  return SUCCESS;
}

WebCameraTracker::ResultType WebCameraTracker::InternalStartTracking( void )
{
  return SUCCESS;
}

WebCameraTracker::ResultType WebCameraTracker::InternalReset( void )
{
  return SUCCESS;
}

WebCameraTracker::ResultType WebCameraTracker::InternalStopTracking( void )
{
  return SUCCESS;
}

WebCameraTracker::ResultType WebCameraTracker::InternalDeactivateTools( void )
{
  return SUCCESS;
}

WebCameraTracker::ResultType WebCameraTracker::InternalClose( void )
{
  this->m_VideoSource->ReleaseSystemResources();
  return SUCCESS;
}

WebCameraTracker::ResultType
WebCameraTracker::VerifyTrackerToolInformation(
  const TrackerToolType * trackerTool )
{
  return SUCCESS;
}

WebCameraTracker::ResultType
WebCameraTracker::InternalUpdateStatus( void )
{
  igstkLogMacro( DEBUG, "WebCameraTracker::InternalUpdateStatus called ...\n");

  this->m_VideoSource->Grab();
  this->m_VideoSource->UpdateWholeExtent();
  this->m_ImageViewer->Render();
  this->DetectBrightestPixels();
  this->ComputeCenterOfMass();

  typedef TrackerToolsContainerType::const_iterator  ConstIteratorType;

  TrackerToolsContainerType trackerToolContainer =
    this->GetTrackerToolContainer();

  ConstIteratorType inputItr = trackerToolContainer.begin();
  ConstIteratorType inputEnd = trackerToolContainer.end();

  typedef igstk::Transform   TransformType;
  TransformType transform;

  transform.SetToIdentity( this->GetValidityTime() );

  typedef TransformType::VectorType PositionType;
  PositionType  position;
  position[0] = m_Position[0];
  position[1] = m_Position[1];
  position[2] = m_Position[2];

  typedef TransformType::ErrorType  ErrorType;
  ErrorType errorValue = 0.5; // +/- half millimeter Uncertainty

  transform.SetTranslation( position, errorValue, this->GetValidityTime() );

  // set the raw transform in all the tracker tools
  while( inputItr != inputEnd )
    {
    this->SetTrackerToolRawTransform(
      trackerToolContainer[inputItr->first], transform );

    this->SetTrackerToolTransformUpdate(
      trackerToolContainer[inputItr->first], true );

    ++inputItr;
    }

  m_X += 0.5;

  return SUCCESS;
}


WebCameraTracker::ResultType
WebCameraTracker::InternalThreadedUpdateStatus( void )
{
  igstkLogMacro( DEBUG,
    "WebCameraTracker::InternalThreadedUpdateStatus called ...\n");
  return SUCCESS;
}

WebCameraTracker::ResultType
WebCameraTracker
::RemoveTrackerToolFromInternalDataContainers(
  const TrackerToolType * trackerTool )
{
  return SUCCESS;
}

WebCameraTracker::ResultType
WebCameraTracker
::AddTrackerToolToInternalDataContainers( const TrackerToolType * trackerTool )
{
  return SUCCESS;
}

void
WebCameraTracker::DetectBrightestPixels( void )
{
  this->m_BrightestPixelValues.clear();
  this->m_BrightestPixelPositions.clear();

  vtkImageData * inputImage = this->m_VideoSource->GetOutput();

  double origin[3];
  double spacing[3];

  inputImage->GetOrigin( origin );
  inputImage->GetSpacing( spacing );

  int extent[6];
  inputImage->GetWholeExtent( extent );

  double imageCenter[3];

  imageCenter[0] = origin[0] + spacing[0] * ( extent[1] - extent[0] ) / 2.0;
  imageCenter[1] = origin[1] + spacing[1] * ( extent[3] - extent[2] ) / 2.0;
  imageCenter[2] = origin[2] + spacing[2] * ( extent[3] - extent[2] ) / 2.0;

  const int maxC = inputImage->GetNumberOfScalarComponents();
  int idxC;


  vtkImageIterator< PixelType >  itr( inputImage, extent );

  const PixelType threshold = 250;

  double threshold2 = threshold;
  threshold2 *= threshold;

  unsigned int numberOfPixels = 0;

  int posX = 0;
  int posY = 0;

  double pixelValue = 0.0;

  PointType point;

  while( !itr.IsAtEnd() )
    {
    PixelType * inSI    = itr.BeginSpan();
    PixelType * inSIEnd = itr.EndSpan();

    while( inSI != inSIEnd )
      {

      // Compute luminosity from components
      // Note: It should use RGB to Luminance computation instead.
      pixelValue = 0.0;
      for( idxC=0; idxC < maxC; idxC++ )
        {
        pixelValue += ( *inSI ) * ( *inSI );
        ++inSI;
        }

      if( pixelValue > threshold2 )
        {
        point[0] = origin[0] + posX * spacing[0] - imageCenter[0];
        point[1] = origin[1] + posY * spacing[1] - imageCenter[1];
        this->m_BrightestPixelPositions.push_back( point );
        this->m_BrightestPixelValues.push_back( vcl_sqrt( pixelValue ) );
        }

      ++posX;
      ++numberOfPixels;
      }
    itr.NextSpan();
    posX = 0;
    ++posY;
    }
}


void
WebCameraTracker::ComputeCenterOfMass( void )
{
  typedef PixelValueListType::const_iterator IteratorType;

  IteratorType valueItr = this->m_BrightestPixelValues.begin();
  IteratorType valueEnd = this->m_BrightestPixelValues.end();

  PointListType::const_iterator pointItr =
      this->m_BrightestPixelPositions.begin();

  double sumWeights   = 0.0;
  double sumWeightedX = 0.0;
  double sumWeightedY = 0.0;

  while( valueItr != valueEnd )
    {
    sumWeights   += (*valueItr);
    sumWeightedX += (*valueItr) * pointItr->operator[]( 0 );
    sumWeightedY += (*valueItr) * pointItr->operator[]( 1 );
    ++pointItr;
    ++valueItr;
    }

  if( sumWeights != 0.0 )
    {
    sumWeightedX /= sumWeights;
    sumWeightedY /= sumWeights;
    }

  this->m_Position[0] = sumWeightedX;
  this->m_Position[1] = sumWeightedY;

  std::cout << this->m_Position << " from "
    << this->m_BrightestPixelPositions.size()
    << " pixels " << std::endl;
}


/** Print Self function */
void WebCameraTracker::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Video Source = " << this->m_VideoSource << std::endl;
}

}
