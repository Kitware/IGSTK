/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPivotCalibrationReader.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "igstkPivotCalibrationReader.h"


namespace igstk
{ 
/** Constructor */
PivotCalibrationReader
::PivotCalibrationReader():m_StateMachine(this)
{
  m_RootMeanSquareError = 0;

} 

/** Destructor */
PivotCalibrationReader
::~PivotCalibrationReader()  
{
}

/** Read the XML file */
bool PivotCalibrationReader
::RetrieveParametersFromFile( TransformType & transform )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationReader::\
                      RetrieveParametersFromFile called...\n");
  
  TransformType::VectorType translation;
  TransformType::VersorType rotation;


  float value;
  if( !this->GetParameter( "translation_x", value ) )
    {
    return false;
    }
  translation[0]=value;

  if( !this->GetParameter( "translation_y", value ) )
    {
    return false;
    }
  translation[1]=value;

  if( !this->GetParameter( "translation_z", value ) )
    {
    return false;
    }
  translation[2]=value;

  if( !this->GetParameter( "quaternion_x", value ))
    {
    return false;
    }
  float x=value;

  if( !this->GetParameter( "quaternion_y", value ))
    {
    return false;
    }  
  float y=value;
  
  if( !this->GetParameter( "quaternion_z", value ))
    {
    return false;
    }
  float z=value;

  if( !this->GetParameter( "quaternion_w", value ))
    {
    return false;
    }
  float w=value;

  rotation.Set(x,y,z,w);

  transform.SetTranslationAndRotation(translation,rotation,0,10000);

  // Set the error
  if( !this->GetError( "rms", value ) )
    {
    return false;
    }

  m_RootMeanSquareError = value;

  return true;
}


void PivotCalibrationReader
::ReportObjectReadingSuccessProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibrationReader::\
                        ReportObjectReadingSuccessProcessing: called...\n");

  Friends::PivotCalibrationReaderToPivotCalibration
   ::ConnectToolCalibration( this, this->GetCalibration() );

  this->Superclass::ReportObjectReadingSuccessProcessing();
}


/** Return the RootMeanSquareError */
float 
PivotCalibrationReader
::GetRootMeanSquareError() const
{
  return m_RootMeanSquareError;
}

/** Print Self function */
void
PivotCalibrationReader
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Root mean square error " 
     << m_RootMeanSquareError << std::endl;
}

} // end namespace igstk
