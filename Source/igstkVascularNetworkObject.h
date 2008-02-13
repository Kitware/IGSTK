/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVascularNetworkObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkVascularNetworkObject_h
#define __igstkVascularNetworkObject_h

#include "igstkGroupObject.h"
#include "igstkVesselObject.h"

namespace igstk
{

/** \class VascularNetworkObject
 * \brief Implements the 3-dimensional vascular network structure.
 *
 * This class implents a collection of VesselObject instances. They are
 * intended to represent a vascular network but it can not ensure that the
 * vessels are actually connected in a network. The vessels are typically
 * extracted by applying a segmentation method on a pre-operative image.
 *
 * \ingroup GroupObject
 */

class VascularNetworkObject 
: public GroupObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( VascularNetworkObject, GroupObject )

public:
    
  /** Typedefs */
  typedef VesselObject                      VesselObjectType;

  /** Return the number of objects in the group */
  unsigned long GetNumberOfVessels() const;
  
  /** Request Adding a Vessel to the list of children. Note that this
   * method may invoke the RequestAddChild() method in the superclass. */
  void RequestAddVessel( const Transform & transform, VesselObjectType * child );
 
  /** Return the output vessel as an event with payload. */
  void RequestGetVessel(unsigned long id);

protected:

  /** Constructor */
  VascularNetworkObject( void );

  /** Destructor */
  ~VascularNetworkObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /* Make the following methods protected to avoid ambiguities in the 
   * API of this class. When adding Vessels, users should call RequestAddVessel()
   * and not attempt to use the superclass method RequestAddChild() */
  unsigned long GetNumberOfChildren() const;
  void RequestAddChild( SpatialObject * child );
  void RequestGetChild( unsigned long childId );

private:

  /** These two methods must be declared and not be implemented
  *  in order to enforce the protocol of smart pointers. */
  VascularNetworkObject(const Self&);          //purposely not implemented
  void operator=(const Self&);                 //purposely not implemented
 
  /** Inputs to the State Machine */
  igstkDeclareInputMacro( GetVessel );
  igstkDeclareInputMacro( VesselReceived );
  igstkDeclareInputMacro( VesselNotFound );

  /** States for the State Machine */
  igstkDeclareStateMacro( Init );
  igstkDeclareStateMacro( AttemtingToGetVessel );

  unsigned long         m_VesselId;

  /** This function sends a found vessel as a payload to an event. */
  void ReportVesselProcessing();

  /** This function search for a vessel the vascular network */
  void SearchForVesselProcessing();

  /** Report when a request has been made at an incorrect time. */
  void ReportInvalidRequestProcessing();

  /** Report no vessel found */
  void ReportVesselNotFoundProcessing();

  igstkLoadedObjectEventTransductionMacro(
    SpatialObjectModified, VesselReceived );

  igstkEventTransductionMacro(
    SpatialObjectNotAvailable, VesselNotFound );

};

//
// Macros declaring events to be used when requesting VascularNetworkObjects.
//
igstkLoadedObjectEventMacro( VascularNetworkObjectModifiedEvent, 
  IGSTKEvent, VascularNetworkObject );

igstkEventMacro( VascularNetworkObjectNotAvailableEvent, 
  InvalidRequestErrorEvent );

} // end namespace igstk

#endif // __igstkTubeGroupObject_h
