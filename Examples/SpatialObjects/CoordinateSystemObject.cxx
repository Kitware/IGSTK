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
// This example shows the basic techniques for working with
// coordinate system objects. 
//
// Software Guide : EndLatex 

// Software Guide : BeginLatex
// 
// First, we include the appropriate header files. 
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
#include "igstkConeObject.h"
#include "igstkCylinderObject.h"
// Software Guide : EndCodeSnippet

int main( int , char *[] )
{

// Software Guide : BeginLatex
// 
// Then we construct a cone object and set the radius and height.
//
// Software Guide : EndLatex 


// Software Guide : BeginCodeSnippet
  typedef igstk::ConeObject ConeObjectType;
  ConeObjectType::Pointer cone = ConeObjectType ::New();

  cone->SetRadius(10.0);
  cone->SetHeight(20.0);
// Software Guide : EndCodeSnippet


// Software Guide : BeginLatex
// 
// We construct a transform which will hold the
// relative position to the parent. 
// We also set some initial values.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  const double validityTimeInMilliseconds = 200.0;

  igstk::Transform::VectorType translation;
  translation[0] = 10.0;
  translation[1] =  0.0;
  translation[2] =  0.0;
  igstk::Transform::VersorType rotation;
  rotation.Set( -0.707, 0.0, 0.707, 0.0 );

  igstk::Transform::ErrorType errorValue = 0.01; // 10 microns

  igstk::Transform transform;

  transform.SetTranslationAndRotation( 
      translation, rotation, errorValue, validityTimeInMilliseconds );

// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// 
// Next, we construct a cylinder object with a radius and height.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet

  typedef igstk::CylinderObject CylinderObjectType;
  CylinderObjectType::Pointer cylinder = CylinderObjectType ::New();

  cylinder->SetRadius(10.0);
  cylinder->SetHeight(20.0);
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// Then, we build a very simple scene graph by connecting the 
// cylinder to the cone. The cone is the parent and transform
// specifies the transformation from the cylinder's coordinates
// into the cone's coordinates.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
  cylinder->RequestSetTransformAndParent( transform, cone );
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// Now we can modify the transform. 
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
  translation[0] = -10.0;
  translation[1] =   0.0;
  translation[2] =   0.0;
  rotation.Set( 0.707, 0.0, 0.707, 0.0 );

  transform.SetTranslationAndRotation( 
      translation, rotation, errorValue, validityTimeInMilliseconds );
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// And then we can reconnect the cylinder to the cone with the new transform.
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
  cylinder->RequestSetTransformAndParent( transform, cone );
// Software Guide : EndCodeSnippet

  std::cout << "Cone " << std::endl;
  cone->Print( std::cout );

  std::cout << "Cylinder " << std::endl;
  cylinder->Print( std::cout );

  return EXIT_SUCCESS;
}
