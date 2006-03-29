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

#ifndef __igstkPivotCalibration_h
#define __igstkPivotCalibration_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "itkCovariantVector.h"
#include "itkVectorContainer.h"

#include "igstkToolCalibration.h"
#include "igstkStateMachine.h"
#include "igstkMacros.h"

namespace igstk
{

/** \class PivotCalibration
 * 
 * \brief Create a calibration transform for tracker tools.
 * 
 * This class calibrates the tracker tools and get the transform from the
 * tracked sensor or marker to the pivot point of the surgical tool. The result
 * will include the transform, the pivot position and also the root mean square
 * error (RMSE) of the calibration, which is used to evaluate the calibration
 * accuracy. Generally, more samples will give more stable result. 
 *
 *  \image html  igstkPivotCalibration.png  
 *               "PivotCalibration State Machine Diagram"
 *  \image latex igstkPivotCalibration.eps  
 *               "PivotCalibration State Machine Diagram" 
 *
 * \ingroup Calibration
 */

class PivotCalibration : public ToolCalibration
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( PivotCalibration, ToolCalibration );

public:

  /** Typedefs for the internal computation */
  typedef Superclass::TransformType       TransformType;
  typedef TransformType::VersorType       VersorType;
  typedef TransformType::VectorType       VectorType;
  typedef TransformType::PointType        PointType;
  typedef VersorType::MatrixType          MatrixType;
  typedef double                          ErrorType;

private:

  typedef itk::VectorContainer< int, VersorType > InputVersorContainerType;

  typedef InputVersorContainerType::Pointer InputVersorContainerPointerType;

  typedef itk::VectorContainer< int, VectorType > InputVectorContainerType;

  typedef InputVectorContainerType::Pointer InputVectorContainerPointerType;

  typedef itk::CovariantVector< double >  CovariantVectorType;

public:

  /** Method to check whether a valid calibration is calculated */
  igstkGetMacro( ValidPivotCalibration, bool );

  /** Method to get the pivot position used to calibrate */
  igstkGetMacro( PivotPosition, PointType );

  /** Method to retrieve the RootMeanSquareError (RMS) calibration error */
  igstkGetMacro( RootMeanSquareError, ErrorType );

  /** Method to get the number of samples used to calibrate */
  unsigned int GetNumberOfSamples() const;

  /** Method invoked by the user to reset the calibration process */
  void RequestReset();

  /** Method invoked by the user to add a new sample */
  void RequestAddSample( const VersorType & versor, 
                         const VectorType & translation );

  /** Method invoked by the user to calculate the calibration matrix */
  void RequestCalculateCalibration(); 

  /** Method invoked by the user to calculate the calibration matrix along 
   *  z-axis */
  void RequestCalculateCalibrationZ(); 

  /** Method invoked by the user to calculate simulated pivot position of 
   *  any input */
  PointType RequestSimulatePivotPosition( const VersorType & versor, 
                                          const VectorType & translation );

  /** Method invoked by the user to get the rotation and translation in 
   *  the input container */
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
  void NoProcessing();

  /** Reset the calibration matrix */
  void ResetProcessing();

  /** Add a new sample, remove parameters to make it work with state machine 
   *  input */
  void AddSampleProcessing();

  /** Internal function to add a new sample */
  void InternalAddSampleProcessing( const VersorType & versor, 
                                    const VectorType & translation );

  /** Calculate the calibration */
  void CalculateCalibrationProcessing();

  /** Calculate the calibration along z-axis */
  void CalculateCalibrationZProcessing();

  /** Internal function to calculate the calibration */
  void InternalCalculateCalibrationProcessing( unsigned int axis );

  /** Calculate the simulated pivot position */
  void SimulatePivotPositionProcessing();

  /** Internal function to calculate the simulated pivot position */
  PointType InternalSimulatePivotPositionProcessing(const VersorType & versor,
                                             const VectorType & translation );

  /** Get the rotation and translation inputed */
  void GetInputSampleProcessing();

  /** Internal function to get the rotation and translation inputed */
  bool InternalGetInputSampleProcessing( unsigned int index, 
                                         VersorType & versor, 
                                         VectorType & translation );

private:

  PivotCalibration( const Self & ); //purposely not implemented
  void operator=(const Self&);  //purposely not implemented
  
  /** List of States */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( SampleAdd );
  igstkDeclareStateMacro( CalibrationCalculated ); 
  igstkDeclareStateMacro( CalibrationZCalculated ); 

  /** List of Inputs */
  igstkDeclareInputMacro( ResetCalibration );
  igstkDeclareInputMacro( Sample );
  igstkDeclareInputMacro( CalculateCalibration );
  igstkDeclareInputMacro( CalculateCalibrationZ );
  igstkDeclareInputMacro( SimulatePivotPosition );
  igstkDeclareInputMacro( GetInputSample );

  /** Temporary input variables for state machine */
  VersorType                        m_VersorToBeSent;
  VectorType                        m_TranslationToBeSent;
  int                               m_InputIndexToBeSent;
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

  /** Variable to save the pivot position */
  PointType                         m_PivotPosition;

  /** Variable to indicate the RootMeanSquareError error */
  ErrorType                         m_RootMeanSquareError;

};

}

#endif // _igstkPivotCalibration_h
