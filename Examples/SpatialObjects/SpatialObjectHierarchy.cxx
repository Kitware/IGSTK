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
                   ::igstk::CoordinateSystemTransformToEvent,
                   ::igstk::CoordinateSystemTransformToResult)

int main( int , char *[] )
{

// Software Guide : BeginLatex
// First, we create two spheres, sphere1 and sphere2 using the \doxygen{EllipsoidObject} class.
// They are created using smart pointers, as follows:
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef igstk::EllipsoidObject SpatialObjectType;

  SpatialObjectType::Pointer sphere1 = SpatialObjectType ::New();
  SpatialObjectType::Pointer sphere2 = SpatialObjectType ::New();

// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// We create a transformation between sphere1 and sphere2 and set the 
// translation vector to be $10mm$ in each direction, with an error value of 
// $0.001mm$ and a validity time of $10ms$. 
// Then we attach sphere2 as a child of sphere1 and we set the relative 
// transform between the two objects via the
// \code{RequestSetTransformAndParent()} function. The commands appear as
// follows:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  igstk::Transform transformSphere2ToSphere1;
  transformSphere2ToSphere1.SetTranslation(10,0.001,10);
  sphere2->RequestSetTransformAndParent( transformSphere2ToSphere1, sphere1 );

// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// In order to retrieve a transformation between an object and its parent, an 
// observer should be created using the igstkObserverMacro. The first parameter
// of the macro is the name of the observer, the second is the type of event
// to observe and the third parameter is the expected result type. 
// Then we instantiate the observer using smart pointers.
//
// \begin{verbatim}
// igstkObserverMacro(TransformToParent,
//                   ::igstk::CoordinateSystemTransformToEvent,
//                   ::igstk::CoordinateSystemTransformToResult)
// \end{verbatim}
//
// Note that the declaration of the observer should be done outside 
// of the class. This macro will create two functions 
// depending on the name of the first argument:
// \begin{enumerate}
// \item \code{GotTransformToParent()} which returns true if the transform exists.
// \item \code{GetTransformToParent()} which returns the actual pointer to the transform.
// \end{enumerate}
//
// We create the observer of the transformation using smart pointers.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet

  TransformToParentObserver::Pointer transformToParentObserver 
                           = TransformToParentObserver::New();
                           
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// We add the observer to the object using the \code{AddObserver()} command.
// The first argument specifies the type of event to observe and the second
// argument is the observer instantiated previously.
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet

  sphere2->AddObserver( ::igstk::CoordinateSystemTransformToEvent(), 
                        transformToParentObserver );

// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// 
// Then, we request the transform using the \code{RequestGetTransformToParent()}
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
// Next, we introduce the \doxygen{GroupObject}. The \doxygen{GroupObject} class
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
// we can use the \code{RequestAddChild()} function to add object into the
// group. This function expects the relative transform between the two objects as
// the first argument. For instance, we group the previously created \code{sphere1} 
// and the newly created \code{sphere3} together, as follows:
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
// \code{GetNumberOfChildren()} function. This function returns
// only the number of first level children. In this case we have two
// children in the group, even if sphere2 is a child of shpere1.
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
