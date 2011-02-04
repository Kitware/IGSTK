/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPETImageSpatialObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkPETImageSpatialObject_h
#define __igstkPETImageSpatialObject_h

#include "igstkImageSpatialObject.h"

namespace igstk
{

/** \class PETImageSpatialObject
 * \brief  Represents a PET scan placed correctly in physical coordinates.
 *
 *  This class is the data structure used for containing a volumetric PET scan.
 *  The scan is usually provided by a PETImageReader class, but can also be
 *  provided by other sources. Due to the protection of the ITK layer, any
 *  other potential source of ITK images must be included in the Friends
 *  mechanism.
 *
 *  \sa MRImageSpatialObject
 *  \sa CTImageSpatialObject
 *  \sa PETImageReader
 *  \sa PETImageSpatialObjectRepresentation
 *
 *  \ingroup Object
 */

class PETImageSpatialObject : 
      public ImageSpatialObject< float, 3 >
{

public:

  /** Type of the superclass. 
   *  This must be declared first because the StandardClassTraitsMacro
   *  will otherwise get confused with the commas of the template */
  typedef ImageSpatialObject< float, 3>     SuperclassType;
  
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( PETImageSpatialObject, SuperclassType )


protected:

  PETImageSpatialObject();
  virtual ~PETImageSpatialObject();

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:
 
  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  PETImageSpatialObject(const Self&);    //purposely not implemented
  void operator=(const Self&);           //purposely not implemented


};

} // end namespace igstk

#endif // __igstkPETImageSpatialObject_h
