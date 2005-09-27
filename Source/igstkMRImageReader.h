/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMRImageReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkMRImageReader_h
#define __igstkMRImageReader_h

#include "igstkDICOMImageReader.h"
#include "igstkMRImageSpatialObject.h"

namespace igstk
{

/** \class MRImageReader
 *  \brief This class implements a reader specific for MR modality.
 *
 * \ingroup Readers
 */

class MRImageReader : 
         public DICOMImageReader< MRImageSpatialObject >
{

public:

  /** Typedefs */
  typedef MRImageReader                               Self;
  typedef DICOMImageReader< MRImageSpatialObject >    Superclass;
  typedef itk::SmartPointer< Self >                   Pointer;
  typedef itk::SmartPointer< const Self >             ConstPointer;


  /**  Run-time type information (and related methods). */
  igstkTypeMacro( MRImageReader, DICOMImageReader );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( Self );

protected:

  MRImageReader( void );
  virtual ~MRImageReader( void ) {};

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

};

} // end namespace igstk

#endif // __igstkMRImageReader_h

