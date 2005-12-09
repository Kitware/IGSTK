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
  this->m_StateMachine.AddState( this->m_IdleState, 
                            "IdleState");
  this->m_StateMachine.AddState( this->m_RotationTranslationAddState, 
                            "RotationTranslationAddState");
  this->m_StateMachine.AddState( this->m_CalibrationCalculatedState, 
                            "CalibrationCalculatedState");
  this->m_StateMachine.AddState( this->m_CalibrationZCalculatedState, 
                            "CalibrationZCalculatedState");

  // Set the input descriptors 
  this->m_StateMachine.AddInput( this->m_ResetCalibrationInput, 
                            "ResetCalibrationInput");
  this->m_StateMachine.AddInput( this->m_RotationTranslationInput, 
                            "RotationTranslationInput");
  this->m_StateMachine.AddInput( this->m_CalculateCalibrationInput, 
                            "CalculateCalibrationInput");
  this->m_StateMachine.AddInput( this->m_CalculateCalibrationZInput, 
                            "CalculateCalibrationZInput");
  this->m_StateMachine.AddInput( this->m_SimulatePivotPositionInput, 
                            "SimulatePivotPositionInput");
  this->m_StateMachine.AddInput( this->m_GetInputRotationTranslationInput, 
                            "GetInputRotationTranslationInput");
  this->m_StateMachine.AddInput( this->m_PrincipalAxisInput, 
                            "PrincipalAxisInput");
  this->m_StateMachine.AddInput( this->m_PlaneNormalInput, 
                            "PlaneNormalInput");
  this->m_StateMachine.AddInput( this->m_TranslationInput, 
                            "TranslationInput");
  this->m_StateMachine.AddInput( this->m_QuaternionInput, 
                            "QuaternionInput");
  this->m_StateMachine.AddInput( this->m_RotationMatrixInput, 
                            "RotationMatrixInput");

  // Add transition  for idle state
  this->m_StateMachine.AddTransition( this->m_IdleState,
                                this->m_ResetCalibrationInput,
                                this->m_IdleState,
                                &PivotCalibration::Reset );

  this->m_StateMachine.AddTransition( this->m_IdleState,
                                this->m_RotationTranslationInput,
                                this->m_RotationTranslationAddState,
                                &PivotCalibration::AddRotationTranslation );

  this->m_StateMachine.AddTransition( this->m_IdleState,
                                this->m_CalculateCalibrationInput,
                                this->m_IdleState,
                                NULL );

  this->m_StateMachine.AddTransition( this->m_IdleState,
                                this->m_CalculateCalibrationZInput,
                                this->m_IdleState,
                                NULL );

  this->m_StateMachine.AddTransition( this->m_IdleState,
                                this->m_SimulatePivotPositionInput,
                                this->m_IdleState,
                                NULL );

  this->m_StateMachine.AddTransition( this->m_IdleState,
                                this->m_GetInputRotationTranslationInput,
                                this->m_IdleState,
                                NULL );

  this->m_StateMachine.AddTransition( this->m_IdleState,
                                this->m_PrincipalAxisInput,
                                this->m_IdleState,
                                &PivotCalibration::SetToolPrincipalAxis );

  this->m_StateMachine.AddTransition( this->m_IdleState,
                                this->m_PlaneNormalInput,
                                this->m_IdleState,
                                &PivotCalibration::SetToolPlaneNormal );

  this->m_StateMachine.AddTransition( this->m_IdleState,
                                this->m_TranslationInput,
                                this->m_IdleState,
                                &PivotCalibration::SetTranslation );

  this->m_StateMachine.AddTransition( this->m_IdleState,
                                this->m_QuaternionInput,
                                this->m_IdleState,
                                &PivotCalibration::SetQuaternion );

  this->m_StateMachine.AddTransition( this->m_IdleState,
                                this->m_RotationMatrixInput,
                                this->m_IdleState,
                                &PivotCalibration::SetRotationMatrix );

  // Add transition  for RotationTranslationAdd state
  this->m_StateMachine.AddTransition( this->m_RotationTranslationAddState,
                                this->m_ResetCalibrationInput,
                                this->m_IdleState,
                                &PivotCalibration::Reset );

  this->m_StateMachine.AddTransition( this->m_RotationTranslationAddState,
                                this->m_RotationTranslationInput,
                                this->m_RotationTranslationAddState,
                                &PivotCalibration::AddRotationTranslation );

  this->m_StateMachine.AddTransition( this->m_RotationTranslationAddState,
                                this->m_CalculateCalibrationInput,
                                this->m_CalibrationCalculatedState,
                                &PivotCalibration::CalculateCalibration );

  this->m_StateMachine.AddTransition( this->m_RotationTranslationAddState,
                                this->m_CalculateCalibrationZInput,
                                this->m_CalibrationZCalculatedState,
                                &PivotCalibration::CalculateCalibrationZ );

  this->m_StateMachine.AddTransition( this->m_RotationTranslationAddState,
                                this->m_SimulatePivotPositionInput,
                                this->m_RotationTranslationAddState,
                                NULL );

  this->m_StateMachine.AddTransition( this->m_RotationTranslationAddState,
                                this->m_GetInputRotationTranslationInput,
                                this->m_RotationTranslationAddState,
                                &PivotCalibration::GetInputRotationTranslation );

  this->m_StateMachine.AddTransition( this->m_RotationTranslationAddState,
                                this->m_PrincipalAxisInput,
                                this->m_RotationTranslationAddState,
                                &PivotCalibration::SetToolPrincipalAxis );

  this->m_StateMachine.AddTransition( this->m_RotationTranslationAddState,
                                this->m_PlaneNormalInput,
                                this->m_RotationTranslationAddState,
                                &PivotCalibration::SetToolPlaneNormal );

  this->m_StateMachine.AddTransition( this->m_RotationTranslationAddState,
                                this->m_TranslationInput,
                                this->m_RotationTranslationAddState,
                                &PivotCalibration::SetTranslation );

  this->m_StateMachine.AddTransition( this->m_RotationTranslationAddState,
                                this->m_QuaternionInput,
                                this->m_RotationTranslationAddState,
                                &PivotCalibration::SetQuaternion );

  this->m_StateMachine.AddTransition( this->m_RotationTranslationAddState,
                                this->m_RotationMatrixInput,
                                this->m_RotationTranslationAddState,
                                &PivotCalibration::SetRotationMatrix );

  // Add transition  for CalibrationCalculated state
  this->m_StateMachine.AddTransition( this->m_CalibrationCalculatedState,
                                this->m_ResetCalibrationInput,
                                this->m_IdleState,
                                &PivotCalibration::Reset );

  this->m_StateMachine.AddTransition( this->m_CalibrationCalculatedState,
                                this->m_RotationTranslationInput,
                                this->m_RotationTranslationAddState,
                                &PivotCalibration::AddRotationTranslation );

  this->m_StateMachine.AddTransition( this->m_CalibrationCalculatedState,
                                this->m_CalculateCalibrationInput,
                                this->m_CalibrationCalculatedState,
                                NULL );

  this->m_StateMachine.AddTransition( this->m_CalibrationCalculatedState,
                                this->m_CalculateCalibrationZInput,
                                this->m_CalibrationZCalculatedState,
                                &PivotCalibration::CalculateCalibrationZ );

  this->m_StateMachine.AddTransition( this->m_CalibrationCalculatedState,
                                this->m_SimulatePivotPositionInput,
                                this->m_CalibrationCalculatedState,
                                &PivotCalibration::SimulatePivotPosition );

  this->m_StateMachine.AddTransition( this->m_CalibrationCalculatedState,
                                this->m_GetInputRotationTranslationInput,
                                this->m_CalibrationCalculatedState,
                                &PivotCalibration::GetInputRotationTranslation );

  this->m_StateMachine.AddTransition( this->m_CalibrationCalculatedState,
                                this->m_PrincipalAxisInput,
                                this->m_CalibrationCalculatedState,
                                &PivotCalibration::SetToolPrincipalAxis );

  this->m_StateMachine.AddTransition( this->m_CalibrationCalculatedState,
                                this->m_PlaneNormalInput,
                                this->m_CalibrationCalculatedState,
                                &PivotCalibration::SetToolPlaneNormal );

  this->m_StateMachine.AddTransition( this->m_CalibrationCalculatedState,
                                this->m_TranslationInput,
                                this->m_CalibrationCalculatedState,
                                &PivotCalibration::SetTranslation );

  this->m_StateMachine.AddTransition( this->m_CalibrationCalculatedState,
                                this->m_QuaternionInput,
                                this->m_CalibrationCalculatedState,
                                &PivotCalibration::SetQuaternion );

  this->m_StateMachine.AddTransition( this->m_CalibrationCalculatedState,
                                this->m_RotationMatrixInput,
                                this->m_CalibrationCalculatedState,
                                &PivotCalibration::SetRotationMatrix );

  // Add transition  for CalibrationZCalculated state
  this->m_StateMachine.AddTransition( this->m_CalibrationZCalculatedState,
                                this->m_ResetCalibrationInput,
                                this->m_IdleState,
                                &PivotCalibration::Reset );

  this->m_StateMachine.AddTransition( this->m_CalibrationZCalculatedState,
                                this->m_RotationTranslationInput,
                                this->m_RotationTranslationAddState,
                                &PivotCalibration::AddRotationTranslation );

  this->m_StateMachine.AddTransition( this->m_CalibrationZCalculatedState,
                                this->m_CalculateCalibrationInput,
                                this->m_CalibrationCalculatedState,
                                &PivotCalibration::CalculateCalibration );

  this->m_StateMachine.AddTransition( this->m_CalibrationZCalculatedState,
                                this->m_CalculateCalibrationZInput,
                                this->m_CalibrationZCalculatedState,
                                NULL );

  this->m_StateMachine.AddTransition( this->m_CalibrationZCalculatedState,
                                this->m_SimulatePivotPositionInput,
                                this->m_CalibrationZCalculatedState,
                                &PivotCalibration::SimulatePivotPosition );

  this->m_StateMachine.AddTransition( this->m_CalibrationZCalculatedState,
                                this->m_GetInputRotationTranslationInput,
                                this->m_CalibrationZCalculatedState,
                                &PivotCalibration::GetInputRotationTranslation );

  this->m_StateMachine.AddTransition( this->m_CalibrationZCalculatedState,
                                this->m_PrincipalAxisInput,
                                this->m_CalibrationZCalculatedState,
                                &PivotCalibration::SetToolPrincipalAxis );

  this->m_StateMachine.AddTransition( this->m_CalibrationZCalculatedState,
                                this->m_PlaneNormalInput,
                                this->m_CalibrationZCalculatedState,
                                &PivotCalibration::SetToolPlaneNormal );

  this->m_StateMachine.AddTransition( this->m_CalibrationZCalculatedState,
                                this->m_TranslationInput,
                                this->m_CalibrationZCalculatedState,
                                &PivotCalibration::SetTranslation );

  this->m_StateMachine.AddTransition( this->m_CalibrationZCalculatedState,
                                this->m_QuaternionInput,
                                this->m_CalibrationZCalculatedState,
                                &PivotCalibration::SetQuaternion );

  this->m_StateMachine.AddTransition( this->m_CalibrationZCalculatedState,
                                this->m_RotationMatrixInput,
                                this->m_CalibrationZCalculatedState,
                                &PivotCalibration::SetRotationMatrix );

  // Select the initial state of the state machine
  this->m_StateMachine.SelectInitialState( this->m_IdleState );

  // Finish the programming and get ready to run
  this->m_StateMachine.SetReadyToRun();

  // Allocte the container pointer for input samples
  this->m_QuaternionContainer = InputVersorContainerType::New();
  this->m_TranslationContainer = InputVectorContainerType::New();

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
  os << indent << "Pivot Calibration: " << std::endl;

  os << indent << "Number Of Samples: " << this->GetNumberOfSamples() << std::endl;

  os << indent << "Calibration Transform: " << this->m_CalibrationTransform << std::endl;
  
  os << indent << "Pivot Position: " << this->m_PivotPosition << std::endl;
  
  os << indent << "Calibration RMS: " << this->m_RMS << std::endl;

}

/** Method to return the number of samples */
int PivotCalibration::GetNumberOfSamples() const
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::GetNumberOfSamples called...\n" );

  return this->m_QuaternionContainer->Size();

}

/** Method to reset the calibration */
void PivotCalibration::Reset()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::Reset called...\n" );

  VersorType quaternion;
  VectorType translation;

  // Clear the input container for quaternion and translation
  this->m_QuaternionContainer->Initialize();
  this->m_TranslationContainer->Initialize();

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
  
  this->m_NumberOfSamplesToBeReceived = this->InternalAddRotationTranslation( this->m_QuaternionToBeSent, this->m_TranslationToBeSent);
}

/** Internal method to add the sample information */
int PivotCalibration::InternalAddRotationTranslation( VersorType quaternion, VectorType translation )
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::InternalAddRotationTranslation called...\n" );

  // Push the quaternion sample into the input container
  this->m_QuaternionContainer->push_back( quaternion );

  // Push the translation sample into the input container
  this->m_TranslationContainer->push_back( translation );

  // Unvalid the calibration
  this->m_ValidPivotCalibration = false;

  // Return the number of current samples;
  return this->GetNumberOfSamples();
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
   *  or [ Offset0 Offset1 Offset2 x0 y0 z0]' = SVD( M, N )
   *  RMS = sqrt( |M * [ Offset0 Offset1 Offset2 x0 y0 z0 ]' - N|^2 / num ) */   

  igstkLogMacro( DEBUG, "igstk::PivotCalibration::InternalCalculateCalibration called...\n" );

  unsigned int i, j, k;
  unsigned int r, c, num;

  // Set the number of sample, tow and column number of matrix
  num = this->GetNumberOfSamples();
  r = num * 3;
  c = 3 + axis;

  // Define the Vnl matrix and intermediate variables
  VnlMatrixType matrix(r, c);
  VnlVectorType x(c), b(r), br(r);  
  VersorType quat;
  VectorType translation;
  MatrixType rotMatrix;

  // Fill the matrix of M
  for (k = 0; k < num; k++)
    {
    quat = this->m_QuaternionContainer->GetElement(k);
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
        b[3 * k + j] = -this->m_TranslationContainer->GetElement(k)[j];
      }
    }

  // Use SVD to solve the vector M * x = y
  VnlSVDType svd( matrix);
  x = svd.solve( b);

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

  VectorType vec[3];

  // Assign and normalize the vectors
  vec[1].SetVnlVector( this->m_PlaneNormal.GetVnlVector());
  vec[1].Normalize();
  vec[2].SetVnlVector( this->m_PrincipalAxis.GetVnlVector());
  vec[2].Normalize();

  // Compute the x axis
  vec[0] = CrossProduct( vec[1], vec[2]);

  // Compute the perpendicular yvec
  vec[1] = CrossProduct( vec[2], vec[0]);

  // No direct conversion from itk::Vector to itk::ConvariantVector
  this->m_AdjustedPlaneNormal.SetVnlVector( vec[1].GetVnlVector());
}

/** Internal method to build the rotation */
void PivotCalibration::InternalBuildRotation()
{
  igstkLogMacro( DEBUG, "igstk::PivotCalibration::InternalBuildRotation called...\n" );

  unsigned int i, j;
  VectorType vec[3];
  VersorType::MatrixType orthomatrix;
  VersorType quaternion;

  // Set three axis
  vec[1].SetVnlVector( this->m_AdjustedPlaneNormal.GetVnlVector());
  vec[1].Normalize();
  vec[2].SetVnlVector( this->m_PrincipalAxis.GetVnlVector());
  vec[2].Normalize();
  vec[0] = CrossProduct( vec[1], vec[2]);

  // Fill the orthogonal matrix
  for ( j = 0; j < 3; j++)
  {
    for ( i = 0; i < 3; i++)
      {
      orthomatrix[j][i] = vec[j][i];
      }
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
  
  this->m_SimulatedPivotPositionToBeReceived = this->InternalSimulatePivotPosition( this->m_QuaternionToBeSent, this->m_TranslationToBeSent);
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

  MatrixType rotMatrix;
  VectorType pivotPosition;
  VnlMatrixType matrix(3, 3);
  VnlVectorType translation(3), pos(3), offset(3);

  rotMatrix = quat.GetMatrix();
  matrix = rotMatrix.GetVnlMatrix();
  translation = trans.GetVnlVector();
  offset = this->GetCalibrationTransform().GetTranslation().GetVnlVector();
  pos = matrix * offset + translation;
  pivotPosition.SetVnlVector( pos);

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

  if (index >= 0 && index < this->GetNumberOfSamples())
    {
    quat = this->m_QuaternionContainer->GetElement( index);
    trans = this->m_TranslationContainer->GetElement( index);

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

  return this->m_NumberOfSamplesToBeReceived;
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
