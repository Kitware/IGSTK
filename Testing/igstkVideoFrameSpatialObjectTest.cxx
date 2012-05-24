/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVideoFrameSpatialObjectTest.cxx
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
// Warning about: identifier was truncated to '255' characters 
// in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4786 )
#endif

#ifdef ConnectObjectToRepresentationMacro
#undef ConnectObjectToRepresentationMacro
#endif

#define ConnectObjectToRepresentationMacro( object, representation ) \
  representation->RequestSetVideoFrameSpatialObject( object );

#include <iostream>
#include "igstkLandmark3DRegistration.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"
#include "itkObject.h"
#include "itkCommand.h"
#include "itkMacro.h"
#include "igstkEvents.h"
#include "igstkVideoFrameSpatialObject.h"
#include "igstkVideoFrameRepresentation.h"
#include "igstkSpatialObjectTestHelper.h"

int igstkVideoFrameSpatialObjectTest( int , char * [] )
{
  typedef igstk::VideoFrameSpatialObject<unsigned char, 3 >      VideoFrameObjectType;
  typedef igstk::VideoFrameRepresentation<VideoFrameObjectType>  VideoFrameRepresentationType;

  typedef igstk::SpatialObjectTestHelper<
    VideoFrameObjectType, VideoFrameRepresentationType > TestHelperType;

  //
  // The helper constructor intializes all the elements needed for the test.
  //
  TestHelperType  testHelper;

  //
  //  Tests that are specific to this type of SpatialObject
  //
  //
  //  None.
  //

  testHelper.TestRepresentationProperties();
  testHelper.ExercisePrintSelf();
  testHelper.TestTransform();
  testHelper.ExerciseDisplay();

 
  //testHelper.TestRepresentationCopy();
  testHelper.ExerciseScreenShot();

  return testHelper.GetFinalTestStatus();

  return EXIT_SUCCESS;
}
