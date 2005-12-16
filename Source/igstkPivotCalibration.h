/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPivotCalibration.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef _igstkPivotCalibration_h
#define _igstkPivotCalibration_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "vnl/algo/vnl_svd.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"

#include "itkCovariantVector.h"
#include "itkVectorContainer.h"

#include "igstkObject.h"
#include "igstkStateMachine.h"
#include "igstkEvents.h"
#include "igstkMacros.h"

namespace igstk
{

/** \class PivotCalibration
 * 
 * \brief Create a calibration transform for tracker tools.
 * 
 * This class calibrates the tracker tools and get the transform
 * from the tracked sensor or marker to the pivot point of the surgical
 * tool. The result will include the transform, the pivot position and
 * also the RMS error of the calibration, which is used to evaluate
 * the calibration accuracy. Generally, more samples will give more
 * stable result. 
 *
 *  \image html  igstkPivotCalibration.png  "PivotCalibration State Machine Diagram"
 *  \image latex igstkPivotCalibration.eps  "PivotCalibration State Machine Diagram" 
 *
 * \ingroup Calibration
 */

class PivotCalibration : public Object
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( PivotCalibration, Object );

public:

  /** Typedefs for the internal computation */
  typedef Transform                       TransformType;

  typedef TransformType::VersorType       VersorType;

  typedef TransformType::VectorType       VectorType;

  typedef TransformType::PointType        PointType;

  typedef VersorType::MatrixType          MatrixType;

  typedef double                          ErrorType;

private:

  typedef vnl_matrix< double >            VnlMatrixType;

  typedef vnl_vector< double >            VnlVectorType;

  typedef vnl_svd< double >               VnlSVDType;

  typedef itk::VectorContainer< int, VersorType >
                                          InputVersorContainerType;

  typedef InputVersorContainerType::Pointer
                                          InputVersorContainerPointerType;

  typedef itk::VectorContainer< int, VectorType >
                                          InputVectorContainerType;

  typedef InputVectorContainerType::Pointer
                                          InputVectorContainerPointerType;

  typedef itk::CovariantVector< double >  CovariantVectorType;        

public:

  /** Method to check whether a valid calibration is calculated */
  igstkGetMacro( ValidPivotCalibration, bool );

  /** Method to get the final calibration transform */
  igstkGetMacro( CalibrationTransform, TransformType );

  /** Method to get the pivot position used to calibrate */
  igstkGetMacro( PivotPosition, VectorType );

  /** Method to retrieve the RMS calibration error */
  igstkGetMacro( RMS, ErrorType );

  /** Method to get the number of samples used to calibrate */
  unsigned int GetNumberOfSamples() const;

  /** Method invoked by the user to reset the calibration process */
  void RequestReset();

  /** Method invoked by the user to add a new sample */
  void RequestAddSample( const VersorType & versor, 
                         const VectorType & translation );

  /** Method invoked by the user to calculate the calibration matrix */
  void RequestCalculateCalibration(); 

  /** Method invoked by the user to calculate the calibration matrix along z-axis */
  void RequestCalculateCalibrationZ(); 

  /** Method invoked by the user to calculate simulated pivot position of any input */
  PointType RequestSimulatePivotPosition( const VersorType & versor, 
                                          const VectorType & translation );

  /** Method invoked by the user to get the rotation and translation in the input container */
  bool RequestGetInputSample( unsigned int index, 
                              VersorType & versor, 
                              VectorType & translation );

protected:

  /** Constructor */
  PivotCalibration();

  /** Destructor */
  virtual ~PivotCalibration();

  /** Print the object information in a stream */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** Null operation for a transition in the State Machine */
  void NoAction();

  /** Reset the calibration matrix */
  void Reset();

  /** Add a new sample, remove parameters to make it work with state machine input  */
  void AddSample();

  /** Internal function to add a new sample */
  void InternalAddSample( const VersorType & versor, 
                                       const VectorType & translation );

  /** Calculate the calibration */
  void CalculateCalibration();

  /** Calculate the calibration along z-axis */
  void CalculateCalibrationZ();

  /** Internal function to calculate the calibration */
  void InternalCalculateCalibration( unsigned int axis );

  /** Calculate the simulated pivot position */
  void SimulatePivotPosition();

  /** Internal function to calculate the simulated pivot position */
  PointType InternalSimulatePivotPosition( const VersorType & versor, 
                                           const VectorType & translation );

  /** Get the rotation and translation inputed */
  void GetInputSample();

  /** Internal function to get the rotation and translation inputed */
  bool InternalGetInputSample( unsigned int index, 
                               VersorType & versor, 
                               VectorType & translation );

private:

  /** List of States */
  StateType                         m_IdleState;
  StateType                         m_SampleAddState;
  StateType                         m_CalibrationCalculatedState; 
  StateType                         m_CalibrationZCalculatedState; 

  /** List of Inputs */
  InputType                         m_ResetCalibrationInput;
  InputType                         m_SampleInput;
  InputType                         m_CalculateCalibrationInput;
  InputType                         m_CalculateCalibrationZInput;
  InputType                         m_SimulatePivotPositionInput;
  InputType                         m_GetInputSampleInput;

  /** Temporary input variables for state machine */
  VersorType                        m_VersorToBeSent;

  VectorType                        m_TranslationToBeSent;

  int                               m_InputIndexToBeSent;

  VectorType                        m_VectorToBeSent;

  MatrixType                        m_MatrixToBeSent;

  PointType                         m_SimulatedPivotPositionToBeReceived;

  VersorType                        m_VersorToBeReceived;

  VectorType                        m_TranslationToBeReceived;

  int                               m_NumberOfSamplesToBeReceived;

  /** Container to save the samples */
  InputVersorContainerPointerType   m_VersorContainer;
  
  InputVectorContainerPointerType   m_TranslationContainer;

  /** Variable to indicate the valid calibration */
  bool                              m_ValidPivotCalibration;

  /** Variable to indicate the valid input sample */
  bool                              m_ValidInputSample;

  /** Variable to save the calibration transform */
  TransformType                     m_CalibrationTransform;

  /** Variable to save the pivot position */
  VectorType                        m_PivotPosition;

  /** Variable to indicate the RMS error */
  ErrorType                         m_RMS;

};

}

#endif // _igstkPivotCalibration_h
