/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkGenericImageReader.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkGenericImageReader_h
#define __igstkGenericImageReader_h

#include "igstkObject.h"
#include "igstkMacros.h"
#include "igstkStateMachine.h"

// forward declaration.
class vtkKWImage;
class vtkKWImageIO;

namespace igstk {

/** \class GenericImageReader
 * 
 * \brief Class for reading image of all modalities supported by ITK.
 *
 * Implementation based on vtkKWImageIO class.
 * http://insight-journal.org/dspace/handle/1926/495
 *
 * \ingroup Object
 */

class GenericImageReader : public Object
{

public:
  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro(GenericImageReader, Object)
    
  /** Type for representing the string of the directory 
   *  from which the DICOM files will be read. */
  typedef std::string    SourceNameType;

  /** Method to pass the directory name containing the DICOM image data */
  void RequestSetSource( const SourceNameType & source );

  /** This method request image read */
  void RequestReadImage();

  /** Request to get the output image as an event */
  void RequestGetImage();

  /** Event type */
  //igstkLoadedTemplatedObjectEventMacro( ImageModifiedEvent, IGSTKEvent, 
  //  TImageSpatialObject);

  /** Declarations needed for the Logger */
  igstkLoggerMacro();

protected:

  /** Constructor. */
  GenericImageReader();

  /** Destructor */
  ~GenericImageReader();

  void SetImageSourceProcessing();
  void ReportInvalidRequestProcessing();
  void NoActionProcessing();
  void ReportErrorProcessing();
  void AttemptingToReadImageProcessing();
  void ReportImageProcessing();
  
  

private:

  /** These two methods must be declared and note be implemented
  *  in order to enforce the protocol of smart pointers. */
  GenericImageReader(const Self&);    //purposely not implemented
  void operator=(const Self&);        //purposely not implemented

  vtkKWImageIO *  m_ImageIO;


  /** List of States */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( ImageSourceSet );
  igstkDeclareStateMacro( AttemptingToReadImage );
  igstkDeclareStateMacro( ImageRead );

  /** List of State Inputs */
  igstkDeclareInputMacro( ImageSourceValid ); 
  igstkDeclareInputMacro( ImageSourceInvalid );  
  igstkDeclareInputMacro( ReadImage );
  igstkDeclareInputMacro( ImageReadingSuccess );
  igstkDeclareInputMacro( ImageReadingError );
  igstkDeclareInputMacro( ResetReader );
  igstkDeclareInputMacro( GetImage );

};

} // end namespace igstk

#endif
