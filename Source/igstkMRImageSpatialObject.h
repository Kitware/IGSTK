/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMRImageSpatialObject.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkMRImageSpatialObject_h
#define __igstkMRImageSpatialObject_h

#include "igstkImageSpatialObject.h"

namespace igstk
{

/** \class MRImageSpatialObject
 * \brief  Represents a MR scan placed correctly in physical coordinates.
 *
 *  This class is the data structure used for containing a volumetric MRI scan.
 *  The scan is usually provided by a MRImageReader class, but can also be
 *  provided by other sources. Due to the protection of the ITK layer, any
 *  other potential source of ITK images must be included in the Friends
 *  mechanism.
 *
 *  \sa CTImageSpatialObject
 *  \sa MRImageReader
 *  \sa MRImageSpatialObjectRepresentation

 * \ingroup Object
 */

class MRImageSpatialObject : 
      public ImageSpatialObject< unsigned short, 3 >
{

public:

  /** Typedefs */
  typedef MRImageSpatialObject                    Self;
  typedef ImageSpatialObject< unsigned short, 3>  Superclass;
  typedef itk::SmartPointer< Self >               Pointer;
  typedef itk::SmartPointer< const Self >         ConstPointer;


  /**  Run-time type information (and related methods). */
  igstkTypeMacro( MRImageSpatialObject, ImageSpatialObject );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( Self );

protected:

  MRImageSpatialObject( void );
  virtual ~MRImageSpatialObject( void );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:
 
  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  MRImageSpatialObject(const Self&);     //purposely not implemented
  void operator=(const Self&);           //purposely not implemented


};

} // end namespace igstk

#endif // __igstkMRImageSpatialObject_h

