/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPointerUltrasoundCalibration.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more DEBUGrmation.

=========================================================================*/

#ifndef _igstkPointerUltrasoundCalibration_cxx
#define _igstkPointerUltrasoundCalibration_cxx


#include "igstkPointerUltrasoundCalibration.h"
#include "itkVersorRigid3DTransform.h"

#include "vtkLandmarkTransform.h"
#include "vtkPoints.h"

namespace igstk
{

/** Constructor */
PointerUltrasoundCalibration::PointerUltrasoundCalibration() :
  m_StateMachine( this )
{
  // Set the state descriptors
  igstkAddStateMacro( Idle );
  igstkAddStateMacro( PointerToolCalibrationSet );
  igstkAddStateMacro( IndexPositionSampleAdd );
  igstkAddStateMacro( ImagePositionSampleAdd );
  igstkAddStateMacro( PointerToolIndexPositionSampleAdd );
  igstkAddStateMacro( PointerToolImagePositionSampleAdd );
  igstkAddStateMacro( CalibrationCalculated );

  // Set the input descriptors 
  igstkAddInputMacro( ResetCalibration );
  igstkAddInputMacro( PointerToolCalibrationTransform );
  igstkAddInputMacro( IndexPositionSample );
  igstkAddInputMacro( ImagePositionSample );
  igstkAddInputMacro( PointerToolIndexPositionSample );
  igstkAddInputMacro( PointerToolImagePositionSample );
  igstkAddInputMacro( CalculateCalibration );

  // Add transition  for idle state
  igstkAddTransitionMacro( Idle, ResetCalibration, Idle, Reset );
  igstkAddTransitionMacro( Idle, PointerToolCalibrationTransform, PointerToolCalibrationSet, SetPointerToolCalibrationTransform );
  igstkAddTransitionMacro( Idle, IndexPositionSample, IndexPositionSampleAdd, AddIndexPositionSample );
  igstkAddTransitionMacro( Idle, ImagePositionSample, ImagePositionSampleAdd, AddImagePositionSample );
  igstkAddTransitionMacro( Idle, PointerToolIndexPositionSample, Idle, No );
  igstkAddTransitionMacro( Idle, PointerToolImagePositionSample, Idle, No );
  igstkAddTransitionMacro( Idle, CalculateCalibration, Idle, No );
  
  // Add transition  for PointerToolCalibrationSet state
  igstkAddTransitionMacro( PointerToolCalibrationSet, ResetCalibration, Idle, Reset );
  igstkAddTransitionMacro( PointerToolCalibrationSet, PointerToolCalibrationTransform, PointerToolCalibrationSet, SetPointerToolCalibrationTransform );
  igstkAddTransitionMacro( PointerToolCalibrationSet, IndexPositionSample, PointerToolCalibrationSet, No );
  igstkAddTransitionMacro( PointerToolCalibrationSet, ImagePositionSample, PointerToolCalibrationSet, No );
  igstkAddTransitionMacro( PointerToolCalibrationSet, PointerToolIndexPositionSample, PointerToolIndexPositionSampleAdd, AddPointerToolIndexPositionSample );
  igstkAddTransitionMacro( PointerToolCalibrationSet, PointerToolImagePositionSample, PointerToolImagePositionSampleAdd, AddPointerToolImagePositionSample );
  igstkAddTransitionMacro( PointerToolCalibrationSet, CalculateCalibration, PointerToolCalibrationSet, No );

  // Add transition  for IndexPositionSampleAdd state
  igstkAddTransitionMacro( IndexPositionSampleAdd, ResetCalibration, Idle, Reset );
  igstkAddTransitionMacro( IndexPositionSampleAdd, PointerToolCalibrationTransform, IndexPositionSampleAdd, No );
  igstkAddTransitionMacro( IndexPositionSampleAdd, IndexPositionSample, IndexPositionSampleAdd, AddIndexPositionSample );
  igstkAddTransitionMacro( IndexPositionSampleAdd, ImagePositionSample, IndexPositionSampleAdd, No );
  igstkAddTransitionMacro( IndexPositionSampleAdd, PointerToolIndexPositionSample, IndexPositionSampleAdd, No );
  igstkAddTransitionMacro( IndexPositionSampleAdd, PointerToolImagePositionSample, IndexPositionSampleAdd, No );
  igstkAddTransitionMacro( IndexPositionSampleAdd, CalculateCalibration, CalibrationCalculated, CalculateCalibrationByIndexPosition );

  // Add transition  for ImagePositionSampleAdd state
  igstkAddTransitionMacro( ImagePositionSampleAdd, ResetCalibration, Idle, Reset );
  igstkAddTransitionMacro( ImagePositionSampleAdd, PointerToolCalibrationTransform, ImagePositionSampleAdd, No );
  igstkAddTransitionMacro( ImagePositionSampleAdd, IndexPositionSample, ImagePositionSampleAdd, No );
  igstkAddTransitionMacro( ImagePositionSampleAdd, ImagePositionSample, ImagePositionSampleAdd, AddImagePositionSample );
  igstkAddTransitionMacro( ImagePositionSampleAdd, PointerToolIndexPositionSample, ImagePositionSampleAdd, No );
  igstkAddTransitionMacro( ImagePositionSampleAdd, PointerToolImagePositionSample, ImagePositionSampleAdd, No );
  igstkAddTransitionMacro( ImagePositionSampleAdd, CalculateCalibration, CalibrationCalculated, CalculateCalibrationByImagePosition );

  // Add transition  for PointerToolIndexPositionSampleAdd state
  igstkAddTransitionMacro( PointerToolIndexPositionSampleAdd, ResetCalibration, Idle, Reset );
  igstkAddTransitionMacro( PointerToolIndexPositionSampleAdd, PointerToolCalibrationTransform, PointerToolIndexPositionSampleAdd, SetPointerToolCalibrationTransform );
  igstkAddTransitionMacro( PointerToolIndexPositionSampleAdd, IndexPositionSample, PointerToolIndexPositionSampleAdd, No );
  igstkAddTransitionMacro( PointerToolIndexPositionSampleAdd, ImagePositionSample, PointerToolIndexPositionSampleAdd, No );
  igstkAddTransitionMacro( PointerToolIndexPositionSampleAdd, PointerToolIndexPositionSample, PointerToolIndexPositionSampleAdd, AddPointerToolIndexPositionSample );
  igstkAddTransitionMacro( PointerToolIndexPositionSampleAdd, PointerToolImagePositionSample, PointerToolIndexPositionSampleAdd, No );
  igstkAddTransitionMacro( PointerToolIndexPositionSampleAdd, CalculateCalibration, CalibrationCalculated, CalculateCalibrationByIndexPosition );

  // Add transition  for PointerToolImagePositionSampleAdd state
  igstkAddTransitionMacro( PointerToolImagePositionSampleAdd, ResetCalibration, Idle, Reset );
  igstkAddTransitionMacro( PointerToolImagePositionSampleAdd, PointerToolCalibrationTransform, PointerToolImagePositionSampleAdd, SetPointerToolCalibrationTransform );
  igstkAddTransitionMacro( PointerToolImagePositionSampleAdd, IndexPositionSample, PointerToolImagePositionSampleAdd, No );
  igstkAddTransitionMacro( PointerToolImagePositionSampleAdd, ImagePositionSample, PointerToolImagePositionSampleAdd, No );
  igstkAddTransitionMacro( PointerToolImagePositionSampleAdd, PointerToolIndexPositionSample, PointerToolImagePositionSampleAdd, No );
  igstkAddTransitionMacro( PointerToolImagePositionSampleAdd, PointerToolImagePositionSample, PointerToolImagePositionSampleAdd, AddPointerToolImagePositionSample );
  igstkAddTransitionMacro( PointerToolImagePositionSampleAdd, CalculateCalibration, CalibrationCalculated, CalculateCalibrationByImagePosition );
  
  // Add transition  for CalibrationCalculated state
  igstkAddTransitionMacro( CalibrationCalculated, ResetCalibration, Idle, Reset );
  igstkAddTransitionMacro( CalibrationCalculated, PointerToolCalibrationTransform, PointerToolCalibrationSet, SetPointerToolCalibrationTransform );
  igstkAddTransitionMacro( CalibrationCalculated, IndexPositionSample, CalibrationCalculated, No );
  igstkAddTransitionMacro( CalibrationCalculated, ImagePositionSample, CalibrationCalculated, No );
  igstkAddTransitionMacro( CalibrationCalculated, PointerToolIndexPositionSample, CalibrationCalculated, No );
  igstkAddTransitionMacro( CalibrationCalculated, PointerToolImagePositionSample, CalibrationCalculated, No );
  igstkAddTransitionMacro( CalibrationCalculated, CalculateCalibration, CalibrationCalculated, No );

  // Select the initial state of the state machine
  igstkSetInitialStateMacro( Idle );

  // Finish the programming and get ready to run
  this->m_StateMachine.SetReadyToRun();

  // Allocte the container pointer for input samples
  this->m_IndexPositionContainer = InputIndexContainerType::New();
  this->m_ImagePositionContainer = InputPointContainerType::New();
  this->m_PointerPositionContainer = InputPointContainerType::New();
  this->m_PointerVersorContainer = InputVersorContainerType::New();
  this->m_PointerTranslationContainer = InputVectorContainerType::New();
  this->m_ProbeVersorContainer = InputVersorContainerType::New();
  this->m_ProbeTranslationContainer = InputVectorContainerType::New();

  this->m_LandmarkTransform = vtkLandmarkTransform::New();

  // ResetProcessing the initial state and variables
  this->ResetProcessing();

}

/** Destructor */
PointerUltrasoundCalibration::~PointerUltrasoundCalibration()
{
  this->m_LandmarkTransform->Delete();

}

/** Print Self function */
void PointerUltrasoundCalibration::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf(os, indent);

  // Dump the calibration class information
  os << indent << "Pointer Ultrasound Calibration: " << std::endl;

  os << indent << "Number Of Samples: " << this->GetNumberOfSamples() << std::endl;

  os << indent << "Calibration Transform: " << this->GetCalibrationTransform() << std::endl;

  os << indent << "Scale Transform: " << this->GetScaleTransform() << std::endl;

  os << indent << "4x4 Calibration Matrix: " << this->GetCalibrationMatrix4x4() << std::endl;
  
  os << indent << "Calibration RMS: " << this->GetRootMeanSquareError() << std::endl;

}

/** Method to return the number of samples */
unsigned int PointerUltrasoundCalibration
::GetNumberOfSamples() const
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::GetNumberOfSamples called...\n" );

  return this->m_PointerPositionContainer->Size();

}

/** Method to NoProcessing */
void PointerUltrasoundCalibration::NoProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::NoProcessing called...\n" );

}

/** Method to reset the calibration */
void PointerUltrasoundCalibration::ResetProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::ResetProcessing called...\n" );

  VersorType quaternion;
  VectorType translation;

  // Clear the input container for quaternion and translation
  this->m_IndexPositionContainer->Initialize();
  this->m_ImagePositionContainer->Initialize();
  this->m_PointerPositionContainer->Initialize();
  this->m_PointerVersorContainer->Initialize();
  this->m_PointerTranslationContainer->Initialize();
  this->m_ProbeVersorContainer->Initialize();
  this->m_ProbeTranslationContainer->Initialize();

  // Reset the calibration transform (rotation and translation)
  quaternion.SetIdentity();
  translation.Fill( 0.0);
  this->m_CalibrationTransform.SetTranslationAndRotation( translation, quaternion, 0.1, 1000);

  // Reset the scale factor
  this->m_ScaleTransform.Fill( 1.0);

  // Reset the final 4x4 calibration transform
  this->m_CalibrationMatrix4x4.SetIdentity();

  // Reset the RMS calibration error
  this->m_RootMeanSquareError = 0.0;

  // Reset the validation indicator
  this->m_ValidPointerUltrasoundCalibration = false;

}

/** Method to add the sample information */
void PointerUltrasoundCalibration::AddIndexPositionSampleProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::AddIndexPositionSampleProcessing called...\n" );

  this->InternalAddIndexPositionSampleProcessing( this->m_IndexPositionToBeSent, 
                                             this->m_PointerPositionToBeSent, 
                                             this->m_ProbeVersorToBeSent, this->m_ProbeTranslationToBeSent );

}

/** Internal method to add the sample information */
void PointerUltrasoundCalibration
::InternalAddIndexPositionSampleProcessing( const IndexType & indexposition,
                               const PointType & pointerposition, 
                               const VersorType & probeversor, const VectorType & probetranslation )
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::InternalAddIndexPositionSampleProcessing called...\n" );

  // Push the sample into the input container
  this->m_IndexPositionContainer->push_back( indexposition );
  this->m_PointerPositionContainer->push_back( pointerposition );
  this->m_ProbeVersorContainer->push_back( probeversor );
  this->m_ProbeTranslationContainer->push_back( probetranslation );

  // Unvalid the calibration
  this->m_ValidPointerUltrasoundCalibration = false;

}

/** Method to add the sample information */
void PointerUltrasoundCalibration::AddImagePositionSampleProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::AddImagePositionSampleProcessing called...\n" );

  this->InternalAddImagePositionSampleProcessing( this->m_ImagePositionToBeSent, 
                                             this->m_PointerPositionToBeSent, 
                                             this->m_ProbeVersorToBeSent, this->m_ProbeTranslationToBeSent );

}

/** Internal method to add the sample information */
void PointerUltrasoundCalibration
::InternalAddImagePositionSampleProcessing( const PointType & imageposition,
                               const PointType & pointerposition, 
                               const VersorType & probeversor, const VectorType & probetranslation )
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::InternalAddImagePositionSampleProcessing called...\n" );

  // Push the sample into the input container
  this->m_ImagePositionContainer->push_back( imageposition );
  this->m_PointerPositionContainer->push_back( pointerposition );
  this->m_ProbeVersorContainer->push_back( probeversor );
  this->m_ProbeTranslationContainer->push_back( probetranslation );

  // Unvalid the calibration
  this->m_ValidPointerUltrasoundCalibration = false;

}

/** Method to add the sample information */
void PointerUltrasoundCalibration::AddPointerToolIndexPositionSampleProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::AddPointerToolIndexPositionSampleProcessing called...\n" );

  this->InternalAddPointerToolIndexPositionSampleProcessing( this->m_IndexPositionToBeSent, 
                                                this->m_PointerVersorToBeSent, this->m_PointerTranslationToBeSent, 
                                                this->m_ProbeVersorToBeSent, this->m_ProbeTranslationToBeSent );
}

/** Internal method to add the sample information */
void PointerUltrasoundCalibration
::InternalAddPointerToolIndexPositionSampleProcessing( const IndexType & indexposition,
                               const VersorType & pointerversor, const VectorType & pointertranslation,
                               const VersorType & probeversor, const VectorType & probetranslation )
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::InternalAddPointerToolIndexPositionSampleProcessing called...\n" );

  PointType pointerposition;

  // Push the sample into the input container
  this->m_IndexPositionContainer->push_back( indexposition );
  this->m_PointerVersorContainer->push_back( pointerversor );
  this->m_PointerTranslationContainer->push_back( pointertranslation );
  pointerposition = this->InternalTransformPointerToolPoint( pointerversor, pointertranslation ); 
  this->m_PointerPositionContainer->push_back( pointerposition);
  this->m_ProbeVersorContainer->push_back( probeversor );
  this->m_ProbeTranslationContainer->push_back( probetranslation );

  // Unvalid the calibration
  this->m_ValidPointerUltrasoundCalibration = false;

}

/** Method to add the sample information */
void PointerUltrasoundCalibration::AddPointerToolImagePositionSampleProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::AddPointerToolImagePositionSampleProcessing called...\n" );

  this->InternalAddPointerToolImagePositionSampleProcessing( this->m_ImagePositionToBeSent, 
                                                this->m_PointerVersorToBeSent, this->m_PointerTranslationToBeSent, 
                                                this->m_ProbeVersorToBeSent, this->m_ProbeTranslationToBeSent );
}

/** Internal method to add the sample information */
void PointerUltrasoundCalibration
::InternalAddPointerToolImagePositionSampleProcessing( const PointType & imageposition,
                               const VersorType & pointerversor, const VectorType & pointertranslation,
                               const VersorType & probeversor, const VectorType & probetranslation )
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::InternalAddPointerToolImagePositionSampleProcessing called...\n" );

  PointType pointerposition;

  // Push the sample into the input container
  this->m_ImagePositionContainer->push_back( imageposition );
  this->m_PointerVersorContainer->push_back( pointerversor );
  this->m_PointerTranslationContainer->push_back( pointertranslation );
  pointerposition = this->InternalTransformPointerToolPoint( pointerversor, pointertranslation ); 
  this->m_PointerPositionContainer->push_back( pointerposition);
  this->m_ProbeVersorContainer->push_back( probeversor );
  this->m_ProbeTranslationContainer->push_back( probetranslation );

  // Unvalid the calibration
  this->m_ValidPointerUltrasoundCalibration = false;

}

/** Internal function to transform the pointer tool's input into position */
PointerUltrasoundCalibration::PointType PointerUltrasoundCalibration
::InternalTransformPointerToolPoint( const VersorType & versor, const VectorType & translation )
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::InternalTransformPointerToolPoint called...\n" );

  // Reconstruct the position from the pointer tool's input
  // Pos = Rotation * Offset + Translation
  typedef itk::VersorRigid3DTransform<double> RigidTransformType;

  RigidTransformType::Pointer rigidTransform = RigidTransformType::New();

  rigidTransform->SetRotation( versor );
  rigidTransform->SetTranslation( translation );

  VectorType offset = this->m_PointerToolCalibrationTransform.GetTranslation();

  VectorType rotatedOffset = rigidTransform->TransformVector( offset );

  rotatedOffset += translation;

  PointType pointerPosition;
  pointerPosition.Fill( 0.0 );
  
  pointerPosition += rotatedOffset;

  return pointerPosition;

}

/** Internal function to reconstruct the coordinate from the index input */
PointerUltrasoundCalibration::PointType 
PointerUltrasoundCalibration::InternalReconstruct( const IndexType & indexposition,
               const VersorType & probeversor, const VectorType & probetranslation )
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::InternalReconstruct called...\n" );

  unsigned int i;
  PointType position;

  for (i = 0; i < 3; i++)
    {
    position[i] = indexposition[i];
    }

  return this->InternalReconstruct( position, probeversor, probetranslation );

}

/** Internal function to reconstruct the coordinate from the image input */
PointerUltrasoundCalibration::PointType 
PointerUltrasoundCalibration::InternalReconstruct( const PointType & imageposition,
               const VersorType & probeversor, const VectorType & probetranslation )
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::InternalReconstruct called...\n" );

  // Reconstruct the 3D position from calibrated ultrasound probe
  // Ptip = Tref * Tcal * Pus

  unsigned int i;
  VnlMatrixType Tref, Tcal;
  VnlVectorType Ptip, Pus(4);
  PointType position;

  Tref = this->InternalExportTransformMatrix( probeversor, probetranslation );
  Tcal = this->InternalExportTransformMatrix( this->m_CalibrationTransform, this->m_ScaleTransform );
  for (i = 0; i < 3; i++)
    {
    Pus[i] = imageposition[i];
    }
  Pus[3] = 1.0;

  Ptip = Tref * Tcal * Pus;
  for ( i = 0; i < 3; i++)
    {
    position[i] = Ptip[i];
    }

  return position;

}

/** Internal function to export the 4x4 matrix from rotation and translation */
PointerUltrasoundCalibration::VnlMatrixType 
PointerUltrasoundCalibration::InternalExportTransformMatrix( const VersorType & versor, const VectorType & translation )
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::InternalExportTransformMatrix called...\n" );

  unsigned int i, j;
  VnlMatrixType matrix(4, 4);

  for ( j = 0; j < 3; j++)
    {
    for ( i = 0; i < 3; i++)
      {
      matrix[j][i] = versor.GetMatrix()[j][i];
      }
    matrix[j][3] = translation[j];
    matrix[3][j] = 0.0;
    }
  matrix[3][3] = 1.0;

  return matrix;

}

/** Internal function to export the 4x4 matrix from igstkTransform */
PointerUltrasoundCalibration::VnlMatrixType 
PointerUltrasoundCalibration::InternalExportTransformMatrix( const TransformType & transform, const SpacingType & spacing )
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::InternalExportTransformMatrix called...\n" );

  unsigned int i, j;
  VnlMatrixType matrix;
  
  matrix = this->InternalExportTransformMatrix( transform.GetRotation(), transform.GetTranslation() );
  for ( j = 0; j < 2; j++)
    {
    for ( i = 0; i < 3; i++)
      {
      matrix(i, j) = matrix(i, j) * spacing[j];
      }
    }

  return matrix;

}

/** Internal method to calculate the calibration */
void PointerUltrasoundCalibration::CalculateCalibrationByIndexPositionProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::CalculateCalibrationByIndexPositionProcessing called...\n" );

  this->InternalCalculateCalibrationProcessing( PointerUltrasoundCalibration::INDEX_POSITION_INPUT );

}

/** Internal method to calculate the calibration */
void PointerUltrasoundCalibration::CalculateCalibrationByImagePositionProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::CalculateCalibrationByImagePositionProcessing called...\n" );

  this->InternalCalculateCalibrationProcessing( PointerUltrasoundCalibration::IMAGE_POSITION_INPUT );

}

/** Internal method to calculate the calibration */
void PointerUltrasoundCalibration::InternalCalculateCalibrationProcessing( InputPositionType input )
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::InternalCalculateCalibrationProcessing called...\n" );

  unsigned int i, j, k;
  unsigned int num = this->GetNumberOfSamples();
  double d[3];
  double scale[3];
  double rms = 0;

  VnlMatrixType Tcal(4, 4), M( 4, num), Tus( 4, num);
  VnlMatrixType Tref(4, 4), TrefINV;
  VnlVectorType Ptip(4), PtipINVref;

  vtkPoints* source;
  vtkPoints* target;
  vtkMatrix4x4* matrix;

  MatrixType rotmatrix;
  VersorType versor;
  VectorType translation;
  PointType position;
  VectorType vec[3];

  InputPointContainerPointerType positions = InputPointContainerType::New();

  for ( j = 0; j < num; j++)
    {
    for ( i = 0; i < 3; i++)
      {
      switch ( input )
        {
      case INDEX_POSITION_INPUT:
        Tus( i, j) = this->m_IndexPositionContainer->GetElement(j)[i];
        break;
      case IMAGE_POSITION_INPUT:
        Tus( i, j) = this->m_ImagePositionContainer->GetElement(j)[i];
        break;
        }
      }
    Tus( 3, j) = 1.0;
    }

  for ( k = 0; k < num; k++)
    {
    Tref = this->InternalExportTransformMatrix( this->m_ProbeVersorContainer->GetElement(k), this->m_ProbeTranslationContainer->GetElement(k));
    TrefINV = vnl_matrix_inverse< double >( Tref);

    for ( i = 0; i < 3; i++)
      {
      Ptip(i) = this->m_PointerPositionContainer->GetElement(k)[i];
      }
    Ptip(3) = 1.0;

    PtipINVref = TrefINV * Ptip;

    for ( i = 0; i < 4; i++)
      {
      M( i, k) = PtipINVref[i];
      }
  }

  switch ( input)
    {
  case INDEX_POSITION_INPUT:
    this->m_LandmarkTransform->SetModeToSimilarity();
    break;
  case IMAGE_POSITION_INPUT:
    this->m_LandmarkTransform->SetModeToRigidBody();
    break;
    }

  source = vtkPoints::New();
  target = vtkPoints::New();

  for ( i = 0; i < num; i++)
    {
    source->InsertNextPoint( Tus[0][i], Tus[1][i], Tus[2][i]);
    target->InsertNextPoint( M[0][i], M[1][i], M[2][i]);    
    }

  this->m_LandmarkTransform->SetSourceLandmarks( source);
  this->m_LandmarkTransform->SetTargetLandmarks( target);
  this->m_LandmarkTransform->Update();
  matrix = this->m_LandmarkTransform->GetMatrix();

  switch ( input)
    {
  case INDEX_POSITION_INPUT:
    for ( j = 0; j < 3; j++)
      {
      scale[j] = 0.0;
      for ( i = 0; i < 3; i++)
        {
        scale[j] += matrix->GetElement(j, i) * matrix->GetElement(j, i);
        }
      scale[j] = sqrt(scale[j]);
      this->m_ScaleTransform[j] = scale[j];
      }
    break;
  case IMAGE_POSITION_INPUT:
    this->m_ScaleTransform.Fill( 1.0);
    break;
    }

  for ( j = 0; j < 4; j++)
    {
    for ( i = 0; i < 4; i++)
      {
      this->m_CalibrationMatrix4x4(j, i) = matrix->GetElement(j, i);
      }
    }  

  for (j = 0; j < 3; j++)
    {
    for (i = 0; i < 3; i++)
      {
      rotmatrix[j][i] = matrix->GetElement(j, i) / this->m_ScaleTransform[i];
      }
    translation[j] = matrix->GetElement(j, 3);
    }
  versor.Set( rotmatrix);
  this->m_CalibrationTransform.SetTranslationAndRotation( translation, versor, 0.001, 1000); 

  for ( i = 0; i < num; i++)
    {
    switch ( input )
      {
    case INDEX_POSITION_INPUT:
      position = this->InternalReconstruct( this->m_IndexPositionContainer->GetElement(i), this->m_ProbeVersorContainer->GetElement(i), this->m_ProbeTranslationContainer->GetElement(i));
      break;
    case IMAGE_POSITION_INPUT:
      position = this->InternalReconstruct( this->m_ImagePositionContainer->GetElement(i), this->m_ProbeVersorContainer->GetElement(i), this->m_ProbeTranslationContainer->GetElement(i));
      break;
      }
    positions->push_back( position);
    }

  for ( j = 0; j < num; j++)
    {
    for ( i = 0; i < 3; i++)
      {
      d[i] = positions->GetElement(j)[i] - this->m_PointerPositionContainer->GetElement(j)[i];
      rms += d[i] * d[i];
      }
    }

  this->m_RootMeanSquareError = sqrt(rms / num);

  this->m_ValidPointerUltrasoundCalibration = true;

  source->Delete();
  target->Delete();

}

/** Set the calibration matrix of pointer tools */
void PointerUltrasoundCalibration
::SetPointerToolCalibrationTransformProcessing()
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::SetPointerToolCalibrationTransformProcessing called...\n" );

  this->InternalSetPointerToolCalibrationTransformProcessing( this->m_PointerToolTransformToBeSent);
}

/** Internal function to set the pointer tool calibration matrix */
void PointerUltrasoundCalibration
::InternalSetPointerToolCalibrationTransformProcessing( const TransformType & transform )
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::InternalSetPointerToolCalibrationTransformProcessing called...\n" );

  // Set the pointer tool's calibration transform
  this->m_PointerToolCalibrationTransform = transform;

}

/** Method to invoke the ResetProcessing function */
void PointerUltrasoundCalibration::RequestReset()
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::RequestReset called...\n" );

  this->m_StateMachine.PushInput( this->m_ResetCalibrationInput );
  this->m_StateMachine.ProcessInputs();
}

/** Method invoked by the user to set the pointer's calibration matrix */
void PointerUltrasoundCalibration::
RequestSetPointerToolCalibrationTransform( const TransformType & transform )
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::RequestSetPointerToolCalibrationTransform called...\n" );

  this->m_PointerToolTransformToBeSent = transform;
  this->m_StateMachine.PushInput( this->m_PointerToolCalibrationTransformInput );
  this->m_StateMachine.ProcessInputs();
}

/** Method to invoke adding the sample */
void PointerUltrasoundCalibration
::RequestAddIndexPositionSample( const IndexType & indexposition,
                    const PointType & pointerposition, 
                    const VersorType & probeversor, const VectorType & probetranslation )
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::RequestAddIndexPositionSample called...\n" );
  
  this->m_IndexPositionToBeSent = indexposition;
  this->m_PointerPositionToBeSent = pointerposition;
  this->m_ProbeVersorToBeSent = probeversor;
  this->m_ProbeTranslationToBeSent = probetranslation;

  this->m_StateMachine.PushInput( this->m_IndexPositionSampleInput );
  this->m_StateMachine.ProcessInputs();
}

/** Method to invoke adding the sample */
void PointerUltrasoundCalibration
::RequestAddImagePositionSample( const PointType & imageposition,
                    const PointType & pointerposition, 
                    const VersorType & probeversor, const VectorType & probetranslation )
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::RequestAddImagePositionSample called...\n" );
  
  this->m_ImagePositionToBeSent = imageposition;
  this->m_PointerPositionToBeSent = pointerposition;
  this->m_ProbeVersorToBeSent = probeversor;
  this->m_ProbeTranslationToBeSent = probetranslation;

  this->m_StateMachine.PushInput( this->m_ImagePositionSampleInput );
  this->m_StateMachine.ProcessInputs();
}

/** Method to invoke adding the sample */
void PointerUltrasoundCalibration
::RequestAddPointerToolIndexPositionSample( const IndexType & indexposition,
                    const VersorType & pointerversor, const VectorType & pointertranslation, 
                    const VersorType & probeversor, const VectorType & probetranslation )
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::RequestAddPointerToolIndexPositionSample called...\n" );
  
  this->m_IndexPositionToBeSent = indexposition;
  this->m_PointerVersorToBeSent = pointerversor;
  this->m_PointerTranslationToBeSent = pointertranslation;
  this->m_ProbeVersorToBeSent = probeversor;
  this->m_ProbeTranslationToBeSent = probetranslation;

  this->m_StateMachine.PushInput( this->m_PointerToolIndexPositionSampleInput );
  this->m_StateMachine.ProcessInputs();
}

/** Method to invoke adding the sample */
void PointerUltrasoundCalibration
::RequestAddPointerToolImagePositionSample( const PointType & imageposition,
                    const VersorType & pointerversor, const VectorType & pointertranslation, 
                    const VersorType & probeversor, const VectorType & probetranslation )
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::RequestAddPointerToolImagePositionSample called...\n" );
  
  this->m_ImagePositionToBeSent = imageposition;
  this->m_PointerVersorToBeSent = pointerversor;
  this->m_PointerTranslationToBeSent = pointertranslation;
  this->m_ProbeVersorToBeSent = probeversor;
  this->m_ProbeTranslationToBeSent = probetranslation;

  this->m_StateMachine.PushInput( this->m_PointerToolImagePositionSampleInput );
  this->m_StateMachine.ProcessInputs();
}

/** Method to invoke the calculation */
void PointerUltrasoundCalibration::RequestCalculateCalibration()
{
  igstkLogMacro( DEBUG, "igstk::PointerUltrasoundCalibration::RequestCalculateCalibration called...\n" );

  this->m_StateMachine.PushInput( this->m_CalculateCalibrationInput );
  this->m_StateMachine.ProcessInputs();

}


} // end namespace igstk

#endif
