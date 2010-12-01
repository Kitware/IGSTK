/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVascularNetworkObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkVascularNetworkObjectRepresentation_h
#define __igstkVascularNetworkObjectRepresentation_h

#include "igstkObjectRepresentation.h"
#include "igstkVascularNetworkObject.h"

namespace igstk
{

/** \class VascularNetworkObjectRepresentation
 * 
 * \brief This class implements the representation of a VascularNetwork object.
 * 
 *        This class represents a collection of VesselObjects, each one of them
 *        is a tube represented as a VTK actor using a vtkTubeFilter.
 *
 * \ingroup ObjectRepresentation
 */

class VascularNetworkObjectRepresentation 
: public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( VascularNetworkObjectRepresentation, 
                                 ObjectRepresentation )

public:

  typedef VascularNetworkObject                     VascularNetworkObjectType;
  typedef VesselObject                              VesselObjectType;

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetVascularNetworkObject( const VascularNetworkObjectType 
                                                    * VascularNetworkObject );

protected:
  
  /** Constructor */
  VascularNetworkObjectRepresentation();

  /** Destructor */
  ~VascularNetworkObjectRepresentation();

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Create the VTK actors */
  void CreateActors();

private:

  /** Internal igstk group object */
  VascularNetworkObjectType::ConstPointer   m_VascularNetworkObject;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Connect this representation class to the spatial object. Only to be
   *  called by the State Machine. */
  void SetVascularNetworkObjectProcessing(); 

  /** Null operation for a State Machine transition */
  void NoProcessing();

  /** Create the VTK actors for one Vessel. This method will serve as 
   * callback for the VesselObserver. */
  void CreateActorsForOneVesselProcessing();


private:

  /** These two methods must be declared and note be implemented
  *  in order to enforce the protocol of smart pointers. */
  VascularNetworkObjectRepresentation (const Self&); //purposely not implemented
  void operator=(const Self&);                       //purposely not implemented

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidVascularNetworkObject );
  igstkDeclareInputMacro( NullVascularNetworkObject );
  igstkDeclareInputMacro( VesselReceived );
  igstkDeclareInputMacro( VesselNotFound );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( NullVascularNetworkObject );
  igstkDeclareStateMacro( ValidVascularNetworkObject );
  igstkDeclareStateMacro( AttemptingToGetVessel );

  VascularNetworkObjectType::ConstPointer m_VascularNetworkObjectToAdd;

// This is a temporary fix. Instead of using these macros, 
// observers were used locally and CreateActorsForOneVesselProcessing
// is now evoked directly 
/* 
  igstkLoadedObjectEventTransductionMacro(
    VesselObjectModified, VesselReceived );

  igstkEventTransductionMacro(
    VesselObjectNotAvailable, VesselNotFound );
*/

  igstkObserverObjectMacro(Vessel,
    ::igstk::VesselObjectModifiedEvent,
    ::igstk::VesselObject)

  igstk::VesselObject::Pointer m_VesselToBeSet;
};


} // end namespace igstk

#endif // __igstkVascularNetworkObjectRepresentation_h
