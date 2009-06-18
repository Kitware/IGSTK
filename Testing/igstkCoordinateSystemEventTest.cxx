/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCoordinateSystemEventTest.cxx
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

#include <iostream>

#include "igstkCoordinateSystem.h"
#include "igstkCoordinateSystemTransformToResult.h"
#include "igstkCoordinateSystemTransformToErrorResult.h"
#include "igstkCoordinateSystemSetTransformResult.h"


int igstkCoordinateSystemEventTest( int, char * [] )
{
  /** This test is mainly for code coverage on the event results. */
  typedef igstk::CoordinateSystem              CoordinateSystemType;

  typedef igstk::CoordinateSystemTransformToResult         TransformToResult;
  typedef igstk::CoordinateSystemTransformToErrorResult    ErrorResult;
  typedef igstk::CoordinateSystemSetTransformResult        SetTransformResult;
  typedef igstk::Transform                                 TransformType;

  CoordinateSystemType::Pointer cs1 = CoordinateSystemType::New();
  CoordinateSystemType::Pointer cs2 = CoordinateSystemType::New();

  TransformType transform;

  TransformToResult transformToResult;

  // Test Initialize
  transformToResult.Initialize( transform, cs1, cs2 );

  // Copy ctor
  TransformToResult transformToResult2 = transformToResult;

  // Assignment operator
  transformToResult = transformToResult2;

  // GetTransform
  transformToResult.GetTransform();

  // GetSource
  transformToResult.GetSource();

  // GetDestination
  transformToResult.GetDestination();

  // Clear
  transformToResult.Clear();

  // Error result
  ErrorResult errorResult;

  // Test Initialize
  errorResult.Initialize( cs1, cs2 );

  // Copy ctor
  ErrorResult errorResult2 = errorResult;

  // Assignment operator
  errorResult = errorResult2;

  // GetSource
  errorResult.GetSource();

  // GetDestination
  errorResult.GetDestination();

  // Clear
  errorResult.Clear();

  // Set transform result
  SetTransformResult stResult;
  bool attaching = true;

  // Test Initialize
  stResult.Initialize( transform, cs1, cs2, attaching );

  // Copy ctor
  SetTransformResult stResult2 = stResult;

  // Assignment operator
  stResult = stResult2;

  // GetTransform
  stResult.GetTransform();

  // GetSource
  stResult.GetSource();

  // GetDestination
  stResult.GetDestination();

  // Is attaching?
  if (! stResult.IsAttach())
    {
    return EXIT_FAILURE;
    }

  // Clear
  stResult.Clear();

  return EXIT_SUCCESS;
}
