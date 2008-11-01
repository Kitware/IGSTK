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
#include "igstkStateMachine.h"
#include "igstkEvents.h"

#include "igstkCoordinateSystemInterfaceMacros.h"

namespace igstk
{


/** \class SpatialObject
 * 
 * \brief Geometrical abstraction of physical objects present in the surgical
 * scene.
 *
 * This class is intended to describe objects in the surgical scenario.
 * Subclasses of this class will be used for representing, for example,
 * surgical instruments such as needles, catheters and guide wires; as well as
 * pre-operative and intra-operative images.
 *
 * This class encapsulates an ITK spatial object with the goal of restricting
 * access to functionalities that are not essential for IGS applications, or to
 * functionalities thay may present risks and unnecessary flexibility.  This is
 * an abstract class, you should use the derived classes that represent
 * specific shapes.
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

public: // this "public: is necessary because the 
        // Macro above introduces a private section.


  /** Typedefs */
  typedef itk::SpatialObject<3>          SpatialObjectType;

  typedef SpatialObjectType::BoundingBoxType BoundingBoxType;

  igstkLoadedObjectEventMacro( BoundingBoxEvent, IGSTKEvent,
                                        BoundingBoxType );

  void RequestGetBounds();
  void RequestGetBounds() const;

protected:

  /** The constructor of this class is declared protected to enforce the use of
   * SmartPointers syntax when instantiating objects of this class. This
   * constructor will be called indirectly by the ::New() method. It will
   * initialize the internal state machine of this class. */
  SpatialObject( void );

  /** The destructor should be overriden in derived classes that allocate
   * memory for member variables. */
  ~SpatialObject( void );

  /** Replacement for RequestSetSpatialObject(). Internal is added to the name
   * to clarify that this is used with the ITK spatial object as argument */
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

  void ReportBoundsProcessing();
  void ReportBoundsNotAvailableProcessing();

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( InternalSpatialObjectNull );
  igstkDeclareInputMacro( InternalSpatialObjectValid );
  igstkDeclareInputMacro( RequestBounds );

  /** States for the State Machine */
  igstkDeclareStateMacro( Initial );
  igstkDeclareStateMacro( Ready );

  /** Action methods to be invoked only by the state machine */
  void SetInternalSpatialObjectProcessing();

  /** Invoked by the state machine after a call to 
   *  RequestSetInternalSpatialObject with a null object. */
  void ReportSpatialObjectNullProcessing();

  /** Define the coordinate system interface 
   */
  igstkCoordinateSystemClassInterfaceMacro();

};

/** Event to be invoked when the state of the SpatialObject changes.
 *  For example, if the radius of a cylinder changes. */
igstkLoadedObjectEventMacro( 
  SpatialObjectModifiedEvent, IGSTKEvent, SpatialObject );

/** Event to be send to observers that request a SpatialObject, when the
 * spatial object is not yet ready at the provider.  For example, when a
 * GroupObject is queried for a child or when a VascularNetwork is queried for
 * a Vessel using an Id. */
igstkEventMacro( SpatialObjectNotAvailableEvent, IGSTKErrorEvent );

} // end namespace igstk

#endif // __igstkSpatialObject_h
