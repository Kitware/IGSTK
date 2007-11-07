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
// FIXCS #include "igstkCoordinateReferenceSystemObject.h"
// FIXCS #include "igstkWorldCoordinateReferenceSystemObject.h"
// Software Guide : EndCodeSnippet

#include "igstkConeObject.h"
#include "igstkCylinderObject.h"

int main( int , char *[] )
{
// Software Guide : BeginLatex
// We then declare the object using smart pointers, as follows:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  /* FIXCS
  typedef igstk::WorldCoordinateReferenceSystemObject WorldReferenceSystemType;
  WorldReferenceSystemType::Pointer worldReference = 
     WorldReferenceSystemType ::New();
   */
// Software Guide : EndCodeSnippet

// Software Guide : BeginCodeSnippet
  // FIXCS typedef igstk::CoordinateReferenceSystemObject ReferenceSystemType;
  // FIXCS ReferenceSystemType::Pointer referenceA = ReferenceSystemType ::New();
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// The reference system must be attached to the
// WorldCoordinateReferenceSystemObject.  
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  // FIXCS referenceA->RequestAttachToSpatialObjectParent( worldReference );
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// 
// The relative position to the parent is stored as a Transform.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  const double validityTimeInMilliseconds = 20000.0; // 20 seconds

  igstk::Transform::VectorType translation;
  translation[0] =  0.0;
  translation[1] = 10.0;
  translation[2] =  0.0;
  igstk::Transform::VersorType rotation;
  rotation.Set( 0.0, 0.0, 0.0, 1.0 );

  igstk::Transform::ErrorType errorValue = 0.01; // 10 microns

  igstk::Transform transform;

  transform.SetTranslationAndRotation( 
      translation, rotation, errorValue, validityTimeInMilliseconds );

  // FIXCS referenceA->RequestSetTransform( transform );
// Software Guide : EndCodeSnippet

  typedef igstk::ConeObject ConeObjectType;
  ConeObjectType::Pointer cone = ConeObjectType ::New();

  cone->SetRadius(10.0);
  cone->SetHeight(20.0);

  // FIXCS cone->RequestAttachToSpatialObjectParent( referenceA );

  double validityTimeInMilliseconds2 = 200.0;

  translation[0] = 10.0;
  translation[1] =  0.0;
  translation[2] =  0.0;
  rotation.Set( -0.707, 0.0, 0.707, 0.0 );

  transform.SetTranslationAndRotation( 
      translation, rotation, errorValue, validityTimeInMilliseconds2 );

#ifdef USE_SPATIAL_OBJECT_DEPRECATED  
  cone->RequestSetTransform( transform );
#endif

  typedef igstk::CylinderObject CylinderObjectType;
  CylinderObjectType::Pointer cylinder = CylinderObjectType ::New();

  cylinder->SetRadius(10.0);
  cylinder->SetHeight(20.0);

  // FIXCS cylinder->RequestAttachToSpatialObjectParent( referenceA );

  translation[0] = -10.0;
  translation[1] =   0.0;
  translation[2] =   0.0;
  rotation.Set( 0.707, 0.0, 0.707, 0.0 );

  transform.SetTranslationAndRotation( 
      translation, rotation, errorValue, validityTimeInMilliseconds2 );

#ifdef USE_SPATIAL_OBJECT_DEPRECATED  
  cylinder->RequestSetTransform( transform );
  cone->RequestGetTransform();
  cylinder->RequestGetTransform();
#endif

  std::cout << "Reference System A " << std::endl;
  // FIXCS referenceA->Print( std::cout );

  std::cout << "Cone " << std::endl;
  cone->Print( std::cout );

  std::cout << "Cylinder " << std::endl;
  cylinder->Print( std::cout );

  return EXIT_SUCCESS;
}
