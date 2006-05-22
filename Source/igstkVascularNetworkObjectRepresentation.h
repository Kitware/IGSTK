/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkVascularNetworkObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

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
 *        Basically the tube is represented as a VTK actor using a
 *        vtkTubeFilter.
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
  VascularNetworkObjectRepresentation( void );

  /** Destructor */
  ~VascularNetworkObjectRepresentation( void );

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

private:

  /** These two methods must be declared and note be implemented
  *  in order to enforce the protocol of smart pointers. */
  VascularNetworkObjectRepresentation (const Self&); //purposely not implemented
  void operator=(const Self&);                       //purposely not implemented

  /** Inputs to the State Machine */
  igstkDeclareInputMacro( ValidVascularNetworkObject );
  igstkDeclareInputMacro( NullVascularNetworkObject );
  
  /** States for the State Machine */
  igstkDeclareStateMacro( NullVascularNetworkObject );
  igstkDeclareStateMacro( ValidVascularNetworkObject );

  VascularNetworkObjectType::ConstPointer m_VascularNetworkObjectToAdd;

};


} // end namespace igstk

#endif // __igstkVascularNetworkObjectRepresentation_h
