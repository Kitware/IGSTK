/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkObliqueContourMeshObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkObliqueContourMeshObjectRepresentation_h
#define __igstkObliqueContourMeshObjectRepresentation_h

#include "igstkMacros.h"
#include "igstkMeshObject.h"
#include "igstkStateMachine.h"
#include "igstkObjectRepresentation.h"

#include <vtkPlane.h>
#include <vtkCutter.h>

namespace igstk
{

/** \class ObliqueContourMeshObjectRepresentation
 * 
 * \brief This class represents a Mesh object.
 * 
 *
 * \ingroup ObjectRepresentation
 */

class ObliqueContourMeshObjectRepresentation 
: public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( ObliqueContourMeshObjectRepresentation, 
                                 ObjectRepresentation )

public:
    
  /** Typedefs */
  typedef MeshObject               MeshObjectType;
  typedef itk::Point<double,3>     PointType;
  typedef itk::Vector<double,3>    VectorType;

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetMeshObject( const MeshObjectType * MeshObject );

  /** Request to set origin point on the plane */
  void RequestSetOriginPointOnThePlane( const PointType & point);

  /** Request to set vector 1 on the plane */
  void RequestSetVector1OnThePlane( const VectorType & vector );

  /** Request to set vector 2 on the plane */
  void RequestSetVector2OnThePlane( const VectorType & vector );

  /** Request reslice a 3D image */
  void RequestReslice();

protected:

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 
  
  /** Constructor */
  ObliqueContourMeshObjectRepresentation( void );

  /** Destructor */
  ~ObliqueContourMeshObjectRepresentation( void );

  /** Create the VTK actors */
  void CreateActors();

private:

  /** Internal itkSpatialObject */
  MeshObjectType::ConstPointer   m_MeshObject;

  /** Oblique plane parameters */
  PointType                              m_OriginPointOnThePlane;
  PointType                              m_OriginPointOnThePlaneToBeSet;

  VectorType                             m_Vector1OnThePlane;
  VectorType                             m_Vector1OnThePlaneToBeSet;

  VectorType                             m_Vector2OnThePlane;
  VectorType                             m_Vector2OnThePlaneToBeSet;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetMeshObjectProcessing(); 

  /** Null operation for a State Machine transition */
  void NoProcessing();

  /** Set the origin point on the plane */
  void SetOriginPointOnThePlaneProcessing();
  
  /** Set vector 1 on the plane */
  void SetVector1OnThePlaneProcessing();
 
  /** Set vector 2 on the plane */
  void SetVector2OnThePlaneProcessing();
 
  /** Reslice processing */
  void ResliceProcessing();

private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidMeshObject );
  igstkDeclareInputMacro( NullMeshObject );
  igstkDeclareInputMacro( ValidOriginPointOnThePlane );
  igstkDeclareInputMacro( InValidOriginPointOnThePlane );

  igstkDeclareInputMacro( ValidVector1OnThePlane );
  igstkDeclareInputMacro( InValidVector1OnThePlane );
 
  igstkDeclareInputMacro( ValidVector2OnThePlane );
  igstkDeclareInputMacro( InValidVector2OnThePlane );

  igstkDeclareInputMacro( EmptyImageSpatialObject ); 
  igstkDeclareInputMacro( Reslice );

  /** States for the State Machine */
  igstkDeclareStateMacro( NullMeshObject );
  igstkDeclareStateMacro( ValidMeshObject );

  igstkDeclareStateMacro( NullOriginPointOnThePlane );
  igstkDeclareStateMacro( ValidOriginPointOnThePlane );
  
  igstkDeclareStateMacro( NullVector1OnthePlane );
  igstkDeclareStateMacro( ValidVector1OnThePlane );

  igstkDeclareStateMacro( NullVector2OnthePlane );
  igstkDeclareStateMacro( ValidVector2OnThePlane );

  MeshObjectType::ConstPointer m_MeshObjectToAdd;

  /** Plane defining the contour */
  vtkPlane*  m_Plane;
  vtkCutter* m_Cutter;
};


} // end namespace igstk

#endif // __igstkContourMeshObjectRepresentation_h
