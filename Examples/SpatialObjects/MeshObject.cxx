/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    MeshObject.cxx
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
// \index{igstk::MeshObject}
// This example describes how to use the \doxygen{MeshObject} which implements
// a 3-dimensional mesh structure.  
// The mesh class provides an API to perform operations on points, 
// cells, boundaries, etc. Typically points and cells are created, with 
// the cells referring to their defining points.
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
#include "igstkMeshObject.h"
// Software Guide : EndCodeSnippet

int main( int , char *[] )
{
// Software Guide : BeginLatex
//
// First we declare the object using smart pointers.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef igstk::MeshObject MeshObjectType;
  MeshObjectType::Pointer mesh = MeshObjectType ::New();
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// A mesh is defined as a collection of point in space (x,y,z) referenced
// by an identification number.
// In order to add point we use the 
// AddPoint(unsigned int id,float x, float y,float z) function.
// Let's add 4 points in our mesh.
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
mesh->AddPoint(0,0,0,0);
mesh->AddPoint(1,0,10,0);
mesh->AddPoint(2,0,10,10);
mesh->AddPoint(3,10,0,10);
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// 
// We can retreive the list of points using the GetPoints()
// function.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
  typedef MeshObjectType::PointsContainer PointsContainer;
  typedef MeshObjectType::PointsContainerPointer PointsContainerPointer;
  PointsContainerPointer points = mesh->GetPoints();

  PointsContainer::const_iterator it = points->begin();
  while(it != points->end())
    {
    typedef MeshObjectType::PointType PointType;
    PointType pt = (*it).second;
    std::cout << "Point id = " << (*it).first << " : " 
              << pt[0] << "," << pt[1] << "," << pt[2] << std::endl;
    it++;
    }
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
// The next step is to define cells for the mesh. IGSTK currently supports
// two type of cells: Tetrahedron and Triangle:
// 
// \begin{verbatim}
//  bool AddTetrahedronCell(unsigned int id,
//                          unsigned int vertex1,unsigned int vertex2,
//                          unsigned int vertex3,unsigned int vertex4);
// 
//  bool AddTriangleCell(unsigned int id,
//                       unsigned int vertex1,
//                       unsigned int vertex2,
//                       unsigned int vertex3);
// \end{verbatim}
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
 mesh->AddTetrahedronCell(0,0,1,2,3);
 
 mesh->AddTriangleCell(1,0,1,2);
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// Software Guide : EndLatex
// Software Guide : BeginCodeSnippet
  typedef MeshObjectType::CellsContainer CellsContainer;
  typedef MeshObjectType::CellsContainerPointer CellsContainerPointer;
  CellsContainerPointer cells = mesh->GetCells();

  CellsContainer::const_iterator itCell = cells->begin();
  while(itCell != cells->end())
    {
    typedef MeshObjectType::CellType CellType;
    unsigned int cellId = (*itCell).first;
    itCell++;
    }
// Software Guide : EndCodeSnippet

  return EXIT_SUCCESS;
}
