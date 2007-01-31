/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    SpatialObjectHierarchy.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

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

igstkObserverMacro(Transform,
    ::igstk::TransformModifiedEvent,::igstk::Transform)

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
// We then add the second sphere to the first one by using the
// \code{RequestAddObject()} method.  As a result, \code{sphere2} becomes a
// child of \code{sphere1}:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet 
  sphere1->RequestAddObject(sphere2);
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// A child object can be retrieved from its parent 
// by using the \code{GetObject()} function, as follows:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  sphere1->GetObject(0)->Print(std::cout);
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// Next, we assign a transformation to the object.
// We first create a transformation and set the translation vector to be $10mm$
// in each direction, with an error value of $0.001mm$ and a validity time
// of $10ms$. Second, we assign the transform to the object via the 
// \code{RequestSetTransform()} function. The commands appear as follows:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  igstk::Transform transform;
  transform.SetTranslation(10,0.001,10);
  sphere1->RequestSetTransform(transform);
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// Then, in order to retrieve the transformation, we create an observer:
//
// \begin{verbatim}
//   igstkObserverMacro(Transform,
//   ::igstk::TransformModifiedEvent,::igstk::Transform)
// \end{verbatim}
// We then add the observer to the object using the 
// \code{AddObserver()} command:
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  TransformObserver::Pointer transformObserver 
                           = TransformObserver::New();
  sphere1->AddObserver( ::igstk::TransformModifiedEvent(), transformObserver );
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// 
// Then, we request the transform using the \code{RequestGetTransform()}
// command:
// 
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  sphere1->RequestGetTransform();
  if( !transformObserver->GotTransform() )
    {
    std::cerr << "Sphere1 did not returned a Transform event" << std::endl;
    return EXIT_FAILURE;
    }
      
  igstk::Transform  transform2 = transformObserver->GetTransform();
// Software Guide : EndCodeSnippet
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
  group->RequestAddObject(sphere1);
  SpatialObjectType::Pointer sphere3 = SpatialObjectType::New();
  group->RequestAddObject(sphere3);
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
// 
// We can request the number of objects in the group using the
// \code{GetNumberOfObjects()} function.
// Since \code{sphere1} has a child, \code{sphere2}, there are actually
// three objects in the group.
//
// Software Guide : EndLatex 
// Software Guide : BeginCodeSnippet
  std::cout << "Number of object in my group: " 
            << group->GetNumberOfObjects() << std::endl;
// Software Guide : EndCodeSnippet
  if(group->GetNumberOfObjects() != 3)
    {
    std::cout << "group should have 3 objects and got: " 
              << group->GetNumberOfObjects() << std::endl;
    }
  return EXIT_SUCCESS;
}
