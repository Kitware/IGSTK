/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCrossHairRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCrossHairRepresentation_h
#define __igstkCrossHairRepresentation_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkCrossHairObject.h"
#include "igstkImageReslicePlaneSpatialObject.h"
#include "igstkStateMachine.h"

class vtkLineSource;
class vtkTubeFilter;
class vtkPolyDataMapper;

namespace igstk
{

/** \class CrossHairRepresentation
 * 
 * \brief This class provides a visual representation of a CrossHairObject. 
 *
 * RequestSetReslicePlaneSpatialObject sets the driving plane onto which the tool´s long axis
 * is projected.
 *
 * \sa CrossHairObject
 *
 * \ingroup ObjectRepresentation
 */
template < class TImageSpatialObject >
class CrossHairRepresentation : public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardTemplatedClassTraitsMacro( CrossHairRepresentation, 
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

  typedef CrossHairObject                 CrossHairSpatialObjectType;

  void RequestSetReslicePlaneSpatialObject( const ReslicePlaneSpatialObjectType *
                                                             planeSpatialObject);

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetCrossHairObject( const CrossHairSpatialObjectType * CrossHairObject );

  virtual void SetVisibility ( bool visible);

protected:

  CrossHairRepresentation( void );
  virtual ~CrossHairRepresentation( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Create the VTK actors */
  void CreateActors();

  /** Verify time stamp. Use the reslicing tool transform to verify 
  * the time stamp */
  virtual bool VerifyTimeStamp() const;

private:

  CrossHairRepresentation(const Self&); //purposely not implemented
  void operator=(const Self&);     //purposely not implemented

  
  /** Internal itkSpatialObject */
  CrossHairSpatialObjectType::ConstPointer   m_CrossHairSpatialObject;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetCrossHairObjectProcessing();

  /** Method for performing a null operation during a State Machine 
   *  transition */
  void NoProcessing();

  /** Set the reslice plane spatial object */
  void SetReslicePlaneSpatialObjectProcessing();

   /** Define observers for event communication */

  igstkObserverMacro( ImageBounds, igstk::ImageBoundsEvent, 
                                  igstk::EventHelperType::ImageBoundsType );

  igstkObserverMacro( ToolPosition, igstk::PointEvent, 
                                  igstk::EventHelperType::PointType );

private:

  /** Variables for maanging reslice plane spatial object */
  ReslicePlaneSpatialObjectPointer  m_ReslicePlaneSpatialObjectToBeSet;
  ReslicePlaneSpatialObjectPointer  m_ReslicePlaneSpatialObject;

  double                            m_ImageBounds[6];

  vtkLineSource* m_AxialLineSource;
  vtkLineSource* m_SagittalLineSource;
  vtkLineSource* m_CoronalLineSource;

  vtkTubeFilter *m_AxialTuber;
  vtkTubeFilter *m_SagittalTuber;
  vtkTubeFilter *m_CoronalTuber;

  vtkPolyDataMapper* m_AxialLineMapper;
  vtkPolyDataMapper* m_SagittalLineMapper;
  vtkPolyDataMapper* m_CoronalLineMapper;

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidCrossHairObject );
  igstkDeclareInputMacro( NullCrossHairObject );
  igstkDeclareInputMacro( ValidReslicePlaneSpatialObject );
  igstkDeclareInputMacro( InValidReslicePlaneSpatialObject );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( NullCrossHairObject );
  igstkDeclareStateMacro( ValidCrossHairObject );
  igstkDeclareStateMacro( ValidReslicePlaneSpatialObject );


  CrossHairSpatialObjectType::ConstPointer m_CrossHairObjectToAdd;

};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkCrossHairRepresentation.txx"
#endif

#endif // __igstkCrossHairRepresentation_h
