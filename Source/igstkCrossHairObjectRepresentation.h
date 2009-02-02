/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCrossHairObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCrossHairObjectRepresentation_h
#define __igstkCrossHairObjectRepresentation_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkCrossHairSpatialObject.h"
#include "igstkStateMachine.h"

class vtkLineSource;
class vtkProperty;

namespace igstk
{

/** \class CrossHairObjectRepresentation
 * 
 * \brief 
 *  This class provides a visual representation of a CrossHairSpatialObject. 
 *
 * RequestSetReslicePlaneSpatialObject sets the driving plane onto which the 
 * tool´s long axis is projected.
 *
 * \sa CrossHairSpatialObject
 *
 * \ingroup ObjectRepresentation
 */

class CrossHairObjectRepresentation : public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( CrossHairObjectRepresentation, 
                                 ObjectRepresentation )

public:

  /** Typedefs */
  typedef CrossHairSpatialObject                      CrossHairType;
  typedef CrossHairType::Pointer                      CrossHairPointerType;
  typedef CrossHairType::PointType                    PointType;

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetCrossHairObject( const CrossHairType * crossHairObject );

  virtual void SetVisibility ( bool visible);

  /** Set/Get line width */
  void SetLineWidth(double LineWidth);
  igstkGetMacro( LineWidth, double );

protected:

  CrossHairObjectRepresentation( void );
  virtual ~CrossHairObjectRepresentation( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Create the VTK actors */
  void CreateActors();

  /** Verify time stamp. Use the reslicing tool transform to verify 
  * the time stamp */
  virtual bool VerifyTimeStamp() const;

private:

  CrossHairObjectRepresentation(const Self&); //purposely not implemented
  void operator=(const Self&);     //purposely not implemented  

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetCrossHairObjectProcessing();

  /** Method for performing a null operation during a State Machine 
   *  transition */
  void NoProcessing();

  /** Define observers for event communication */
  igstkObserverMacro( CrossHairPosition, PointEvent,
                                    igstk::EventHelperType::PointType );

  CrossHairPositionObserver::Pointer  m_CrossHairPositionObserver;

private:

  double                            m_ImageBounds[6];

  vtkLineSource* m_LineSourceX;
  vtkLineSource* m_LineSourceY;
  vtkLineSource* m_LineSourceZ;

  vtkProperty* m_LineProperty;

  double m_LineWidth;

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidCrossHairObject );
  igstkDeclareInputMacro( NullCrossHairObject );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( NullCrossHairObject );
  igstkDeclareStateMacro( ValidCrossHairObject );
  igstkDeclareStateMacro( ValidReslicePlaneSpatialObject );

  CrossHairPointerType    m_CrossHairSpatialObjectToAdd;
  CrossHairPointerType    m_CrossHairSpatialObject;

  bool                    m_Visibility;

};

} // end namespace igstk

#endif // __igstkCrossHairObjectRepresentation_h
