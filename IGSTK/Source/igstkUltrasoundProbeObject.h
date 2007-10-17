/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkUltrasoundProbeObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkUltrasoundProbeObject_h
#define __igstkUltrasoundProbeObject_h

#include "igstkMacros.h"
#include "igstkSpatialObject.h"
#include "itkGroupSpatialObject.h"

namespace igstk
{

/** \class UltrasoundProbeObject
 * 
 * \brief This class represents a UltrasoundProbe object. 
 * \warning For the moment this class does not offer any parameter tuning
 * such as height and width.
 * The actual representation of the class is done via the itkGroupSpatialObject 
 *
 * Default representation axis is Z.
 *
 * \ingroup Object
 */

class UltrasoundProbeObject 
: public SpatialObject
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( UltrasoundProbeObject, SpatialObject )

protected:

  UltrasoundProbeObject( void );
  ~UltrasoundProbeObject( void );

  /** Print object information */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:
 
  /** Typedefs */
  typedef itk::GroupSpatialObject<3>     UltrasoundProbeSpatialObjectType;

  UltrasoundProbeObject(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  UltrasoundProbeSpatialObjectType::Pointer m_UltrasoundProbeSpatialObject;

};

} // end namespace igstk

#endif // __igstkUltrasoundProbeObject_h
