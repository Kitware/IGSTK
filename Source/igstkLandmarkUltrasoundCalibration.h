/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkLandmarkUltrasoundCalibration.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkLandmarkUltrasoundCalibration_h
#define __igstkLandmarkUltrasoundCalibration_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "vnl/algo/vnl_svd.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"

#include "itkImage.h"
#include "itkIndex.h"
#include "itkVectorContainer.h"

#include "vtkLandmarkTransform.h"

#include "igstkObject.h"
#include "igstkStateMachine.h"
#include "igstkEvents.h"
#include "igstkMacros.h"

namespace igstk
{

/** \class LandmarkUltrasoundCalibration
 * 
 * \brief Create a calibration transform for tracked ultrasound by
 * pointer-based method.
 * 
 * This class calibrates the tracked ultrasound probe and get the transform 
 * from the indices of the ultrasound imaging plane to the general tracking 
 * coordinate. The result will include the final calibration transform, 
 * the RMS calibration error and also the orthogonality and determinant of the
 * rotation matrix to evaluate the calibration accuracy. Generally, more 
 * samples will give more stable result.
 *
 * \ingroup Calibration
 */

class LandmarkUltrasoundCalibration : public Object
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( LandmarkUltrasoundCalibration, Object );

public:

  /** Typedefs for the internal computation */
  typedef Transform                       TransformType;
  typedef TransformType::VersorType       VersorType;
  typedef TransformType::VectorType       VectorType;
  typedef TransformType::PointType        PointType;
  typedef VersorType::MatrixType          MatrixType;
  typedef itk::Index< 3 >                 IndexType;
  typedef itk::Matrix< double, 4, 4 >     Matrix4x4Type;
  typedef itk::Image<double,3>            ImageType;
  typedef ImageType::SpacingType          SpacingType;
  typedef double                          ErrorType;

public:

  /** Method to check whether a valid calibration is calculated */
  igstkGetMacro( ValidLandmarkUltrasoundCalibration, bool );

  /** Method to get the final calibration transform with rotation 
   *  and translation */
  igstkGetMacro( CalibrationTransform, TransformType );

  /** Method to get the final spacing transform */
  igstkGetMacro( ScaleTransform, SpacingType );

  /** Method to get the final 4x4 calibration matrix with rotation, 
   *  translation and spacing */
  igstkGetMacro( CalibrationMatrix4x4, Matrix4x4Type );

  /** Method to retrieve the RMS calibration error */
  igstkGetMacro( RootMeanSquareError, ErrorType );

  /** Method to get the number of samples used to calibrate */
  unsigned int GetNumberOfSamples() const;

  /** Method invoked by the user to reset the calibration process */
  void RequestReset();

  /** Method invoked by the user to set the pointer's calibration matrix */
  void RequestSetPointerToolCalibrationTransform( const TransformType & 
                                                                transform );

  /** Method invoked by the user to add a new sample */
  void RequestAddIndexPositionSample( const IndexType & indexposition,
                                    const PointType & pointerposition, 
                                    const VersorType & probeversor, 
                                    const VectorType & probetranslation );

  /** Method invoked by the user to add a new sample */
  void RequestAddImagePositionSample( const PointType & imageposition,
                                    const PointType & pointerposition, 
                                    const VersorType & probeversor, 
                                    const VectorType & probetranslation );

  /** Method invoked by the user to add a new sample */
  void RequestAddPointerToolIndexPositionSample( 
                                       const IndexType & indexposition,
                                       const VersorType & pointerversor, 
                                       const VectorType & pointertranslation,
                                       const VersorType & probeversor, 
                                       const VectorType & probetranslation );

  /** Method invoked by the user to add a new sample */
  void RequestAddPointerToolImagePositionSample( 
                                       const PointType & imageposition,
                                       const VersorType & pointerversor, 
                                       const VectorType & pointertranslation,
                                       const VersorType & probeversor, 
                                       const VectorType & probetranslation );

  /** Method invoked by the user to calculate the calibration matrix */
  void RequestCalculateCalibration(); 

protected:

  typedef vnl_matrix<double>                   VnlMatrixType;
  typedef vnl_vector<double>                   VnlVectorType;
  typedef vnl_svd<double>                      VnlSVDType;
  typedef itk::VectorContainer<int,IndexType>  InputIndexContainerType;
  typedef InputIndexContainerType::Pointer     InputIndexContainerPointerType;

  typedef itk::VectorContainer<int,PointType>  InputPointContainerType;
  typedef InputPointContainerType::Pointer     InputPointContainerPointerType;
  typedef itk::VectorContainer<int,VersorType> InputVersorContainerType;
  typedef InputVersorContainerType::Pointer    InputVersorContainerPointerType;

  typedef itk::VectorContainer<int,VectorType> InputVectorContainerType;
  typedef InputVectorContainerType::Pointer    InputVectorContainerPointerType;

  typedef enum {
    INDEX_POSITION_INPUT,
    IMAGE_POSITION_INPUT
    } InputPositionType;

protected:

  /** Constructor */
  LandmarkUltrasoundCalibration();

  /** Destructor */
  virtual ~LandmarkUltrasoundCalibration();

  /** Print the object information in a stream */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /** Null operation for a transition in the State Machine */
  void NoProcessing();

  /** Reset the calibration matrix */
  void ResetProcessing();

  /** Set the calibration matrix of pointer tools */
  void SetPointerToolCalibrationTransformProcessing();

  /** Internal function to set the pointer tool calibration matrix */
  void InternalSetPointerToolCalibrationTransformProcessing( 
                                            const TransformType & transform );

  /** Add a new sample, remove parameters to make it work with state machine 
   *  input  */
  void AddIndexPositionSampleProcessing();

  /** Internal function to add a new sample */
  void InternalAddIndexPositionSampleProcessing( 
                                        const IndexType & indexposition,
                                        const PointType & pointerposition, 
                                        const VersorType & probeversor, 
                                        const VectorType & probetranslation );

  /** Add a new sample, remove parameters to make it work with state 
   *  machine input  */
  void AddImagePositionSampleProcessing();

  /** Internal function to add a new sample */
  void InternalAddImagePositionSampleProcessing( 
                            const PointType & imageposition,
                            const PointType & pointerposition, 
                            const VersorType & probeversor, 
                            const VectorType & probetranslation );

  /** Add a new sample, remove parameters to make it work with state machine 
   *  input  */
  void AddPointerToolIndexPositionSampleProcessing();

  /** Internal function to add a new sample */
  void InternalAddPointerToolIndexPositionSampleProcessing( 
                                        const IndexType & indexposition,
                                        const VersorType & pointerversor, 
                                        const VectorType & pointertranslation, 
                                        const VersorType & probeversor, 
                                        const VectorType & probetranslation );

  /** Add a new sample, remove parameters to make it work with state 
   *  machine input  */
  void AddPointerToolImagePositionSampleProcessing();

  /** Internal function to add a new sample */
  void InternalAddPointerToolImagePositionSampleProcessing( 
                                        const PointType & imageposition,
                                        const VersorType & pointerversor, 
                                        const VectorType & pointertranslation,
                                        const VersorType & probeversor,
                                        const VectorType & probetranslation );

  /** Internal function to transform the pointer tool's input into position */
  PointType InternalTransformPointerToolPoint(const VersorType & versor, 
                                              const VectorType & translation );

  /** Calculate the calibration by index position */
  void CalculateCalibrationByIndexPositionProcessing();

  /** Calculate the calibration by image position */
  void CalculateCalibrationByImagePositionProcessing();

  /** Internal function to calculate the calibration */
  void InternalCalculateCalibrationProcessing( InputPositionType input );

  /** Internal function to reconstruct the position from the index position 
   *  and reference probe */
  PointType InternalReconstruct( const IndexType & indexposition,
                                 const VersorType & probeversor, 
                                 const VectorType & probetranslation );

  /** Internal function to reconstruct the position from the image position 
   *  and reference probe */
  PointType InternalReconstruct( const PointType & imageposition,
                                 const VersorType & probeversor, 
                                 const VectorType & probetranslation );

  /** Internal function to export the 4x4 matrix from rotation 
   *  and translation */
  VnlMatrixType InternalExportTransformMatrix(const VersorType & versor, 
                                              const VectorType & translation );

  /** Internal function to export the 4x4 matrix from igstkTransform and 
   *  scale factor */
  VnlMatrixType InternalExportTransformMatrix( const TransformType & transform,
                                               const SpacingType & spacing );

private:

  /** List of States */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( PointerToolCalibrationSet );
  igstkDeclareStateMacro( IndexPositionSampleAdd );
  igstkDeclareStateMacro( ImagePositionSampleAdd );
  igstkDeclareStateMacro( PointerToolIndexPositionSampleAdd );
  igstkDeclareStateMacro( PointerToolImagePositionSampleAdd );
  igstkDeclareStateMacro( CalibrationCalculated ); 

  /** List of Inputs */
  igstkDeclareInputMacro( ResetCalibration );
  igstkDeclareInputMacro( PointerToolCalibrationTransform );
  igstkDeclareInputMacro( IndexPositionSample );
  igstkDeclareInputMacro( ImagePositionSample );
  igstkDeclareInputMacro( PointerToolIndexPositionSample );
  igstkDeclareInputMacro( PointerToolImagePositionSample );
  igstkDeclareInputMacro( CalculateCalibration );

  /** Temporary input variables for state machine */
  IndexType                         m_IndexPositionToBeSent;

  PointType                         m_ImagePositionToBeSent;

  PointType                         m_PointerPositionToBeSent;

  VersorType                        m_PointerVersorToBeSent;

  VectorType                        m_PointerTranslationToBeSent;

  VersorType                        m_ProbeVersorToBeSent;

  VectorType                        m_ProbeTranslationToBeSent;

  TransformType                     m_PointerToolTransformToBeSent;

  /** Container to save the samples */
  InputIndexContainerPointerType    m_IndexPositionContainer;

  InputPointContainerPointerType    m_ImagePositionContainer;

  InputPointContainerPointerType    m_PointerPositionContainer;

  InputVersorContainerPointerType   m_PointerVersorContainer;
  
  InputVectorContainerPointerType   m_PointerTranslationContainer;

  InputVersorContainerPointerType   m_ProbeVersorContainer;
  
  InputVectorContainerPointerType   m_ProbeTranslationContainer;

  vtkLandmarkTransform*             m_LandmarkTransform;

  /** Variable to save the pointer tool's calibration transform */
  TransformType                     m_PointerToolCalibrationTransform;

  /** Variable to save the calibration transform */
  TransformType                     m_CalibrationTransform;

  /** Variable to save the scale factor */
  SpacingType                       m_ScaleTransform;

  /** Variable to save the 4x4 calibration matrix */
  Matrix4x4Type                     m_CalibrationMatrix4x4;

  /** Variable to indicate whether a valid calibration matrix is computed */
  bool                              m_ValidLandmarkUltrasoundCalibration;

  /** Variable to indicate the RMS error */
  ErrorType                         m_RootMeanSquareError;  

};

}

#endif // _igstkUltrasoundCalibration_h
