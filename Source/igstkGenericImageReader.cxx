/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkGenericImageReader.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkGenericImageReader.h"

#include "vtkKWImage.h"
#include "vtkKWImageIO.h"


namespace igstk {


/* Constructor. It will initialize the point to (0,0,0), and an unknown
 * coordinate system. */
GenericImageReader::GenericImageReader() : m_StateMachine(this)
{
  m_Logger = NULL;

  //Set the state descriptors
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( ImageSourceSet ); 
  igstkAddStateMacro( AttemptingToReadImage ); 
  igstkAddStateMacro( ImageRead ); 

  /** List of  Inputs */
  igstkAddInputMacro( ImageSourceValid );
  igstkAddInputMacro( ImageSourceInvalid );
  igstkAddInputMacro( ReadImage );  
  igstkAddInputMacro( ImageReadingSuccess );
  igstkAddInputMacro( ImageReadingError );  
  igstkAddInputMacro( ResetReader );
  igstkAddInputMacro( GetImage );


  //Transitions for valid directory name
  igstkAddTransitionMacro( Idle,
    ImageSourceValid,
    ImageSourceSet,
    SetImageSource );

  igstkAddTransitionMacro( Idle,
    ImageSourceInvalid,
    Idle,
    ReportError );

  //Transitions for image series file name reading 
  igstkAddTransitionMacro( ImageSourceSet,
    ReadImage,
    AttemptingToReadImage,
    AttemptingToReadImage );

  // Transitions for image reading
  igstkAddTransitionMacro( AttemptingToReadImage,
    ImageReadingSuccess,
    ImageRead,
    NoAction );
  
  igstkAddTransitionMacro( AttemptingToReadImage,
    ImageReadingError,
    ImageSourceSet,
    ReportError );

  igstkAddTransitionMacro( ImageRead,
    GetImage,
    ImageRead,
    ReportImage );

   // Select the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();


  m_ImageIO = vtkKWImageIO::New();
}

/* Destructor. */
GenericImageReader::~GenericImageReader()
{
  m_ImageIO->Delete();
}

/** Method to pass the directory name containing the DICOM image data */
void GenericImageReader::RequestSetSource( const SourceNameType & source )
{

}

/** This method request image read */
void GenericImageReader::RequestReadImage()
{

}

/** Request to get the output image as an event */
void GenericImageReader::RequestGetImage()
{

}


void GenericImageReader::SetImageSourceProcessing()
{

}


void GenericImageReader::ReportInvalidRequestProcessing()
{

}
void GenericImageReader::NoActionProcessing()
{

}
void GenericImageReader::ReportErrorProcessing()
{

}
void GenericImageReader::AttemptingToReadImageProcessing()
{

}
void GenericImageReader::ReportImageProcessing()
{

}

} // end namespace igstk
