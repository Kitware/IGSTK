/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkToolProjectionSpatialObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkToolProjectionSpatialObject_h
#define __igstkToolProjectionSpatialObject_h

#include "igstkMacros.h"
#include "igstkSpatialObject.h"
#include "itkGroupSpatialObject.h"

namespace igstk
{

/** \class ToolProjectionSpatialObject
 * 
 * \brief This class describes a surgical tool object projected onto
 *        a reslicing plane. 
 *
 * A surgical tool is geometrically described by polyline ( vtkLineSource ) 
 *
 * \ingroup Object
 * \sa ToolProjectionRepresentation
 */

class ToolProjectionSpatialObject 
: public SpatialObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( ToolProjectionSpatialObject, SpatialObject )

public:
  
  /** Set the size of each axis */
  void SetSize(double size);
  
  /** Get size of the tool */
  double GetSize() const;
 
protected:

  ToolProjectionSpatialObject( void );
  ~ToolProjectionSpatialObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

  /** Typedefs */
  typedef itk::GroupSpatialObject<3>     ToolProjectionSpatialObjectType;

  ToolProjectionSpatialObject(const Self&);         //purposely not implemented
  void operator=(const Self&);     //purposely not implemented

  ToolProjectionSpatialObjectType::Pointer     m_ToolProjectionSpatialObject;

  double                             m_Size;

};

} // end namespace igstk

#endif // __igstkToolProjectionSpatialObject_h
