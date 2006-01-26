/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPrincipalAxisCalibration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more DEBUGrmation.

=========================================================================*/

#ifndef _igstkPrincipalAxisCalibration_cxx
#define _igstkPrincipalAxisCalibration_cxx


#include "igstkPrincipalAxisCalibration.h"
#include "itkVersorRigid3DTransform.h"

namespace igstk
{

/** Constructor */
PrincipalAxisCalibration::PrincipalAxisCalibration() :
  m_StateMachine( this )
{
  // Set the state descriptors
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( InitialOrientationSet );
  igstkAddStateMacro( DesiredOrientationSet );
  igstkAddStateMacro( OrientationAllSet );
  igstkAddStateMacro( RotationCalculated );

  // Set the input descriptors 
  igstkAddInputMacro( ResetCalibration );
  igstkAddInputMacro( InitialOrientation );
  igstkAddInputMacro( DesiredOrientation );
  igstkAddInputMacro( CalculateRotation );

  // Add transition  for idle state
  igstkAddTransitionMacro( Idle, ResetCalibration, Idle, Reset );
  igstkAddTransitionMacro( Idle, InitialOrientation, InitialOrientationSet, SetInitialOrientation );
  igstkAddTransitionMacro( Idle, DesiredOrientation, DesiredOrientationSet, SetDesiredOrientation );
  igstkAddTransitionMacro( Idle, CalculateRotation, Idle, No );

  // Add transition  for InitialOrientationSet state
  igstkAddTransitionMacro( InitialOrientationSet, ResetCalibration, Idle, Reset );
  igstkAddTransitionMacro( InitialOrientationSet, InitialOrientation, InitialOrientationSet, SetInitialOrientation );
  igstkAddTransitionMacro( InitialOrientationSet, DesiredOrientation, OrientationAllSet, SetDesiredOrientation );
  igstkAddTransitionMacro( InitialOrientationSet, CalculateRotation, InitialOrientationSet, No );

  // Add transition  for DesiredOrientationSet state
  igstkAddTransitionMacro( DesiredOrientationSet, ResetCalibration, Idle, Reset );
  igstkAddTransitionMacro( DesiredOrientationSet, InitialOrientation, OrientationAllSet, SetInitialOrientation );
  igstkAddTransitionMacro( DesiredOrientationSet, DesiredOrientation, DesiredOrientationSet, SetDesiredOrientation );
  igstkAddTransitionMacro( DesiredOrientationSet, CalculateRotation, DesiredOrientationSet, No );

  // Add transition  for OrientationAllSet state
  igstkAddTransitionMacro( OrientationAllSet, ResetCalibration, Idle, Reset );
  igstkAddTransitionMacro( OrientationAllSet, InitialOrientation, OrientationAllSet, SetInitialOrientation );
  igstkAddTransitionMacro( OrientationAllSet, DesiredOrientation, OrientationAllSet, SetDesiredOrientation );
  igstkAddTransitionMacro( OrientationAllSet, CalculateRotation, RotationCalculated, CalculateRotation );

  // Add transition  for RotationCalculated state
  igstkAddTransitionMacro( RotationCalculated, ResetCalibration, Idle, Reset );
  igstkAddTransitionMacro( RotationCalculated, InitialOrientation, RotationCalculated, SetInitialOrientation );
  igstkAddTransitionMacro( RotationCalculated, DesiredOrientation, RotationCalculated, SetDesiredOrientation );
  igstkAddTransitionMacro( RotationCalculated, CalculateRotation, RotationCalculated, No );
  
  // Select the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

  // Finish the programming and get ready to run
  this->m_StateMachine.SetReadyToRun();

  // Reset the initial state and variables
  this->ResetProcessing();
}

/** Destructor */
PrincipalAxisCalibration::~PrincipalAxisCalibration()
{

}

/** Print Self function */
void PrincipalAxisCalibration::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  // Dump the calibration class information
  os << indent << "Initial Principal Axis: " << this->m_InitialPrincipalAxis << std::endl;

  os << indent << "Initial Plane Normal: " << this->m_InitialPlaneNormal << std::endl;

  os << indent << "Initial Adjusted Plane Normal: " << this->m_InitialAdjustedPlaneNormal << std::endl;

  os << indent << "Desired Principal Axis: " << this->m_DesiredPrincipalAxis << std::endl;

  os << indent << "Desired Plane Normal: " << this->m_DesiredPlaneNormal << std::endl;

  os << indent << "Desired Adjusted Plane Normal: " << this->m_DesiredAdjustedPlaneNormal << std::endl;

  os << indent << "Calibration Transform: " << this->m_CalibrationTransform << std::endl;
}

/** Method to reset the calibration */
void PrincipalAxisCalibration::NoProcessing()
{
}

/** Method to reset the calibration */
void PrincipalAxisCalibration::ResetProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PrincipalAxisCalibration::ResetProcessing called...\n" );

  // Reset the initial principal axis
  this->m_InitialPrincipalAxis[0] = 0.0;
  this->m_InitialPrincipalAxis[1] = 1.0;
  this->m_InitialPrincipalAxis[2] = 0.0;

  // Reset the initial plane normal
  this->m_InitialPlaneNormal[0] = 0.0;
  this->m_InitialPlaneNormal[1] = 0.0;
  this->m_InitialPlaneNormal[2] = 1.0;

  // Reset the initial adjusted plane normal
  this->m_InitialAdjustedPlaneNormal = this->m_InitialPlaneNormal;

  // Reset the initial principal axis
  this->m_DesiredPrincipalAxis[0] = 0.0;
  this->m_DesiredPrincipalAxis[1] = 0.0;
  this->m_DesiredPrincipalAxis[2] = 1.0;

  // Reset the initial plane normal
  this->m_DesiredPlaneNormal[0] = 0.0;
  this->m_DesiredPlaneNormal[1] = 1.0;
  this->m_DesiredPlaneNormal[2] = 0.0;

  // Reset the initial adjusted plane normal
  this->m_DesiredAdjustedPlaneNormal = this->m_DesiredPlaneNormal;

  // Reset the validation indicator
  this->m_ValidRotation = false;
}


/** Internal method to adjust the plane normal */
PrincipalAxisCalibration::CovariantVectorType 
PrincipalAxisCalibration
::InternalAdjustPlaneNormalProcessing( const VectorType & axis, 
                                       const CovariantVectorType & normal )
{
  igstkLogMacro( DEBUG, "igstk::PrincipalAxisCalibration::InternalAdjustPlaneNormalProcessing called...\n" );

  // Assign and normalize the vectors
  VectorType vecnormal;
  vecnormal.SetVnlVector( normal.GetVnlVector());
  vecnormal.Normalize();

  // Compute the x axis
  VectorType vec0 = itk::CrossProduct( vecnormal, axis );

  // Compute the perpendicular y axis
  CovariantVectorType adjustednormal;

  itk::CrossProduct( adjustednormal, axis, vec0 );
  adjustednormal.Normalize();

  return adjustednormal;
}

/** Internal method to build the rotation */
void PrincipalAxisCalibration::CalculateRotationProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PrincipalAxisCalibration::CalculateRotationProcessing called...\n" );

  typedef VersorType::MatrixType  MatrixType;

  MatrixType initialorthomatrix;
  MatrixType desiredorthomatrix;
  MatrixType matrix;
  VersorType quaternion;

  // Build the orthogonal matrix from the principal axis and plane normal
  initialorthomatrix = this->InternalBuildOrthogonalMatrixProcessing( 
                                              this->m_InitialPrincipalAxis, 
                                              this->m_InitialAdjustedPlaneNormal );

  desiredorthomatrix = this->InternalBuildOrthogonalMatrixProcessing( 
                                              this->m_DesiredPrincipalAxis, 
                                              this->m_DesiredAdjustedPlaneNormal );

  // Find the rotation by M = A / B
  matrix = desiredorthomatrix * initialorthomatrix.GetInverse();
 
  // Set the quaternion
  quaternion.Set( matrix);
  this->m_CalibrationTransform.SetRotation( quaternion, 0.1, 1000);

  // Valid the rotation
  this->m_ValidRotation = true;
}

/** Internal method to construct the orthogonal matrix */
PrincipalAxisCalibration::MatrixType 
PrincipalAxisCalibration
::InternalBuildOrthogonalMatrixProcessing( const VectorType & axis, 
                                           const CovariantVectorType & normal )
{
  igstkLogMacro( DEBUG, "igstk::PrincipalAxisCalibration::InternalBuildOrthogonalMatrixProcessing called...\n" );

  unsigned int i, j;
  VectorType vec[3];
  VersorType::MatrixType orthomatrix;

  // Set three axis
  vec[1].SetVnlVector( normal.GetVnlVector());
  vec[1].Normalize();
  vec[2].SetVnlVector( axis.GetVnlVector());
  vec[2].Normalize();
  vec[0] = itk::CrossProduct( vec[1], vec[2]);

  // Fill the orthogonal matrix
  for ( j = 0; j < 3; j++)
    {
    for ( i = 0; i < 3; i++)
      {
      orthomatrix[i][j] = vec[j][i];
      }
    }

  return orthomatrix;
}

/** Set the initial orientation */
void PrincipalAxisCalibration
::SetInitialOrientationProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PrincipalAxisCalibration::SetInitialOrientationProcessing called...\n" );

  this->InternalSetInitialOrientationProcessing( 
                              this->m_VectorToBeSent, 
                              this->m_CovariantVectorToBeSent );
}

/** Internal function to set the initial orientation */
void PrincipalAxisCalibration
::InternalSetInitialOrientationProcessing( const VectorType & axis, 
                                           const CovariantVectorType & normal )
{
  igstkLogMacro( DEBUG, "igstk::PrincipalAxisCalibration::InternalSetInitialOrientationProcessing called...\n" );

  this->m_InitialPrincipalAxis = axis;
  this->m_InitialPlaneNormal = normal;
  // Adjust the plane normal
  this->m_InitialAdjustedPlaneNormal = this->InternalAdjustPlaneNormalProcessing( 
                                                    this->m_InitialPrincipalAxis,
                                                    this->m_InitialPlaneNormal );

  this->m_ValidRotation = false;
}

/** Set the desired orientation */
void PrincipalAxisCalibration::SetDesiredOrientationProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PrincipalAxisCalibration::SetDesiredOrientationProcessing called...\n" );

  this->InternalSetDesiredOrientationProcessing( this->m_VectorToBeSent, this->m_CovariantVectorToBeSent );
}

/** Internal function to set the desired orientation */
void PrincipalAxisCalibration
::InternalSetDesiredOrientationProcessing( const VectorType & axis, 
                                           const CovariantVectorType & normal )
{
  igstkLogMacro( DEBUG, "igstk::PrincipalAxisCalibration::InternalSetDesiredOrientationProcessing called...\n" );

  this->m_DesiredPrincipalAxis = axis;
  this->m_DesiredPlaneNormal = normal;
  // Adjust the plane normal
  this->m_DesiredAdjustedPlaneNormal = this->InternalAdjustPlaneNormalProcessing( 
                                                    this->m_DesiredPrincipalAxis,
                                                    this->m_DesiredPlaneNormal ); 

  this->m_ValidRotation = false;
}

/** Method to invoke the reset function */
void PrincipalAxisCalibration::RequestReset()
{
  igstkLogMacro( DEBUG, "igstk::PrincipalAxisCalibration::RequestReset called...\n" );

  this->m_StateMachine.PushInput( this->m_ResetCalibrationInput );
  this->m_StateMachine.ProcessInputs();
}

/** Method to invoke to set the initial orientation */
void PrincipalAxisCalibration
::RequestSetInitialOrientation( const VectorType & axis, 
                                const CovariantVectorType & normal )
{
  igstkLogMacro( DEBUG, "igstk::PrincipalAxisCalibration::RequestSetInitialOrientation called...\n" );

  this->m_VectorToBeSent = axis;
  this->m_CovariantVectorToBeSent = normal;
  this->m_StateMachine.PushInput( this->m_InitialOrientationInput );
  this->m_StateMachine.ProcessInputs();
}

/** Method to invoke to set the desired orientation */
void PrincipalAxisCalibration
::RequestSetDesiredOrientation( const VectorType & axis, 
                                const CovariantVectorType & normal )
{
  igstkLogMacro( DEBUG, "igstk::PrincipalAxisCalibration::RequestSetDesiredOrientation called...\n" );

  this->m_VectorToBeSent = axis;
  this->m_CovariantVectorToBeSent = normal;
  this->m_StateMachine.PushInput( this->m_DesiredOrientationInput );
  this->m_StateMachine.ProcessInputs();
}

/** Method to invoke the calculation */
void PrincipalAxisCalibration::RequestCalculateRotation()
{
  igstkLogMacro( DEBUG, "igstk::PrincipalAxisCalibration::RequestCalculateRotation called...\n" );

  this->m_StateMachine.PushInput( this->m_CalculateRotationInput );
  this->m_StateMachine.ProcessInputs();
}

} // end namespace igstk

#endif

