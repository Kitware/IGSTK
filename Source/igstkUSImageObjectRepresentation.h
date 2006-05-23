/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkUSImageObjectRepresentation.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkUSImageObjectRepresentation_h
#define __igstkUSImageObjectRepresentation_h

#include "igstkImageSpatialObjectRepresentation.h"
#include "igstkUSImageObject.h"

namespace igstk
{

/** \class USImageObjectRepresentation
 *
 *  \brief This class implements a spatial object represenation 
 *         for US modality.
 *
 * \ingroup Spatial object representation
 */

class USImageObjectRepresentation : 
         public ImageSpatialObjectRepresentation< USImageObject >
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( USImageObjectRepresentation, \
                        ImageSpatialObjectRepresentation< USImageObject > )

protected:

  USImageObjectRepresentation( void );
  virtual ~USImageObjectRepresentation( void ) {};

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

};

} // end namespace igstk

#endif // __igstkUSImageObjectRepresentation_h
