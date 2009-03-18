/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkVideoTemporalCalibrationIO.cxx
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "igstkVideoTemporalCalibrationIO.h"
#include "stdio.h"

#include "igstkTransform.h"

#include "vtkMatrix4x4.h"
#include "vtkTransform.h"


namespace igstk
{

/** Constructor: Initializes all internal variables. */
VideoTemporalCalibrationIO::VideoTemporalCalibrationIO()
{
  m_VideoTemporalCalibration = new VideoTemporalCalibration;
  m_FileName      = "";
}

int VideoTemporalCalibrationIO::RequestWrite()
{
  int probeNumberOfSamples = m_VideoTemporalCalibration->m_ProbeTransforms.size();
  int pointerNumberOfSamples = m_VideoTemporalCalibration->m_PointerTransforms.size();

  if ( probeNumberOfSamples != pointerNumberOfSamples)
    return 0;

  std::ofstream  calibFile;
  calibFile.open( m_FileName.c_str(), ios::trunc );
  if ( calibFile.is_open())
    {
    std::string line;
    
    calibFile << "Samples format: \n";
    calibFile << "cam_time cam_tx cam_ty cam_tz cam_qx cam_qy cam_qz cam_qw probe_time probe_tx probe_ty probe_tz probe_qx probe_qy probe_qz probe_qw \n";
    calibFile << "# number of samples: " << probeNumberOfSamples << "\n";

    for ( int i=0; i<probeNumberOfSamples; i++)
    {
      // cam probe
      igstk::Transform probeTr = m_VideoTemporalCalibration->m_ProbeTransforms[i];      

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

      itk::OStringStream probeOStr;
      probeOStr.precision(30);
      probeOStr << probeTime;
      calibFile << probeOStr.str().c_str() << " ";
      calibFile << probePosition[0] << " " << probePosition[1] << " " << probePosition[2] << " ";
      calibFile << probeQuat.GetX() << " " << probeQuat.GetY() << " " << probeQuat.GetZ() << " " << probeQuat.GetW() << " ";

      probeTransform->Delete();
      probeMatrix->Delete();

      // pointer
      igstk::Transform pointerTr = m_VideoTemporalCalibration->m_PointerTransforms[i];    

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
      
      itk::OStringStream pointerOStr;
      pointerOStr.precision(30);
      pointerOStr << pointerTime;
      calibFile << pointerOStr.str().c_str() << " ";
      calibFile << pointerPosition[0] << " " << pointerPosition[1] << " " << pointerPosition[2] << " ";
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
VideoTemporalCalibrationIO::~VideoTemporalCalibrationIO()
{

}
} // end of namespace
