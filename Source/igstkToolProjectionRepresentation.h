/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkToolProjectionRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkToolProjectionRepresentation_h
#define __igstkToolProjectionRepresentation_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkToolProjectionObject.h"
#include "igstkImageReslicePlaneSpatialObject.h"
#include "igstkStateMachine.h"

class vtkLineSource;
class vtkTubeFilter;
class vtkPolyDataMapper;

namespace igstk
{

/** \class ToolProjectionRepresentation
 * 
 * \brief This class provides a visual representation of a ToolProjectionObject. 
 *
 * RequestSetReslicePlaneSpatialObject sets the driving plane onto which the tool´s long axis
 * is projected.
 *
 * \sa ToolProjectionObject
 *
 * \ingroup ObjectRepresentation
 */
template < class TImageSpatialObject >
class ToolProjectionRepresentation : public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardTemplatedClassTraitsMacro( ToolProjectionRepresentation, 
                                 ObjectRepresentation )

public:

  /** Typedefs */
  typedef TImageSpatialObject                  ImageSpatialObjectType;

  typedef typename ImageSpatialObjectType::ConstPointer 
                                               ImageSpatialObjectConstPointer;

  typedef typename ImageSpatialObjectType::PointType  PointType;

  typedef ImageReslicePlaneSpatialObject< ImageSpatialObjectType >
                                                   ReslicePlaneSpatialObjectType;

  typedef typename ReslicePlaneSpatialObjectType::Pointer
                                              ReslicePlaneSpatialObjectPointer;  

  typedef ToolProjectionObject                 ToolProjectionSpatialObjectType;

  void RequestSetReslicePlaneSpatialObject( const ReslicePlaneSpatialObjectType *
                                                             planeSpatialObject);

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetToolProjectionObject( const ToolProjectionSpatialObjectType * ToolProjectionObject );

  virtual void SetVisibility ( bool visible);

protected:

  ToolProjectionRepresentation( void );
  virtual ~ToolProjectionRepresentation( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Create the VTK actors */
  void CreateActors();

  /** Verify time stamp. Use the reslicing tool transform to verify 
  * the time stamp */
  virtual bool VerifyTimeStamp() const;

private:

  ToolProjectionRepresentation(const Self&); //purposely not implemented
  void operator=(const Self&);     //purposely not implemented

  
  /** Internal itkSpatialObject */
  ToolProjectionSpatialObjectType::ConstPointer   m_ToolProjectionSpatialObject;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetToolProjectionObjectProcessing();

  /** Method for performing a null operation during a State Machine 
   *  transition */
  void NoProcessing();

  /** Set the reslice plane spatial object */
  void SetReslicePlaneSpatialObjectProcessing();

   /** Define observers for event communication */

  igstkObserverMacro( ImageBounds, igstk::ImageBoundsEvent, 
                                  igstk::EventHelperType::ImageBoundsType );

private:

  /** Variables for maanging reslice plane spatial object */
  ReslicePlaneSpatialObjectPointer  m_ReslicePlaneSpatialObjectToBeSet;
  ReslicePlaneSpatialObjectPointer  m_ReslicePlaneSpatialObject;

  vtkLineSource* m_LineSource;
  vtkTubeFilter *m_Tuber;

  vtkPolyDataMapper* m_LineMapper;

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidToolProjectionObject );
  igstkDeclareInputMacro( NullToolProjectionObject );
  igstkDeclareInputMacro( ValidReslicePlaneSpatialObject );
  igstkDeclareInputMacro( InValidReslicePlaneSpatialObject );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( NullToolProjectionObject );
  igstkDeclareStateMacro( ValidToolProjectionObject );
  igstkDeclareStateMacro( ValidReslicePlaneSpatialObject );


  ToolProjectionSpatialObjectType::ConstPointer m_ToolProjectionObjectToAdd;

};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkToolProjectionRepresentation.txx"
#endif

#endif // __igstkToolProjectionRepresentation_h
