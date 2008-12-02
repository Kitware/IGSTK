/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkUltrasoundCalibrationIO.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkUltrasoundCalibrationIO.h"
#include "stdio.h"

#include "igstkTransform.h"

#include "vtkMatrix4x4.h"
#include "vtkTransform.h"


namespace igstk
{

/** Constructor: Initializes all internal variables. */
UltrasoundCalibrationIO::UltrasoundCalibrationIO()
{
  m_UltrasoundCalibration = new UltrasoundCalibration;
  m_FileName      = "";
}

int UltrasoundCalibrationIO::RequestWrite()
{
  int probeNumberOfSamples = m_UltrasoundCalibration->m_ProbeTransforms.size();
  int pointerNumberOfSamples = m_UltrasoundCalibration->m_PointerTransforms.size();

  if ( probeNumberOfSamples != pointerNumberOfSamples)
    return 0;

  std::ofstream  calibFile;
  calibFile.open( m_FileName.c_str(), ios::trunc );
  if ( calibFile.is_open())
    {
    std::string line;
    
    calibFile << "# number of samples: " << probeNumberOfSamples << "\n";
    for ( int i=0; i<probeNumberOfSamples; i++)
    {
      // point US calibration format
      //x_i y_i US_tx US_ty US_tz US_qx US_qy US_qz US_qw probe_tx probe_ty probe_tz probe_qx probe_qy probe_qz probe_qw

// US probe
      igstk::Transform probeTr = m_UltrasoundCalibration->m_ProbeTransforms[i];      

      igstk::TimeStamp::TimePeriodType probeTime  = probeTr.GetStartTime();

      igstk::Transform::VersorType probeQuat = probeTr.GetRotation();

      vtkMatrix4x4* probeMatrix = vtkMatrix4x4::New();
      probeTr.ExportTransform( *probeMatrix );
      
      vtkTransform* probeTransform = vtkTransform::New();
      probeTransform->SetMatrix(probeMatrix);
      
      double  probeAngles[3];
      probeTransform->GetOrientation( probeAngles ); 

      double  probePosition[3];
      probeTransform->GetPosition( probePosition ); 
            
      calibFile << probeTime << " ";
      calibFile << probePosition[0] << " " << probePosition[1] << " " << probePosition[2] << " ";

     // calibFile << probeAngles[0] << " " << probeAngles[1] << " " << probeAngles[2] << " ";
      calibFile << probeQuat.GetX() << " " << probeQuat.GetY() << " " << probeQuat.GetZ() << " " << probeQuat.GetW() << " ";

      probeTransform->Delete();
      probeMatrix->Delete();

// pointer
      igstk::Transform pointerTr = m_UltrasoundCalibration->m_PointerTransforms[i];    

      igstk::TimeStamp::TimePeriodType pointerTime  = pointerTr.GetStartTime();

      igstk::Transform::VersorType pointerQuat = pointerTr.GetRotation();

      vtkMatrix4x4* pointerMatrix = vtkMatrix4x4::New();
      pointerTr.ExportTransform( *pointerMatrix );
      
      vtkTransform* pointerTransform = vtkTransform::New();
      pointerTransform->SetMatrix(pointerMatrix);
      
      double  pointerAngles[3];
      pointerTransform->GetOrientation( pointerAngles ); 

      double  pointerPosition[3];
      pointerTransform->GetPosition( pointerPosition ); 
      
      calibFile << pointerTime << " ";
      calibFile << pointerPosition[0] << " " << pointerPosition[1] << " " << pointerPosition[2] << " ";

      //calibFile << pointerAngles[0] << " " << pointerAngles[1] << " " << pointerAngles[2] << "\n";
      calibFile << pointerQuat.GetX() << " " << pointerQuat.GetY() << " " << pointerQuat.GetZ() << " " << pointerQuat.GetW() << "\n";

      pointerTransform->Delete();
      pointerMatrix->Delete();
    }
    calibFile.close();
    return 1;
    }
  else
    {
    return 0;
    }  
}
/** Destructor */
UltrasoundCalibrationIO::~UltrasoundCalibrationIO()
{

}
} // end of namespace
