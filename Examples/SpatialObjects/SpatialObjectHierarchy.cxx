/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    SpatialObjectHierarchy.cxx
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
// \index{igstk::SpatialObjectHierarchy}
// This example describes how to group \doxygen{SpatialObject}s to form an
// aggregate hierarchy of objects and also illustrates their creation and how
// to manipulate them.
//
// The first part of the example makes use of the \doxygen{EllipsoidObject}. The
// second part uses the \doxygen{GroupObject}.
// Let's start by including the appropriate header files:
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
#include "igstkEllipsoidObject.h"
#include "igstkGroupObject.h"
// Software Guide : EndCodeSnippet

igstkObserverMacro(TransformToParent,
                   ::igstk::CoordinateReferenceSystemTransformToEvent,
                   ::igstk::CoordinateReferenceSystemTransformToResult)

int main( int , char *[] )
{

// Software Guide : BeginLatex
//
// First, we create two spheres using the \doxygen{EllipsoidObject} class.
// They are created using smart pointers, as follows:
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef igstk::EllipsoidObject SpatialObjectType;

  SpatialObjectType::Pointer sphere1 = SpatialObjectType ::New();
  SpatialObjectType::Pointer sphere2 = SpatialObjectType ::New();

// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// We then set the second sphere to have a coordinate system
// relative to sphere1.
//
// We first create a transformation and set the translation vector to be $10mm$
// in each direction, with an error value of $0.001mm$ and a validity time
// of $10ms$. Second, we assign the transform to the object via the 
// \code{RequestSetTransformAndParent()} function. The commands appear as follows:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  igstk::Transform transformSphere2ToSphere1;
  transformSphere2ToSphere1.SetTranslation(10,0.001,10);
  sphere2->RequestSetTransformAndParent( transformSphere2ToSphere1, sphere1 );

// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// Then, in order to retrieve the transformation, we create an observer:
//
// \begin{verbatim}
// igstkObserverMacro(TransformToParent,
//                   ::igstk::CoordinateReferenceSystemTransformToEvent,
//                   ::igstk::CoordinateReferenceSystemTransformToResult)
// \end{verbatim}
// We then add the observer to the object using the 
// \code{AddObserver()} command:
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet

  TransformToParentObserver::Pointer transformToParentObserver 
                           = TransformToParentObserver::New();
        
  sphere2->AddObserver( ::igstk::CoordinateReferenceSystemTransformToEvent(), 
                        transformToParentObserver );

// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// 
// Then, we request the transform using the 
// \code{RequestGetTransformToParent()}
// command:
// 
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  sphere2->RequestGetTransformToParent();
  if( !transformToParentObserver->GotTransformToParent() )
    {
    std::cerr << "Sphere1 did not returned a Transform event" << std::endl;
    return EXIT_FAILURE;
    }
      
  igstk::Transform  transform2 = transformToParentObserver
                                    ->GetTransformToParent().GetTransform();
// Software Guide : EndCodeSnippet
  std::cout << "Received transform from sphere2 to parent sphere1 : " 
            << std::endl;
  std::cout << transform2 << std::endl;

// Software Guide : BeginLatex
// 
// Next, we introduce the \doxygen{GroupObject}. The GroupObject class
// derives from \doxygen{SpatialObject} and acts as an empty container
// used for grouping objects together.
//
// First, we declare a new group using standard type definitions and
// smart pointers, as follows:
//
// Software Guide : EndLatex 
// Software Guide : BeginCodeSnippet
  typedef igstk::GroupObject GroupType;
  GroupType::Pointer group = GroupType::New();
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
// 
// Since the \code{igstk::GroupObject} derives from \code{igstk::SpatialObject},
// we can use the \code{RequestAddObject()} function to add object into the
// group. For instance, we group \code{sphere1} and the newly created 
// \code{sphere3} together, as follows:
//
// Software Guide : EndLatex 
// Software Guide : BeginCodeSnippet

  igstk::Transform transformSphere1ToGroup;
  transformSphere1ToGroup.SetToIdentity( 
                  igstk::TimeStamp::GetLongestPossibleTime() );
  group->RequestAddChild( transformSphere1ToGroup, sphere1 );
  SpatialObjectType::Pointer sphere3 = SpatialObjectType::New();

  igstk::Transform transformSphere3ToGroup;
  transformSphere3ToGroup.SetToIdentity(
                  igstk::TimeStamp::GetLongestPossibleTime() );                
  group->RequestAddChild( transformSphere3ToGroup, sphere3 );
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
// 
// We can request the number of objects in the group using the
// \code{GetNumberOfChildren()} function.
// Since \code{sphere1} has a child, \code{sphere2}, there are actually
// three objects in the group.
//
// Software Guide : EndLatex 
// Software Guide : BeginCodeSnippet
  std::cout << "Number of object in my group: " 
            << group->GetNumberOfChildren() << std::endl;
// Software Guide : EndCodeSnippet
  if(group->GetNumberOfChildren() != 2)
    {
    std::cout << "group should have 2 objects and got: " 
              << group->GetNumberOfChildren() << std::endl;
    }
  return EXIT_SUCCESS;
}
