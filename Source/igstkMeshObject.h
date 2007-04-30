/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMeshObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkMeshObject_h
#define __igstkMeshObject_h

#include "igstkSpatialObject.h"
#include <itkMeshSpatialObject.h>
#include <itkTetrahedronCell.h>
#include <itkTriangleCell.h>
#include <itkDefaultDynamicMeshTraits.h>

namespace igstk
{

namespace Friends 
{
class MeshReaderToMeshSpatialObject;
}

/** \class MeshObject
 *
 * \brief Implements the 3-dimensional mesh structure.
 *
 * Mesh implements the 3-dimensional mesh structure.  It provides
 * an API to perform operations on points, cells, boundaries, etc.
 * Mesh is an adaptive, evolving structure. Typically points and cells
 * are created, with the cells referring to their defining points.
 *
 * \ingroup Object
 */

class MeshObject 
: public SpatialObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( MeshObject, SpatialObject )

  /** Unlike DefaultStaticMeshTraits, the DefaultDynamicMeshTraits structure
   * is designed to create Mesh instances that will have many insert and delete
   * operations done on them. */
  typedef itk::DefaultDynamicMeshTraits<float,3,3>  MeshTrait;
  
  /** Type of the itk mesh taking three template parameters:
   *  First, the type stored as data for an entity (cell, point, or boundary).
   *  Second, the dimensionality of the mesh. and third, the type information
   *  structure for the mesh. */
  typedef itk::Mesh<float,3,MeshTrait>              MeshType;

  /** Type of the internal MeshSpatialObject. Templated of the itkMesh type */
  typedef itk::MeshSpatialObject<MeshType>          MeshSpatialObjectType;

  /** Type of the point used to describe the mesh */
  typedef MeshType::PointType                       PointType;
  typedef MeshType::CellTraits                      CellTraits;
  typedef itk::CellInterface< float, CellTraits >   CellInterfaceType;
  typedef itk::TetrahedronCell<CellInterfaceType>   TetraCellType;
  typedef itk::TriangleCell<CellInterfaceType>      TriangleCellType;
  typedef MeshType::CellType                        CellType;
  typedef CellType::CellAutoPointer                 CellAutoPointer;

  /** Type of containers used to store the points and cells */
  typedef MeshType::PointsContainer                 PointsContainer;
  typedef MeshType::PointsContainerPointer          PointsContainerPointer;
  typedef MeshType::CellsContainer                  CellsContainer;
  typedef MeshType::CellsContainerPointer           CellsContainerPointer;

public:

  /** Add a point to the mesh */
  bool AddPoint(unsigned int id,float x, float y,float z);

  /** Add a tetrahedron cell to the mesh */
  bool AddTetrahedronCell(unsigned int id,
                          unsigned int vertex1,unsigned int vertex2,
                          unsigned int vertex3,unsigned int vertex4);

  /** Add a triangle cell to the mesh */
  bool AddTriangleCell(unsigned int id,
                       unsigned int vertex1,
                       unsigned int vertex2,
                       unsigned int vertex3);

  /** Return the points */
  const PointsContainerPointer GetPoints() const;

  /** Return the cells */
  const CellsContainerPointer GetCells() const;

  /** The MeshReaderToMeshSpatialObject class is declared as a friend in
   * order to be able to set the input mesh */
  igstkFriendClassMacro( igstk::Friends::MeshReaderToMeshSpatialObject );

protected:

  /** Constructor */
  MeshObject( void );

  /** Destructor */
  ~MeshObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Set method to be invoked only by friends of this class */
  void SetMesh( MeshType * mesh );

  /** Internal itkSpatialObject */
  MeshSpatialObjectType::Pointer   m_MeshSpatialObject;
  MeshType::Pointer                m_Mesh;

};

} // end namespace igstk

#endif // __igstkMeshObject_h
