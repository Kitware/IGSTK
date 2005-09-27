/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkCTImageReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkCTImageReader_h
#define __igstkCTImageReader_h

#include "igstkDICOMImageReader.h"
#include "igstkCTImageSpatialObject.h"

namespace igstk
{

/** \class CTImageReader
 *  \brief This class implements a reader specific for CT modality.
 *
 * \ingroup Readers
 */

class CTImageReader : 
         public DICOMImageReader< CTImageSpatialObject >
{

public:

  /** Typedefs */
  typedef CTImageReader                               Self;
  typedef DICOMImageReader< CTImageSpatialObject >    Superclass;
  typedef itk::SmartPointer< Self >                   Pointer;
  typedef itk::SmartPointer< const Self >             ConstPointer;


  /**  Run-time type information (and related methods). */
  igstkTypeMacro( CTImageReader, DICOMImageReader );

  /** Method for creation of a reference counted object. */
  igstkNewMacro( Self );

protected:

  CTImageReader( void );
  virtual ~CTImageReader( void ) {};

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:

};

} // end namespace igstk

#endif // __igstkCTImageReader_h

