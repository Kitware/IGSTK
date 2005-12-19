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
  igstkAddStateMacro( IdleState );
  igstkAddStateMacro( InitialOrientationSetState );
  igstkAddStateMacro( DesiredOrientationSetState );
  igstkAddStateMacro( OrientationAllSetState );
  igstkAddStateMacro( RotationCalculatedState );

  // Set the input descriptors 
  igstkAddInputMacro( ResetCalibrationInput );
  igstkAddInputMacro( InitialOrientationInput );
  igstkAddInputMacro( DesiredOrientationInput );
  igstkAddInputMacro( CalculateRotationInput );

  // Add transition  for idle state
  igstkAddTransitionMacro( IdleState, ResetCalibrationInput, IdleState, ResetProcessing );
  igstkAddTransitionMacro( IdleState, InitialOrientationInput, InitialOrientationSetState, SetInitialOrientationProcessing );
  igstkAddTransitionMacro( IdleState, DesiredOrientationInput, DesiredOrientationSetState, SetDesiredOrientationProcessing );
  igstkAddTransitionMacro( IdleState, CalculateRotationInput, IdleState, NoAction );

  // Add transition  for InitialOrientationSet state
  igstkAddTransitionMacro( InitialOrientationSetState, ResetCalibrationInput, IdleState, ResetProcessing );
  igstkAddTransitionMacro( InitialOrientationSetState, InitialOrientationInput, InitialOrientationSetState, SetInitialOrientationProcessing );
  igstkAddTransitionMacro( InitialOrientationSetState, DesiredOrientationInput, OrientationAllSetState, SetDesiredOrientationProcessing );
  igstkAddTransitionMacro( InitialOrientationSetState, CalculateRotationInput, InitialOrientationSetState, NoAction );

  // Add transition  for DesiredOrientationSet state
  igstkAddTransitionMacro( DesiredOrientationSetState, ResetCalibrationInput, IdleState, ResetProcessing );
  igstkAddTransitionMacro( DesiredOrientationSetState, InitialOrientationInput, OrientationAllSetState, SetInitialOrientationProcessing );
  igstkAddTransitionMacro( DesiredOrientationSetState, DesiredOrientationInput, DesiredOrientationSetState, SetDesiredOrientationProcessing );
  igstkAddTransitionMacro( DesiredOrientationSetState, CalculateRotationInput, DesiredOrientationSetState, NoAction );

  // Add transition  for OrientationAllSet state
  igstkAddTransitionMacro( OrientationAllSetState, ResetCalibrationInput, IdleState, ResetProcessing );
  igstkAddTransitionMacro( OrientationAllSetState, InitialOrientationInput, OrientationAllSetState, SetInitialOrientationProcessing );
  igstkAddTransitionMacro( OrientationAllSetState, DesiredOrientationInput, OrientationAllSetState, SetDesiredOrientationProcessing );
  igstkAddTransitionMacro( OrientationAllSetState, CalculateRotationInput, RotationCalculatedState, CalculateRotationProcessing );

  // Add transition  for RotationCalculated state
  igstkAddTransitionMacro( RotationCalculatedState, ResetCalibrationInput, IdleState, ResetProcessing );
  igstkAddTransitionMacro( RotationCalculatedState, InitialOrientationInput, RotationCalculatedState, SetInitialOrientationProcessing );
  igstkAddTransitionMacro( RotationCalculatedState, DesiredOrientationInput, RotationCalculatedState, SetDesiredOrientationProcessing );
  igstkAddTransitionMacro( RotationCalculatedState, CalculateRotationInput, RotationCalculatedState, NoAction );
  
  // Select the initial state of the state machine
  this->m_StateMachine.SelectInitialState( this->m_IdleState );

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
void PrincipalAxisCalibration::NoAction()
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
PrincipalAxisCalibration::InternalAdjustPlaneNormalProcessing( VectorType axis, CovariantVectorType normal )
{
  igstkLogMacro( DEBUG, "igstk::PrincipalAxisCalibration::InternalAdjustPlaneNormalProcessing called...\n" );

  // some part will change when dashboards update to CVS ITK: covariant vector cross product and assignment

  VectorType vec[3];
  CovariantVectorType adjustednormal;

  // Assign and normalize the vectors
  vec[1].SetVnlVector( normal.GetVnlVector());
  vec[1].Normalize();
  vec[2].SetVnlVector( axis.GetVnlVector());
  vec[2].Normalize();

  // Compute the x axis
  vec[0] = itk::CrossProduct( vec[1], vec[2]);

  // Compute the perpendicular y axis
  vec[1] = itk::CrossProduct( vec[2], vec[0]);

  // No direct conversion from itk::Vector to itk::ConvariantVector
  adjustednormal.SetVnlVector( vec[1].GetVnlVector());

  return adjustednormal;
}

/** Internal method to build the rotation */
void PrincipalAxisCalibration::CalculateRotationProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PrincipalAxisCalibration::CalculateRotationProcessing called...\n" );

  VersorType::MatrixType initialorthomatrix, desiredorthomatrix, matrix;
  VersorType quaternion;

  // Build the orthogonal matrix from the principal axis and plane normal
  initialorthomatrix = InternalBuildOrthogonalMatrixProcessing( this->m_InitialPrincipalAxis, this->m_InitialAdjustedPlaneNormal );
  desiredorthomatrix = InternalBuildOrthogonalMatrixProcessing( this->m_DesiredPrincipalAxis, this->m_DesiredAdjustedPlaneNormal );

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
PrincipalAxisCalibration::InternalBuildOrthogonalMatrixProcessing( VectorType axis, CovariantVectorType normal )
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
      orthomatrix[j][i] = vec[j][i];
      }
    }

  return orthomatrix;

}

/** Set the initial orientation */
void PrincipalAxisCalibration::SetInitialOrientationProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PrincipalAxisCalibration::SetInitialOrientationProcessing called...\n" );

  this->InternalSetInitialOrientationProcessing( this->m_VectorToBeSent, this->m_CovariantVectorToBeSent );
}

/** Internal function to set the initial orientation */
void PrincipalAxisCalibration::InternalSetInitialOrientationProcessing( VectorType axis, CovariantVectorType normal )
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
void PrincipalAxisCalibration::InternalSetDesiredOrientationProcessing( VectorType axis, CovariantVectorType normal )
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
void PrincipalAxisCalibration::RequestSetInitialOrientation( VectorType axis, CovariantVectorType normal )
{
  igstkLogMacro( DEBUG, "igstk::PrincipalAxisCalibration::RequestSetInitialOrientation called...\n" );

  this->m_VectorToBeSent = axis;
  this->m_CovariantVectorToBeSent = normal;
  this->m_StateMachine.PushInput( this->m_InitialOrientationInput );
  this->m_StateMachine.ProcessInputs();

}

/** Method to invoke to set the desired orientation */
void PrincipalAxisCalibration::RequestSetDesiredOrientation( VectorType axis, CovariantVectorType normal )
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
