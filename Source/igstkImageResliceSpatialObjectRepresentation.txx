/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkImageResliceSpatialObjectRepresentation.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkImageResliceSpatialObjectRepresentation_txx
#define __igstkImageResliceSpatialObjectRepresentation_txx


#include "igstkImageResliceSpatialObjectRepresentation.h"

#include "igstkEvents.h"

namespace igstk
{

/** Constructor */
template < class TImageSpatialObject >
ImageResliceSpatialObjectRepresentation < TImageSpatialObject >
::ImageResliceSpatialObjectRepresentation():m_StateMachine(this)
{
  igstkAddInputMacro( ValidImageSpatialObject );
  igstkAddInputMacro( InValidImageSpatialObject );
  igstkAddInputMacro( ValidReslicePlaneSpatialObject);
  igstkAddInputMacro( InValidReslicePlaneSpatialObject  );

  igstkAddStateMacro( Initial  );
  igstkAddStateMacro( ImageSpatialObjectSet );
  igstkAddStateMacro( ReslicePlaneSpatialObjectSet );

  //From Initial state
  igstkAddTransitionMacro( Initial, ValidImageSpatialObject, 
                           ImageSpatialObjectSet,  SetImageSpatialObject );
  igstkAddTransitionMacro( Initial, InValidImageSpatialObject, 
                           Initial,  ReportInvalidImageSpatialObject );

  //From ImageSpatialObjectSet
  igstkAddTransitionMacro( ImageSpatialObjectSet, ValidReslicePlaneSpatialObject, 
                           ReslicePlaneSpatialObjectSet,
SetReslicePlaneSpatialObject);
  igstkAddTransitionMacro( ImageSpatialObjectSet,
InValidReslicePlaneSpatialObject, 
                           ImageSpatialObjectSet,
ReportInvalidReslicePlaneSpatialObject );
  
 
  igstkSetInitialStateMacro( Initial );

  m_StateMachine.SetReadyToRun();
}

/** Destructor */

template < class TImageSpatialObject >
ImageResliceSpatialObjectRepresentation < TImageSpatialObject >
::~ImageResliceSpatialObjectRepresentation()
{

}

template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetImageSpatialObject( const ImageSpatialObjectType * imageSpatialObject )
{  
  igstkLogMacro( DEBUG,"igstk::ImageResliceSpatialObjectRepresentation\
                       ::RequestSetImageSpatialObject called...\n");

  m_ImageSpatialObjectToBeSet = const_cast< ImageSpatialObjectType *>(
imageSpatialObject );

  if( !m_ImageSpatialObjectToBeSet )
    {
    m_StateMachine.PushInput( m_InValidImageSpatialObjectInput );
    }
  else
    {
    m_StateMachine.PushInput( m_ValidImageSpatialObjectInput );
    }

  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::SetImageSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageResliceSpatialObjectRepresentation\
                       ::SetImageSpatialObjectProcessing called...\n");

  m_ImageSpatialObject = m_ImageSpatialObjectToBeSet;
}

template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::ReportInvalidImageSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageResliceSpatialObjectRepresentation\
                       ::ReportInvalidImageSpatialObjectProcessing called...\n");
}

template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::RequestSetReslicePlaneSpatialObject( const ReslicePlaneSpatialObjectType *
reslicePlaneSpatialObject )
{  
  igstkLogMacro( DEBUG,"igstk::ImageResliceSpatialObjectRepresentation\
                       ::RequestSetReslicePlaneSpatialObject called...\n");

  m_ReslicePlaneSpatialObjectToBeSet = const_cast< ReslicePlaneSpatialObjectType
*>(reslicePlaneSpatialObject);

  if( !m_ReslicePlaneSpatialObjectToBeSet )
    {
    m_StateMachine.PushInput( m_InValidReslicePlaneSpatialObjectInput );
    }
  else
    {
    m_StateMachine.PushInput( m_ValidReslicePlaneSpatialObjectInput );
    }

  m_StateMachine.ProcessInputs();
}

template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::SetReslicePlaneSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageResliceSpatialObjectRepresentation\
                       ::SetReslicePlaneSpatialObjectProcessing called...\n");

  m_ReslicePlaneSpatialObject = m_ReslicePlaneSpatialObjectToBeSet;
}

template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::ReportInvalidReslicePlaneSpatialObjectProcessing( )
{  
  igstkLogMacro( DEBUG,"igstk::ImageResliceSpatialObjectRepresentation\
                       ::ReportInvalidReslicePlaneSpatialObjectProcessing called...\n");
}


/** Report invalid request */
template < class TImageSpatialObject >
void 
ImageResliceSpatialObjectRepresentation< TImageSpatialObject >
::ReportInvalidRequestProcessing( void )
{
  igstkLogMacro( DEBUG, 
    "igstk::ImageResliceSpatialObjectRepresentation::ReportInvalidRequestProcessing called...\n");

  this->InvokeEvent( InvalidRequestErrorEvent() );
}



/** Print Self function */
template < class TImageSpatialObject >
void
ImageResliceSpatialObjectRepresentation < TImageSpatialObject >
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
}

} // end namespace igstk

#endif
