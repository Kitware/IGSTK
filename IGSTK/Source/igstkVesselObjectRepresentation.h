/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVesselObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkVesselObjectRepresentation_h
#define __igstkVesselObjectRepresentation_h

#include "igstkMacros.h"
#include "igstkObjectRepresentation.h"
#include "igstkVesselObject.h"
#include "igstkStateMachine.h"

namespace igstk
{

/** \class VesselObjectRepresentation
 * 
 * \brief This class implements the representation a Vessel object.
 * Basically the tube is represented as a VTK actor using a
 * vtkTubeFilter.
 *
 * \image html  igstkVesselObjectRepresentation.png "State Machine Diagram"
 * \image latex igstkVesselObjectRepresentation.eps "State Machine Diagram"
 *
 * \ingroup ObjectRepresentation
 */

class VesselObjectRepresentation 
: public ObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( VesselObjectRepresentation, 
                                 ObjectRepresentation )

public:

  typedef VesselObject                     VesselObjectType;

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetVesselObject( const VesselObjectType * VesselObject );

protected:
  
  /** Constructor */
  VesselObjectRepresentation( void );

  /** Destructor */
  ~VesselObjectRepresentation( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

  /** Create the VTK actors */
  void CreateActors();

private:

  /** Internal itkSpatialObject */
  VesselObjectType::ConstPointer   m_VesselSpatialObject;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetVesselObjectProcessing(); 

  /** Null operation for a State Machine transition */
  void NoProcessing();

private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidVesselObject );
  igstkDeclareInputMacro( NullVesselObject );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( NullVesselObject );
  igstkDeclareStateMacro( ValidVesselObject );

  VesselObjectType::ConstPointer m_VesselObjectToAdd;

};


} // end namespace igstk

#endif // __igstkVesselObjectRepresentation_h
