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

#include <vector>

#include "vnl/algo/vnl_matrix_inverse.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"

#include "itkObject.h"

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
 * stable result. The prefered samples number is greater than 100.
 *
 *  \image html  igstkPivotCalibration.png  "PivotCalibration State Machine Diagram"
 *  \image latex igstkPivotCalibration.eps  "PivotCalibration State Machine Diagram" 
 *
 * \ingroup Calibration
 */

class PivotCalibration : public itk::Object
{
public:

  /** Typedefs */
  typedef PivotCalibration               Self;
  typedef itk::Object                    Superclass;
  typedef itk::SmartPointer<Self>        Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer; 

  /** Data type for the logger */
  typedef itk::Logger   LoggerType;

  /** Method for defining the name of the class */ 
  igstkTypeMacro(PivotCalibration, Object); 

  /** Method for creation through the object factory */ 
  igstkNewMacro(Self); 

  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

  /** Define the Logger object for this class */
  igstkLoggerMacro();

  /** Typedefs for the internal computation */
  typedef Transform                       TransformType;

  typedef TransformType::VersorType       VersorType;

  typedef TransformType::VectorType       VectorType;

  typedef VersorType::MatrixType          MatrixType;

  typedef vnl_matrix< double >            VnlMatrixType;

  typedef vnl_vector< double >            VnlVectorType;

  typedef std::vector< double >           InputContainerType;

  typedef double                          ErrorType;

  /** Method to check whether a valid calibration is calculated */
  igstkGetMacro( ValidCalibration, bool );

  /** Method to get the final calibration transform */
  igstkGetMacro( CalibrationTransform, TransformType );

  /** Method to get the pivot position used to calibrate */
  igstkGetMacro( PivotPosition, VectorType );

  /** Method to retrieve the RMS calibration error */
  igstkGetMacro( RMS, ErrorType );

  /** Method to get the number of samples used to calibrate */
  int GetNumberOfFrame();

  /** Method invoked by the user to reset the calibration process */
  void RequestReset();

  /** Method invoked by the user to add a new sample */
  void RequestAddRotationTranslation( VersorType quat, VectorType trans );

  /** Method invoked by the user to calculate the calibration matrix */
  void RequestCalculateCalibration();  

protected:

  /** Constructor */
  PivotCalibration();

  /** Destructor */
  virtual ~PivotCalibration();

  /** Print the object information in a stream */
  void PrintSelf( std::ostream& os, itk::Indent indent );

  /** Reset the calibration matrix */
  void Reset();

  /** Add a new sample, remove paramters to make it work with state machine input  */
  void AddRotationTranslation();

  /** Internal function to add a new sample */
  void InternalAddRotationTranslation( VersorType quat, VectorType trans );

  /** Calculate the calibration */
  void CalculateCalibration();

private:

  /** List of States */
  StateType                                m_IdleState;
  StateType                                m_RotationTranslationAddState;
  StateType                                m_CalibrationCalculatedState; 


  /** List of Inputs */
  InputType                                m_ResetCalibrationInput;
  InputType                                m_RotationTranslationInput;
  InputType                                m_CalculateCalibrationInput;

  /** Temporary input variables for state machine */
  VersorType            m_QuaternionToBeAdded;

  VectorType            m_TranslationToBeAdded;

  /** Container to save the samples */

  InputContainerType    m_Quaternion[4];
  
  InputContainerType    m_Translation[3];

  /** Variable to indicate the valid calibration */
  bool                  m_ValidCalibration;

  /** Variable to save the calibration transform */
  TransformType         m_CalibrationTransform;

  /** Variable to save the pivot position */
  VectorType            m_PivotPosition;

  /** Variable to indicate the RMS error */
  ErrorType             m_RMS;

};

}

#endif // _igstkPivotCalibration_h
