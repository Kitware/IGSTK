/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkUSImageReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkUSImageReader_h
#define __igstkUSImageReader_h

#include "igstkDICOMImageReader.h"
#include "igstkUSImageObject.h"

namespace igstk
{

/** \class USImageReader
 *
 *  \brief This class implements a reader specific for US modality.

 * This class derives from the DICOMImageReader. It expects to load a volume
 * from a set of DICOM files. 
 *
 * \warning: To be fixed: It will verify that the image modality is US.
 *
 * \ingroup Readers
 */

class USImageReader : public DICOMImageReader< USImageObject >
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( USImageReader, 
                                 DICOMImageReader< USImageObject > )

protected:

  USImageReader( void );
  virtual ~USImageReader( void );

  /** check if the dicom data is from a "US" modality */
  bool CheckModalityType( DICOMInformationType modality );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const; 

private:
  
  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  USImageReader(const Self&);         //purposely not implemented
  void operator=(const Self&);        //purposely not implemented


};

} // end namespace igstk

#endif // __igstkUSImageReader_h
