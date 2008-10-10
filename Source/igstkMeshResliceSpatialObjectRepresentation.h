/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMeshResliceSpatialObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkMeshResliceSpatialObjectRepresentation_h
#define __igstkMeshResliceSpatialObjectRepresentation_h

#include "igstkMacros.h"
#include "igstkTransform.h"
#include "igstkMeshObject.h"
#include "igstkStateMachine.h"
#include "igstkObjectRepresentation.h"

class vtkPlane;
class vtkCutter;
class vtkTubeFilter;

namespace igstk
{

/** \class MeshResliceSpatialObjectRepresentation
 * 
 * \brief This class represents a Mesh object.
 * 
 *
 * \ingroup ObjectRepresentation
 */
template < class TImageSpatialObject >
class MeshResliceSpatialObjectRepresentation 
: public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardTemplatedClassTraitsMacro( MeshResliceSpatialObjectRepresentation, 
                                 ObjectRepresentation )

public:
    
 /** Typedefs */
  typedef TImageSpatialObject                      ImageSpatialObjectType;

  typedef typename ImageSpatialObjectType::ConstPointer 
                                                   ImageSpatialObjectConstPointer;

  typedef typename ImageSpatialObjectType::PointType PointType;

  typedef MeshObject                               MeshObjectType;
  typedef igstk::Transform::VectorType             VectorType;

  /** Type for representing the line width of the contour */
  typedef   double                                 LineWidthType;

  typedef ImageReslicePlaneSpatialObject< ImageSpatialObjectType >
                                                    ReslicePlaneSpatialObjectType;

  typedef typename ReslicePlaneSpatialObjectType::Pointer
                                              ReslicePlaneSpatialObjectPointer;  

//  typedef ToolProjectionObject                 ToolProjectionSpatialObjectType;

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetMeshObject( const MeshObjectType * MeshObject );

  /** Sets visibility */
  void SetVisibility(bool visibility);

  /** Sets cutting plane parameters */
  //void RequestSetPlaneNormal(Transform::VectorType &normal);

  //void RequestSetPlaneCenter(Transform::VectorType &center);

  /** Sets the driving plane */
  void RequestSetReslicePlaneSpatialObject( const ReslicePlaneSpatialObjectType *
                                                             planeSpatialObject);
  /** Set/Get the opacity */
  void SetLineWidth(LineWidthType LineWidth);
  igstkGetMacro( LineWidth, LineWidthType );

 // void RequestSetResliceAxes( vtkMatrix4x4 *matrix );

protected:

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 
  
  /** Constructor */
  MeshResliceSpatialObjectRepresentation( void );

  /** Destructor */
  ~MeshResliceSpatialObjectRepresentation( void );

  /** Create the VTK actors */
  void CreateActors();

  /** Verify time stamp. Use the reslicing tool transform to verify 
  * the time stamp */
  virtual bool VerifyTimeStamp() const;

private:
  
  LineWidthType                          m_LineWidth;

  /** Declare the observer that will receive a VTK plane source from the
   * ImageResliceSpatialObject */
  igstkObserverMacro( VTKPlane, VTKPlaneModifiedEvent,
                      EventHelperType::VTKPlaneSourcePointerType);

  typename VTKPlaneObserver::Pointer  m_VTKPlaneObserver;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetMeshObjectProcessing(); 

  /** Null operation for a State Machine transition */
  void NoProcessing();
 
  /** Reslice processing */
  //void ResliceProcessing();

  /** Set reslice plane spatial object processing */
  void SetReslicePlaneSpatialObjectProcessing();

  /** Sets the vtkPlaneSource from the ImageReslicePlaneSpatialObject object. 
  * This method MUST be private in order to prevent unsafe access from the 
  * VTK plane source layer. */
  void SetPlane( const vtkPlaneSource * plane );

private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidMeshObject );
  igstkDeclareInputMacro( NullMeshObject );
  igstkDeclareInputMacro( ValidReslicePlaneSpatialObject );
  igstkDeclareInputMacro( InValidReslicePlaneSpatialObject );

  igstkDeclareInputMacro( EmptyImageSpatialObject ); 
 // igstkDeclareInputMacro( Reslice );

  /** States for the State Machine */
  igstkDeclareStateMacro( NullMeshObject );
  igstkDeclareStateMacro( ValidMeshObject );
  igstkDeclareStateMacro( ValidReslicePlaneSpatialObject );

  /** Variables for managing the mesh spatial object */
  MeshObjectType::ConstPointer   m_MeshObject;
  MeshObjectType::ConstPointer   m_MeshObjectToBeSet;

  /** Variables for managing reslice plane spatial object */
  ReslicePlaneSpatialObjectPointer  m_ReslicePlaneSpatialObjectToBeSet;
  ReslicePlaneSpatialObjectPointer  m_ReslicePlaneSpatialObject;

  /** Plane defining the contour */
  vtkPlane*  m_Plane;
  vtkCutter* m_Cutter;
  vtkTubeFilter* m_Tuber;
  //vtkMatrix4x4* m_ResliceAxes;
};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkMeshResliceSpatialObjectRepresentation.txx"
#endif

#endif // __igstkContourMeshObjectRepresentation_h
