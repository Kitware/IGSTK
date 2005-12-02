/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPivotCalibration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more DEBUGrmation.

=========================================================================*/

#ifndef _igstkPivtoCalibration_cxx
#define _igstkPivtoCalibration_cxx


#include "igstkPivotCalibration.h"

namespace igstk
{

/** Constructor */
PivotCalibration::PivotCalibration() :
  m_StateMachine( this ), m_Logger( NULL)
{
  // Set the state descriptors
  m_StateMachine.AddState( m_IdleState, 
                            "IdleState");
  m_StateMachine.AddState( m_RotationTranslationAddState, 
                            "RotationTranslationAddState");
  m_StateMachine.AddState( m_CalibrationCalculatedState, 
                            "CalibrationCalculatedState");
  m_StateMachine.AddState( m_CalibrationZCalculatedState, 
                            "CalibrationZCalculatedState");

  // Set the input descriptors 
  m_StateMachine.AddInput( m_ResetCalibrationInput, 
                            "ResetCalibrationInput");
  m_StateMachine.AddInput( m_RotationTranslationInput, 
                            "RotationTranslationInput");
  m_StateMachine.AddInput( m_CalculateCalibrationInput, 
                            "CalculateCalibrationInput");
  m_StateMachine.AddInput( m_CalculateCalibrationZInput, 
                            "CalculateCalibrationZInput");
  m_StateMachine.AddInput( m_SimulatePivotPositionInput, 
                            "SimulatePivotPositionInput");
  m_StateMachine.AddInput( m_GetInputRotationTranslationInput, 
                            "GetInputRotationTranslationInput");
  m_StateMachine.AddInput( m_PrincipalAxisInput, 
                            "PrincipalAxisInput");
  m_StateMachine.AddInput( m_PlaneNormalInput, 
                            "PlaneNormalInput");
  m_StateMachine.AddInput( m_TranslationInput, 
                            "TranslationInput");
  m_StateMachine.AddInput( m_QuaternionInput, 
                            "QuaternionInput");
  m_StateMachine.AddInput( m_RotationMatrixInput, 
                            "RotationMatrixInput");

  // Add transition  for idle state
  m_StateMachine.AddTransition( m_IdleState,
                                m_ResetCalibrationInput,
                                m_IdleState,
                                &PivotCalibration::Reset );

  m_StateMachine.AddTransition( m_IdleState,
                                m_RotationTranslationInput,
                                m_RotationTranslationAddState,
                                &PivotCalibration::AddRotationTranslation );

  m_StateMachine.AddTransition( m_IdleState,
                                m_CalculateCalibrationInput,
                                m_IdleState,
                                NULL );

  m_StateMachine.AddTransition( m_IdleState,
                                m_CalculateCalibrationZInput,
                                m_IdleState,
                                NULL );

  m_StateMachine.AddTransition( m_IdleState,
                                m_SimulatePivotPositionInput,
                                m_IdleState,
                                NULL );

  m_StateMachine.AddTransition( m_IdleState,
                                m_GetInputRotationTranslationInput,
                                m_IdleState,
                                NULL );

  m_StateMachine.AddTransition( m_IdleState,
                                m_PrincipalAxisInput,
                                m_IdleState,
                                &PivotCalibration::SetToolPrincipalAxis );

  m_StateMachine.AddTransition( m_IdleState,
                                m_PlaneNormalInput,
                                m_IdleState,
                                &PivotCalibration::SetToolPlaneNormal );

  m_StateMachine.AddTransition( m_IdleState,
                                m_TranslationInput,
                                m_IdleState,
                                &PivotCalibration::SetTranslation );

  m_StateMachine.AddTransition( m_IdleState,
                                m_QuaternionInput,
                                m_IdleState,
                                &PivotCalibration::SetQuaternion );

  m_StateMachine.AddTransition( m_IdleState,
                                m_RotationMatrixInput,
                                m_IdleState,
                                &PivotCalibration::SetRotationMatrix );

  // Add transition  for RotationTranslationAdd state
  m_StateMachine.AddTransition( m_RotationTranslationAddState,
                                m_ResetCalibrationInput,
                                m_IdleState,
                                &PivotCalibration::Reset );

  m_StateMachine.AddTransition( m_RotationTranslationAddState,
                                m_RotationTranslationInput,
                                m_RotationTranslationAddState,
                                &PivotCalibration::AddRotationTranslation );

  m_StateMachine.AddTransition( m_RotationTranslationAddState,
                                m_CalculateCalibrationInput,
                                m_CalibrationCalculatedState,
                                &PivotCalibration::CalculateCalibration );

  m_StateMachine.AddTransition( m_RotationTranslationAddState,
                                m_CalculateCalibrationZInput,
                                m_CalibrationZCalculatedState,
                                &PivotCalibration::CalculateCalibrationZ );

  m_StateMachine.AddTransition( m_RotationTranslationAddState,
                                m_SimulatePivotPositionInput,
                                m_RotationTranslationAddState,
                                NULL );

  m_StateMachine.AddTransition( m_RotationTranslationAddState,
                                m_GetInputRotationTranslationInput,
                                m_RotationTranslationAddState,
                                &PivotCalibration::GetInputRotationTranslation );

  m_StateMachine.AddTransition( m_RotationTranslationAddState,
                                m_PrincipalAxisInput,
                                m_RotationTranslationAddState,
                                &PivotCalibration::SetToolPrincipalAxis );

  m_StateMachine.AddTransition( m_RotationTranslationAddState,
                                m_PlaneNormalInput,
                                m_RotationTranslationAddState,
                                &PivotCalibration::SetToolPlaneNormal );

  m_StateMachine.AddTransition( m_RotationTranslationAddState,
                                m_TranslationInput,
                                m_RotationTranslationAddState,
                                &PivotCalibration::SetTranslation );

  m_StateMachine.AddTransition( m_RotationTranslationAddState,
                                m_QuaternionInput,
                                m_RotationTranslationAddState,
                                &PivotCalibration::SetQuaternion );

  m_StateMachine.AddTransition( m_RotationTranslationAddState,
                                m_RotationMatrixInput,
                                m_RotationTranslationAddState,
                                &PivotCalibration::SetRotationMatrix );

  // Add transition  for CalibrationCalculated state
  m_StateMachine.AddTransition( m_CalibrationCalculatedState,
                                m_ResetCalibrationInput,
                                m_IdleState,
                                &PivotCalibration::Reset );

  m_StateMachine.AddTransition( m_CalibrationCalculatedState,
                                m_RotationTranslationInput,
                                m_RotationTranslationAddState,
                                &PivotCalibration::AddRotationTranslation );

  m_StateMachine.AddTransition( m_CalibrationCalculatedState,
                                m_CalculateCalibrationInput,
                                m_CalibrationCalculatedState,
                                NULL );

  m_StateMachine.AddTransition( m_CalibrationCalculatedState,
                                m_CalculateCalibrationZInput,
                                m_CalibrationZCalculatedState,
                                &PivotCalibration::CalculateCalibrationZ );

  m_StateMachine.AddTransition( m_CalibrationCalculatedState,
                                m_SimulatePivotPositionInput,
                                m_CalibrationCalculatedState,
                                &PivotCalibration::SimulatePivotPosition );

  m_StateMachine.AddTransition( m_CalibrationCalculatedState,
                                m_GetInputRotationTranslationInput,
                                m_CalibrationCalculatedState,
                                &PivotCalibration::GetInputRotationTranslation );

  m_StateMachine.AddTransition( m_CalibrationCalculatedState,
                                m_PrincipalAxisInput,
                                m_CalibrationCalculatedState,
                                &PivotCalibration::SetToolPrincipalAxis );

  m_StateMachine.AddTransition( m_CalibrationCalculatedState,
                                m_PlaneNormalInput,
                                m_CalibrationCalculatedState,
                                &PivotCalibration::SetToolPlaneNormal );

  m_StateMachine.AddTransition( m_CalibrationCalculatedState,
                                m_TranslationInput,
                                m_CalibrationCalculatedState,
                                &PivotCalibration::SetTranslation );

  m_StateMachine.AddTransition( m_CalibrationCalculatedState,
                                m_QuaternionInput,
                                m_CalibrationCalculatedState,
                                &PivotCalibration::SetQuaternion );

  m_StateMachine.AddTransition( m_CalibrationCalculatedState,
                                m_RotationMatrixInput,
                                m_CalibrationCalculatedState,
                                &PivotCalibration::SetRotationMatrix );

  // Add transition  for CalibrationZCalculated state
  m_StateMachine.AddTransition( m_CalibrationZCalculatedState,
                                m_ResetCalibrationInput,
                                m_IdleState,
                                &PivotCalibration::Reset );

  m_StateMachine.AddTransition( m_CalibrationZCalculatedState,
                                m_RotationTranslationInput,
                                m_RotationTranslationAddState,
                                &PivotCalibration::AddRotationTranslation );

  m_StateMachine.AddTransition( m_CalibrationZCalculatedState,
                                m_CalculateCalibrationInput,
                                m_CalibrationCalculatedState,
                                &PivotCalibration::CalculateCalibration );

  m_StateMachine.AddTransition( m_CalibrationZCalculatedState,
                                m_CalculateCalibrationZInput,
                                m_CalibrationZCalculatedState,
                                NULL );

  m_StateMachine.AddTransition( m_CalibrationZCalculatedState,
                                m_SimulatePivotPositionInput,
                                m_CalibrationZCalculatedState,
                                &PivotCalibration::SimulatePivotPosition );

  m_StateMachine.AddTransition( m_CalibrationZCalculatedState,
                                m_GetInputRotationTranslationInput,
                                m_CalibrationZCalculatedState,
                                &PivotCalibration::GetInputRotationTranslation );

  m_StateMachine.AddTransition( m_CalibrationZCalculatedState,
                                m_PrincipalAxisInput,
                                m_CalibrationZCalculatedState,
                                &PivotCalibration::SetToolPrincipalAxis );

  m_StateMachine.AddTransition( m_CalibrationZCalculatedState,
                                m_PlaneNormalInput,
                                m_CalibrationZCalculatedState,
                                &PivotCalibration::SetToolPlaneNormal );

  m_StateMachine.AddTransition( m_CalibrationZCalculatedState,
                                m_TranslationInput,
                                m_CalibrationZCalculatedState,
                                &PivotCalibration::SetTranslation );

  m_StateMachine.AddTransition( m_CalibrationZCalculatedState,
                                m_QuaternionInput,
                                m_CalibrationZCalculatedState,
                                &PivotCalibration::SetQuaternion );

  m_StateMachine.AddTransition( m_CalibrationZCalculatedState,
                                m_RotationMatrixInput,
                                m_CalibrationZCalculatedState,
                                &PivotCalibration::SetRotationMatrix );

  // Select the initial state of the state machine
  m_StateMachine.SelectInitialState( m_IdleState );

  // Finish the programming and get ready to run
  m_StateMachine.SetReadyToRun();

  // Reset the initial state and variables
  this->Reset();
}

/** Destructor */
PivotCalibration::~PivotCalibration()
{

}

/** Print Self function */
void PivotCalibration::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  // Dump the calibration class information
  os << indent << "PivotCalibration: " << std::endl;

  os << indent << "NumberOfFrame: " << this->GetNumberOfFrame() << std::endl;

  os << indent << "Calibration Transform: " << this->m_CalibrationTransform << std::endl;
  
  os << indent << "Pivot Position: " << this->m_PivotPosition << std::endl;
  
  os << indent << "Calibration RMS: " << this->m_RMS << std::endl;

}

/** Method to return the number of samples */
int PivotCalibration::GetNumberOfFrame() const
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::GetNumberOfFrame called...\n" );

  return this->m_Quaternion[0].size();
}

/** Method to reset the calibration */
void PivotCalibration::Reset()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::Reset called...\n" );
  unsigned int i;
  VersorType quaternion;
  VectorType translation;

  // Clear the input container for quaternion and translation
  for ( i = 0; i < 4; i++)
    {
    this->m_Quaternion[i].clear();
    }

  for ( i = 0; i < 3; i++)
    {
    this->m_Translation[i].clear();
    }

  // Reset the calibration transform
  quaternion.SetIdentity();
  translation.Fill( 0.0);
  this->m_CalibrationTransform.SetTranslationAndRotation( translation, quaternion, 0.1, 1000);

  // Reset the pivot position 
  this->m_PivotPosition.Fill( 0.0);

  // Reset the RMS calibration error
  this->m_RMS = 0.0;

  // Reset the principal axis
  this->m_PrincipalAxis[0] = 0.0;
  this->m_PrincipalAxis[1] = 0.0;
  this->m_PrincipalAxis[2] = 1.0;

  // Reset the plane normal
  this->m_PlaneNormal[0] = 0.0;
  this->m_PlaneNormal[1] = 1.0;
  this->m_PlaneNormal[2] = 0.0;

  // Reset the adjusted plane normal
  this->m_AdjustedPlaneNormal = this->m_PlaneNormal;

  // Reset the validation indicator
  this->m_ValidPivotCalibration = false;

  // Reset the validation indicator
  this->m_ValidInputSample = false;
}

/** Method to add the sample information */
void PivotCalibration::AddRotationTranslation()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::AddRotationTranslation called...\n" );
  
  this->m_NumberOfFrameToBeReceived = this->InternalAddRotationTranslation( m_QuaternionToBeSent, m_TranslationToBeSent);
}

/** Internal method to add the sample information */
int PivotCalibration::InternalAddRotationTranslation( VersorType quaternion, VectorType translation )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::InternalAddRotationTranslation called...\n" );

  unsigned int i;

  // Push the quaternion sample into the input container
  this->m_Quaternion[0].push_back( quaternion.GetW() );
  this->m_Quaternion[1].push_back( quaternion.GetX() );
  this->m_Quaternion[2].push_back( quaternion.GetY() );
  this->m_Quaternion[3].push_back( quaternion.GetZ() );

  // Push the translation sample into the input container
  for ( i = 0; i < 3; i++)
    {
    this->m_Translation[i].push_back( translation[i] );
    }

  // Unvalid the calibration
  this->m_ValidPivotCalibration = false;

  // Return the number of current frame;
  return this->GetNumberOfFrame();
}

/** Internal method to calculate the calibration */
void PivotCalibration::InternalCalculateCalibration( unsigned int axis )
{
  /** Use the Moore-Penrose inverse to calculate the calibration matrix
   *  The algorithm used is from the paper "Freehand Ultrasound Calibration using
   *  an Electromagnetic Needle" by Hui Zhang, Filip Banovac, Kevin Cleary to be 
   *  published in SPIE MI 2006. 
   * 
   *  [ r00 r01 r02 tx][ Offset0 ]   [ x0 ]
   *  [ r10 r11 r12 ty][ Offset1 ]   [ y0 ]
   *  [ r20 r21 r22 tz][ Offset2 ] = [ z0 ]
   *  [  0   0   0   1][    1    ]   [  1 ]
   *
   *  After the transformation, the unknowns of [ Offset0 Offset1 Offset2 x0 y0 z0 ]' 
   *  can be calculated by 
   *  
   *  M * [ Offset0 Offset1 Offset2 x0 y0 z0]' = N
   *  [ Offset0 Offset1 Offset2 x0 y0 z0]' = (M' * M)^-1 * M' * N
   *  RMS = sqrt( |M * [ Offset0 Offset1 Offset2 x0 y0 z0 ]' - N|^2 / num ) */   

  igstkLogMacro( DEBUG, "igstk::PivotCalibration::InternalCalculateCalibration called...\n" );

  unsigned int i, j, k;
  unsigned int r, c, num;

  // Set the number of sample, tow and column number of matrix
  num = this->GetNumberOfFrame();
  r = num * 3;
  c = 3 + axis;

  // Define the Vnl matrix and intermediate variables
  VnlMatrixType matrix(r, c), m(c, c), minv;
  VnlVectorType x(c), b(r), br(r);
  VersorType quat;
  VectorType translation;
  MatrixType rotMatrix;

  // Fill the matrix of M
  for (k = 0; k < num; k++)
    {
    quat.Set( this->m_Quaternion[1][k], this->m_Quaternion[2][k], this->m_Quaternion[3][k], this->m_Quaternion[0][k]);
    rotMatrix = quat.GetMatrix();

    for ( j = 0; j < 3; j++)
      {
      for ( i = 0; i < axis; i++)
        {
        matrix[3 * k + j][i] = rotMatrix[j][2 - i];
        }
      for ( i = 0; i < 3; i++)
        {        
        matrix[3 * k + j][i + axis] = 0.0;
        }
      matrix[3 * k + j][j + axis] = -1.0;
      }

    for ( j = 0; j < 3; j++)
      {
      b[3 * k + j] = -this->m_Translation[j][k];
      }
    }

  // Calculate the M' * M
  m = matrix.transpose() * matrix;

  // Calculate (M' * M)^-1
  minv = vnl_matrix_inverse< double >( m );
  
  // Calculate (M' * M)^-1 * M' * N
  x = minv * matrix.transpose() * b;

  // Extract the offset components
  translation.Fill( 0.0);
  for ( i = 0; i < axis; i++)
    {
    translation[2 - i] = x[i];
    }
  
  // Extract the pivot position
  for ( i = 0; i < 3; i++)
    {
    this->m_PivotPosition[i] = x[i + axis];
    }

  // Set the calibration matrix
  this->m_CalibrationTransform.SetTranslation(translation, 0.1, 1000);

  // Calculate the RMS error
  br = matrix * x - b;
  
  this->m_RMS = sqrt( br.squared_magnitude() / num );

  // Set valid indicator
  this->m_ValidPivotCalibration = true;

}

/** Internal method to calculate the rotation from the input */
void PivotCalibration::InternalCalculateRotation()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::InternalCalculateRotation called...\n" );

  this->InternalAdjustPlaneNormal();
  this->InternalBuildRotation();
}

/** Internal method to adjust the plane normal */
void PivotCalibration::InternalAdjustPlaneNormal()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::InternalAdjustPlaneNormal called...\n" );

  unsigned int i;
  VnlVectorType xvec(3), yvec(3), zvec(3);

  // Normalize the vectors
  for ( i = 0; i < 3; i++)
    {
    yvec[i] = this->m_PlaneNormal[i];
    zvec[i] = this->m_PrincipalAxis[i];
    }
  yvec = yvec.normalize();
  zvec = zvec.normalize();
  
  // Compute the x axis
  xvec = cross_3d( yvec, zvec);

  // Compute the perpendicular yvec
  yvec = cross_3d( zvec, xvec);
  for ( i = 0; i < 3; i++)
    {
    this->m_AdjustedPlaneNormal[i] = yvec[i]; 
    }

}

/** Internal method to build the rotation */
void PivotCalibration::InternalBuildRotation()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::InternalBuildRotation called...\n" );

  unsigned int i;
  VnlVectorType xvec(3), yvec(3), zvec(3);
  VersorType::MatrixType orthomatrix;
  VersorType quaternion;

  // Set three axis
  for ( i = 0; i < 3; i++)
    {
    yvec[i] = this->m_AdjustedPlaneNormal[i];
    zvec[i] = this->m_PrincipalAxis[i];
    }
  yvec = yvec.normalize();
  zvec = zvec.normalize();
  xvec = cross_3d( yvec, zvec);

  // Fill the orthogonal matrix
  for ( i = 0; i < 3; i++)
    {
    orthomatrix[0][i] = xvec[i];
    orthomatrix[1][i] = yvec[i];
    orthomatrix[2][i] = zvec[i];
    }
  
  quaternion.Set( orthomatrix);
  this->m_CalibrationTransform.SetRotation( quaternion, 0.1, 1000);

}

/** Method to calculate the calibration */
void PivotCalibration::CalculateCalibration()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::CalculateCalibration called...\n" );

  this->InternalCalculateCalibration( 3);
}

/** Method to calculate the calibration along z-axis */
void PivotCalibration::CalculateCalibrationZ()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::CalculateCalibrationZ called...\n" );

  this->InternalCalculateCalibration( 1);
}

/** Calculate the simulated pivot position */
void PivotCalibration::SimulatePivotPosition()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::SimulatePivotPosition called...\n" );
  
  this->m_SimulatedPivotPositionToBeReceived = this->InternalSimulatePivotPosition( m_QuaternionToBeSent, m_TranslationToBeSent);
}

/** Internal function to calculate the simulated pivot position */
PivotCalibration::VectorType PivotCalibration::InternalSimulatePivotPosition( VersorType quat, VectorType trans )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::InternalSimulatePivotPosition called...\n" );

  /** reconstruct the pivot position from any input translation and rotation
   * 
   *  Pos = Rotation * Offset + Translation
   *
   */

  unsigned int i, j;
  MatrixType rotMatrix;
  VectorType pivotPosition;
  VnlMatrixType matrix(3, 3);
  VnlVectorType translation(3), pos(3), offset(3);

  rotMatrix = quat.GetMatrix();
  
  for ( j = 0; j < 3; j++)
    {
    for ( i = 0; i < 3; i++)
      {
      matrix[j][i] = rotMatrix[j][i];
      }
    translation[j] = trans[j];
    offset[j] = this->GetCalibrationTransform().GetTranslation()[j];
    }

  pos = matrix * offset + translation;

  for ( i = 0; i < 3; i++)
    {
    pivotPosition[i] = pos[i];
    }

  return pivotPosition;

}

/** Get the rotation and translation inputed */
void PivotCalibration::GetInputRotationTranslation()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::GetInputRotationTranslation called...\n" );

  this->m_ValidInputSample = this->InternalGetInputRotationTranslation( this->m_InputIndexToBeSent, this->m_QuaternionToBeReceived, this->m_TranslationToBeReceived);

}

/** Internal method to get the rotation and translation inputed */
bool PivotCalibration::InternalGetInputRotationTranslation( int index, VersorType& quat, VectorType& trans )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::InternalGetInputRotationTranslation called...\n" );

  unsigned int i;

  if (index >= 0 && index < this->GetNumberOfFrame())
  {
    quat.Set( this->m_Quaternion[1][index], this->m_Quaternion[2][index], this->m_Quaternion[3][index], this->m_Quaternion[0][index]);
    for ( i = 0; i < 3; i++)
    {
      trans[i] = this->m_Translation[i][index];
    }

    return true;
  }
  else
  {
    quat.SetIdentity();
    trans.Fill( 0.0);

    return false;
  }
}

/** Set the principal axis of the tool */
void PivotCalibration::SetToolPrincipalAxis()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::SetToolPrincipalAxis called...\n" );

  this->InternalSetToolPrincipalAxis( this->m_VectorToBeSent[0], this->m_VectorToBeSent[1], this->m_VectorToBeSent[2] );
  this->InternalCalculateRotation();
}

/** Internal function to set the principal axis of the tool */
void PivotCalibration::InternalSetToolPrincipalAxis( double vx, double vy, double vz )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::InternalSetToolPrincipalAxis called...\n" );

  this->m_PrincipalAxis[0] = vx;
  this->m_PrincipalAxis[1] = vy;
  this->m_PrincipalAxis[2] = vz;
}

/** Set the normal of the tool plane */
void PivotCalibration::SetToolPlaneNormal()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::SetToolPlaneNormal called...\n" );

  this->InternalSetToolPlaneNormal( this->m_VectorToBeSent[0], this->m_VectorToBeSent[1], this->m_VectorToBeSent[2] );
  this->InternalCalculateRotation();
}

/** Internal function to set the normal of the tool plane */
void PivotCalibration::InternalSetToolPlaneNormal( double nx, double ny, double nz )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::InternalSetToolPrincipalAxis called...\n" );

  this->m_PlaneNormal[0] = nx;
  this->m_PlaneNormal[1] = ny;
  this->m_PlaneNormal[2] = nz;
}

/** Set the translation directly */
void PivotCalibration::SetTranslation()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::SetTranslation called...\n" );

  this->InternalSetTranslation( this->m_TranslationToBeSent[0], this->m_TranslationToBeSent[1], this->m_TranslationToBeSent[2] );
}

/** Internal function to set the translation directly */
void PivotCalibration::InternalSetTranslation( double tx, double ty, double tz )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::InternalSetTranslation called...\n" );

  VectorType translation;

  translation[0] = tx;
  translation[1] = ty;
  translation[2] = tz;
  this->m_CalibrationTransform.SetTranslation( translation, 0.1, 1000 );
}

/** Set the rotation quaternion directly */
void PivotCalibration::SetQuaternion()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::SetQuaternion called...\n" );

  this->InternalSetQuaternion( this->m_QuaternionToBeSent.GetX(), this->m_QuaternionToBeSent.GetY(), this->m_QuaternionToBeSent.GetZ(), this->m_QuaternionToBeSent.GetW() );
}

/** Internal function to set the rotation quaternion directly */
void PivotCalibration::InternalSetQuaternion( double qx, double qy, double qz, double q0 )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::InternalSetQuaternion called...\n" );

  VersorType quaternion;

  quaternion.Set( qx, qy, qz, q0);
  this->m_CalibrationTransform.SetRotation( quaternion, 0.1, 1000);
}

/** Set the rotation matrix directly */
void PivotCalibration::SetRotationMatrix()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::SetRotationMatrix called...\n" );

  this->InternalSetRotationMatrix( this->m_MatrixToBeSent );
}

/** Internal function to set the rotation matrix directly */
void PivotCalibration::InternalSetRotationMatrix( MatrixType matrix )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::InternalSetRotationMatrix called...\n" );

  VersorType quaternion;

  quaternion.Set( matrix);
  this->m_CalibrationTransform.SetRotation( quaternion, 0.1, 1000 );
}

/** Method to invoke the reset function */
void PivotCalibration::RequestReset()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::RequestReset called...\n" );

  this->m_StateMachine.PushInput( this->m_ResetCalibrationInput );
  this->m_StateMachine.ProcessInputs();
}

/** Method to invoke adding the sample */
int PivotCalibration::RequestAddRotationTranslation( VersorType quat, VectorType trans )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::RequestAddRotationTranslation called...\n" );
  
  this->m_QuaternionToBeSent = quat;
  this->m_TranslationToBeSent = trans;
  this->m_StateMachine.PushInput( this->m_RotationTranslationInput );
  this->m_StateMachine.ProcessInputs();

  return this->m_NumberOfFrameToBeReceived;
}

/** Method to invoke the calculation */
void PivotCalibration::RequestCalculateCalibration()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::RequestCalculateCalibration called...\n" );

  this->m_StateMachine.PushInput( this->m_CalculateCalibrationInput );
  this->m_StateMachine.ProcessInputs();

}

/** Method to invoke the calculation only along z-axis */
void PivotCalibration::RequestCalculateCalibrationZ()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::RequestCalculateCalibrationZ called...\n" );

  this->m_StateMachine.PushInput( this->m_CalculateCalibrationZInput );
  this->m_StateMachine.ProcessInputs();

}

/** Method to invoke to simulate the pivot position */
PivotCalibration::VectorType PivotCalibration::RequestSimulatePivotPosition( VersorType quat, VectorType trans )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::RequestSimulatePivotPosition called...\n" );

  this->m_QuaternionToBeSent = quat;
  this->m_TranslationToBeSent = trans;
  this->m_StateMachine.PushInput( this->m_SimulatePivotPositionInput );
  this->m_StateMachine.ProcessInputs();

  return this->m_SimulatedPivotPositionToBeReceived;
}

/** Method to invoke to get the rotation and translation in the input container */
bool PivotCalibration::RequestGetInputRotationTranslation( int index, VersorType& quat, VectorType& trans )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::RequestGetInputRotationTranslation called...\n" );

  this->m_InputIndexToBeSent = index;
  this->m_ValidInputSample = false;
  this->m_StateMachine.PushInput( this->m_GetInputRotationTranslationInput );
  this->m_StateMachine.ProcessInputs();

  quat = this->m_QuaternionToBeReceived;
  trans = this->m_TranslationToBeReceived;

  return this->m_ValidInputSample;

}

/** Method to invoke to set the principal axis of the tool */
void PivotCalibration::RequestSetToolPrincipalAxis( double vx, double vy, double vz )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::RequestSetToolPrincipalAxis called...\n" );

  this->m_VectorToBeSent[0] = vx;
  this->m_VectorToBeSent[1] = vy;
  this->m_VectorToBeSent[2] = vz;
  this->m_StateMachine.PushInput( this->m_PrincipalAxisInput );
  this->m_StateMachine.ProcessInputs();

}

/** Method to invoke to set the normal of the tool plane */
void PivotCalibration::RequestSetToolPlaneNormal( double nx, double ny, double nz )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::RequestSetToolPlaneNormal called...\n" );

  this->m_VectorToBeSent[0] = nx;
  this->m_VectorToBeSent[1] = ny;
  this->m_VectorToBeSent[2] = nz;
  this->m_StateMachine.PushInput( this->m_PlaneNormalInput );
  this->m_StateMachine.ProcessInputs();
}

/** Method to invoke to set the translation directly */
void PivotCalibration::RequestSetTranslation( double tx, double ty, double tz )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::RequestSetTranslation called...\n" );

  this->m_TranslationToBeSent[0] = tx;
  this->m_TranslationToBeSent[1] = ty;
  this->m_TranslationToBeSent[2] = tz;
  this->m_StateMachine.PushInput( this->m_TranslationInput );
  this->m_StateMachine.ProcessInputs();
}

/** Method to invoke to set the rotation quaternion directly */
void PivotCalibration::RequestSetQuaternion( double qx, double qy, double qz, double q0 )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::RequestSetQuaternion called...\n" );

  this->m_QuaternionToBeSent.Set( qx, qy, qz, q0 );
  this->m_StateMachine.PushInput( this->m_QuaternionInput );
  this->m_StateMachine.ProcessInputs();
}

/** Method to invoke to set the rotation matrix directly */
void PivotCalibration::RequestSetRotationMatrix( double matrix[3][3] )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::RequestSetRotationMatrix called...\n" );

  unsigned int i, j;

  for ( j = 0; j < 3; j++)
    { 
    for ( i = 0; i < 3; i++)
      {
      this->m_MatrixToBeSent[j][i] = matrix[j][i];
      }
    }
  this->m_StateMachine.PushInput( this->m_RotationMatrixInput );
  this->m_StateMachine.ProcessInputs();
}

} // end namespace igstk

#endif
