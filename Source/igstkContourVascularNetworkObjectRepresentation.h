/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkContourVascularNetworkObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkContourVascularNetworkObjectRepresentation_h
#define __igstkContourVascularNetworkObjectRepresentation_h

#include "igstkMacros.h"
#include "igstkContourObjectRepresentation.h"
#include "igstkVascularNetworkObject.h"
#include "igstkStateMachine.h"
#include "igstkVesselObject.h"

#include <vtkPlane.h>

namespace igstk
{

/** \class ContourVascularNetworkObjectRepresentation
 * 
 * \brief This class represents a VascularNetwork object as contour
 * 
 *
 * \ingroup ObjectRepresentation
 */

class ContourVascularNetworkObjectRepresentation 
: public ContourObjectRepresentation
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( ContourVascularNetworkObjectRepresentation, ContourObjectRepresentation )

public:
    
  /** Typedefs */
  typedef VascularNetworkObject                     VascularNetworkObjectType;
  typedef VesselObject                              VesselObjectType;

  /** Return a copy of the current object representation */
  Pointer Copy() const;

  /** Connect this representation class to the spatial object */
  void RequestSetVascularNetworkObject( const VascularNetworkObjectType * VascularNetworkObject );

protected:

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 
  
  /** Constructor */
  ContourVascularNetworkObjectRepresentation( void );

  /** Destructor */
  ~ContourVascularNetworkObjectRepresentation( void );

  /** Create the VTK actors */
  void CreateActors();

private:

  /** Internal itkSpatialObject */
  VascularNetworkObjectType::ConstPointer   m_VascularNetworkObject;

  /** update the visual representation with changes in the geometry */
  virtual void UpdateRepresentationProcessing();

  /** Connect this representation class to the spatial object. Only to be
   * called by the State Machine. */
  void SetVascularNetworkObjectProcessing(); 

  /** Null operation for a State Machine transition */
  void NoProcessing();

private:

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidVascularNetworkObject );
  igstkDeclareInputMacro( NullVascularNetworkObject );
//  igstkDeclareInputMacro( SetSlicePosition );
//  igstkDeclareInputMacro( ValidSlicePosition );
//  igstkDeclareInputMacro( InvalidSlicePosition );
//  igstkDeclareInputMacro( ValidOrientation );

  /** States for the State Machine */
//  igstkDeclareStateMacro( ValidVascularNetworkOrientation );
//  igstkDeclareStateMacro( ValidSlicePosition );
//  igstkDeclareStateMacro( AttemptingToSetSlicePosition );
  igstkDeclareStateMacro( NullVascularNetworkObject );
  igstkDeclareStateMacro( ValidVascularNetworkObject );

  /** Variables for managing the Slice number through the StateMachine */
//  float      m_SlicePositionToBeSet;
//  float      m_SlicePosition;

  /** Variables for managing the Orientation of the slices */
//  OrientationType      m_OrientationToBeSet;
 // OrientationType      m_Orientation;

  VascularNetworkObjectType::ConstPointer m_VascularNetworkObjectToAdd;
 // vtkPlane* m_Plane;
};


} // end namespace igstk

#endif // __igstkContourVascularNetworkObjectRepresentation_h

