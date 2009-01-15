/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    MeshObject.cxx
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
// \index{igstk::MeshObject}
// This example describes how to use the \doxygen{MeshObject}, which implements
// a 3D mesh structure.  
// The mesh class provides an API to perform operations on points and
// cells. Typically, points and cells are created, with 
// the cells referring to their defining points.
// 
// Let's include the header file first:
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
#include "igstkMeshObject.h"
// Software Guide : EndCodeSnippet

int main( int , char *[] )
{
// Software Guide : BeginLatex
//
// Then, we declare the object using smart pointers:
//
// Software Guide : EndLatex 

// Software Guide : BeginCodeSnippet
  typedef igstk::MeshObject MeshObjectType;
  MeshObjectType::Pointer mesh = MeshObjectType ::New();
// Software Guide : EndCodeSnippet

// Software Guide : BeginLatex
//
// A mesh is defined as a collection of 3D points (x,y,z) 
// in space referenced by an identification number (ID).
// In order to add points to the mesh structure, we use the 
// \code{AddPoint(unsigned int id,float x, float y,float z)} function.
// Let's add 4 points with consecutive IDs starting at zero, to our mesh:
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
// The list of points can be retrieve using the \code{GetPoints()}
// function. The list of points returned consist of a list of std::pair
// with the first argument of the pair being the id of the point and
// the second argument storing the geometrical representation.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
  typedef MeshObjectType::PointsContainer        PointsContainer;
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
// two type of cells: tetrahedron and triangle cells. The functions for adding
// cells to the mesh are defined as follows, where the vertices referes to the ID
// of the points previously defined.
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
// Let's add one tetrahedral cell and one triangle cell to the mesh.
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
 mesh->AddTetrahedronCell(0,0,1,2,3);
 
 mesh->AddTriangleCell(1,0,1,2);
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// We can then retrieve the cells using \code{GetCells()}.
// This function returns a list of cells, as follows:
//
// Software Guide : EndLatex
// Software Guide : BeginCodeSnippet
  typedef MeshObjectType::CellsContainer        CellsContainer;
  typedef MeshObjectType::CellsContainerPointer CellsContainerPointer;
  CellsContainerPointer cells = mesh->GetCells();

  CellsContainer::const_iterator itCell = cells->begin();
  while(itCell != cells->end())
    {
    typedef MeshObjectType::CellType CellType;
    unsigned int cellId = (*itCell).first;
    std::cout << "Cell ID: " << cellId << std::endl;
    itCell++;
    }
// Software Guide : EndCodeSnippet

  return EXIT_SUCCESS;
}
