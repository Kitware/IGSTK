/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMeshObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkMeshObject_h
#define __igstkMeshObject_h

#include "igstkMacros.h"
#include "igstkSpatialObject.h"
#include <itkMeshSpatialObject.h>
#include <itkTetrahedronCell.h>
#include <itkDefaultDynamicMeshTraits.h>

namespace igstk
{

/** \class MeshObject
 * 
 * \brief This class represents a Mesh object. The parameters of the object
 * are the height of the object, and the radius. Default representation axis is X.
 * 
 * \ingroup Object
 */

class MeshObject 
: public SpatialObject
{

public:

  /** Typedefs */
  typedef MeshObject                                Self;
  typedef SpatialObject                             Superclass;
  typedef itk::SmartPointer<Self>                   Pointer;
  typedef itk::SmartPointer<const Self>             ConstPointer;
  typedef itk::DefaultDynamicMeshTraits<float,3,3>  MeshTrait;
  typedef itk::Mesh<float,3,MeshTrait>              MeshType;
  typedef MeshType::PointType                       PointType;
  typedef itk::MeshSpatialObject<MeshType>          MeshSpatialObjectType;
  typedef MeshType::CellTraits                      CellTraits;
  typedef itk::CellInterface< float, CellTraits >   CellInterfaceType;
  typedef itk::TetrahedronCell<CellInterfaceType>   TetraCellType;
  typedef MeshType::CellType                        CellType;
  typedef CellType::CellAutoPointer                 CellAutoPointer;
  typedef MeshType::PointsContainer                 PointsContainer;
  typedef MeshType::PointsContainerPointer          PointsContainerPointer;
  typedef MeshType::CellsContainer                  CellsContainer;
  typedef MeshType::CellsContainerPointer           CellsContainerPointer;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro( MeshObject, SpatialObject );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( MeshObject );
  
  /** Add a point to the mesh */
  bool AddPoint(unsigned int id,float x, float y,float z);

  /** Add a tetrahedron cell to the mesh */
  bool AddTetrahedronCell(unsigned int id,unsigned int vertex1,unsigned int vertex2,unsigned int vertex3,unsigned int vertex4);

  /** Return the points */
  PointsContainerPointer GetPoints() const;

  /** Return the cells */
  CellsContainerPointer GetCells() const;

protected:

  MeshObject( void );
  ~MeshObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Internal itkSpatialObject */
  MeshSpatialObjectType::Pointer   m_MeshSpatialObject;
  MeshType::Pointer                m_Mesh;

};

} // end namespace igstk

#endif // __igstkMeshObject_h

