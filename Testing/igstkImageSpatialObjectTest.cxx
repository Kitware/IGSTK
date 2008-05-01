/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageSpatialObjectTest.cxx
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
#pragma warning ( disable : 4786 )
#endif

#ifdef ConnectObjectToRepresentationMacro
#undef ConnectObjectToRepresentationMacro
#endif

#define ConnectObjectToRepresentationMacro( object, representation ) \
  representation->RequestSetImageSpatialObject( object );

#include "igstkImageSpatialObject.h"
#include "igstkImageSpatialObjectRepresentation.h"
#include "igstkSpatialObjectTestHelper.h"


int igstkImageSpatialObjectTest( int , char* [] )
{

  typedef signed short    PixelType;
  const unsigned int      Dimension = 3;

  typedef igstk::ImageSpatialObject< PixelType, Dimension >     ObjectType;

  typedef igstk::ImageSpatialObjectRepresentation< 
                                               ObjectType > RepresentationType;

  typedef igstk::SpatialObjectTestHelper<
    ObjectType, RepresentationType > TestHelperType;

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

 
  testHelper.TestRepresentationCopy();
  testHelper.ExerciseScreenShot();

  return testHelper.GetFinalTestStatus();

  return EXIT_SUCCESS;
}
