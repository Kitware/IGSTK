/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSpatialObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkSpatialObject_h
#define __igstkSpatialObject_h

#include "igstkLogger.h"
#include "itkSpatialObject.h"

#include "igstkMacros.h"
#include "igstkObject.h"
#include "igstkTransform.h"
#include "igstkStateMachine.h"
#include "igstkEvents.h"

#include "igstkCoordinateSystemInterfaceMacros.h"

namespace igstk
{


/** \class SpatialObject
 * 
 * \brief This class encapsulates an ITK spatial object with the goal of
 * restricting access to functionalities that are not essential for IGS
 * applications, or to functionalities thay may present risks and unnecessary
 * flexibility.  This is an abstract class, you should use the derived classes
 * that represent specific shapes.
 *
 *
 *  \image html  igstkSpatialObject.png  "SpatialObject State Machine Diagram"
 *  \image latex igstkSpatialObject.eps  "SpatialObject State Machine Diagram" 
 *
 * \ingroup Object
 */
class SpatialObject : public Object
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( SpatialObject, Object )

public:

  /** Typedefs */
  typedef itk::SpatialObject<3>          SpatialObjectType;

protected:

  SpatialObject( void );
  ~SpatialObject( void );


  /** Replacement for RequestSetSpatialObject(). Internal is added to the
   *  name to clarify that this is used with the ITK spatial object as argument */
  void RequestSetInternalSpatialObject( SpatialObjectType * object );

  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** Returns the ITK spatial object that is contained inside this IGSTK 
   *  spatial object */
  SpatialObjectType * GetInternalSpatialObject() const;

private:
  
  /** Internal itkSpatialObject */
  SpatialObjectType::Pointer   m_SpatialObject;
  SpatialObjectType::Pointer   m_SpatialObjectToBeSet;


  /** Inputs to the State Machine */
  igstkDeclareInputMacro( InternalSpatialObjectNull );
  igstkDeclareInputMacro( InternalSpatialObjectValid );

  /** States for the State Machine */
  igstkDeclareStateMacro( Initial );

  /** Action methods to be invoked only by the state machine */
  void SetInternalSpatialObjectProcessing();

  /** Null operation for a State Machine transition */
  void NoProcessing();

  /** Report when a request has been made at an incorrect time. */
  void ReportInvalidRequestProcessing();

  /** Invoked by the state machine after a call to 
   *  RequestSetInternalSpatialObject with a null object. */
  void InternalSpatialObjectNullProcessing();

  /** Define the coordinate system interface 
   */
  igstkCoordinateSystemClassInterfaceMacro();

};

igstkLoadedObjectEventMacro( SpatialObjectModifiedEvent, IGSTKEvent, SpatialObject );
igstkEventMacro( SpatialObjectNotAvailableEvent, IGSTKEvent );

} // end namespace igstk

#endif // __igstkSpatialObject_h
