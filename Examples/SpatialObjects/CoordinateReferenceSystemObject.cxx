/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    CoordinateReferenceSystemObject.cxx
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

// Software Guide : BeginLatex
//
//
// First, we include the appropriate header file:
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
#include "igstkCoordinateReferenceSystemObject.h"
#include "igstkWorldCoordinateReferenceSystemObject.h"
// Software Guide : EndCodeSnippet

int main( int , char *[] )
{
// Software Guide : BeginLatex
// We then declare the object using smart pointers, as follows:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef igstk::WorldCoordinateReferenceSystemObject WorldReferenceSystemType;
  WorldReferenceSystemType::Pointer worldReference = 
     WorldReferenceSystemType ::New();
// Software Guide : EndCodeSnippet

// Software Guide : BeginCodeSnippet
  typedef igstk::CoordinateReferenceSystemObject ReferenceSystemType;
  ReferenceSystemType::Pointer referenceA = ReferenceSystemType ::New();
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// The reference system must be attached to the
// WorldCoordinateReferenceSystemObject.  
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  referenceA->RequestAttachToSpatialObjectParent( worldReference );
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// 
// The relative position to the parent is stored as a Transform.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  const double validityTimeInMilliseconds = 20000.0; // 20 seconds

  igstk::Transform::VectorType translation;
  translation[0] = 0;
  translation[1] = 1;
  translation[2] = 2;
  igstk::Transform::VersorType rotation;
  rotation.Set( 0.707, 0.0, 0.707, 0.0 );

  igstk::Transform::ErrorType errorValue = 0.01; // 10 microns

  igstk::Transform transform;

  transform.SetTranslationAndRotation( 
      translation, rotation, errorValue, validityTimeInMilliseconds );

  referenceA->RequestSetTransform( transform );
// Software Guide : EndCodeSnippet

  return EXIT_SUCCESS;
}
