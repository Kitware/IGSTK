/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    CoordinateSystemObject.cxx
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
// First, we include the appropriate header files. In this case, it is enough
// to use any spatial objects.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
#include "igstkConeObject.h"
#include "igstkCylinderObject.h"
// Software Guide : EndCodeSnippet

int main( int , char *[] )
{
// Software Guide : BeginLatex
// We then declare the object using smart pointers, as follows:
//
// Software Guide : EndLatex 


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

// Software Guide : EndCodeSnippet

  typedef igstk::ConeObject ConeObjectType;
  ConeObjectType::Pointer cone = ConeObjectType ::New();

  cone->SetRadius(10.0);
  cone->SetHeight(20.0);

  double validityTimeInMilliseconds2 = 200.0;

  translation[0] = 10.0;
  translation[1] =  0.0;
  translation[2] =  0.0;
  rotation.Set( -0.707, 0.0, 0.707, 0.0 );

  transform.SetTranslationAndRotation( 
      translation, rotation, errorValue, validityTimeInMilliseconds2 );

  typedef igstk::CylinderObject CylinderObjectType;
  CylinderObjectType::Pointer cylinder = CylinderObjectType ::New();

  cylinder->SetRadius(10.0);
  cylinder->SetHeight(20.0);

  cylinder->RequestSetTransformAndParent( transform, cone );

  translation[0] = -10.0;
  translation[1] =   0.0;
  translation[2] =   0.0;
  rotation.Set( 0.707, 0.0, 0.707, 0.0 );

  transform.SetTranslationAndRotation( 
      translation, rotation, errorValue, validityTimeInMilliseconds2 );

  cylinder->RequestSetTransformAndParent( transform, cone );

  std::cout << "Cone " << std::endl;
  cone->Print( std::cout );

  std::cout << "Cylinder " << std::endl;
  cylinder->Print( std::cout );

  return EXIT_SUCCESS;
}
