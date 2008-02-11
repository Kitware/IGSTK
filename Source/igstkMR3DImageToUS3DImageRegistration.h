/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkMR3DImageToUS3DImageRegistration.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkMR3DImageToUS3DImageRegistration_h
#define __igstkMR3DImageToUS3DImageRegistration_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

#include "igstkObject.h"
#include "igstkStateMachine.h"
#include "igstkEvents.h"
#include "igstkMacros.h"
#include "igstkUSImageObject.h"
#include "igstkMRImageSpatialObject.h"
#include "itkImage.h"
#include "itkIndex.h"
#include "itkVectorContainer.h"

namespace igstk
{

/** \class MR3DImageToUS3DImageRegistration
 * 
 * \brief This class registers a 3D MR image with a 3D Utrasound image.
 *
 * This class registers two 3D images, one from 3D Ultrasund and the other from
 * MR modality.  The interpolator used is linear to perform the registration
 * with a minimum amount of time. The current metric evaluating the match
 * between the two images is a mean square.  The optimizer used in this class
 * is a regular step gradient descent optimizer.
 *
 * \image html  igstkMR3DImageToUS3DImageRegistration.png
 *             "MR to UltraSound Image Registration State Machine Diagram"
 * \image latex igstkMR3DImageToUS3DImageRegistration.eps
 *             "MR to UltraSound Image Registration State Machine Diagram"
 *
 * \ingroup Registration
 */
class MR3DImageToUS3DImageRegistration : public Object
{
public:

  /** Macro with standard traits declarations. */
  igstkStandardClassTraitsMacro( MR3DImageToUS3DImageRegistration, Object );

public:

  /** Typedefs for the internal computation */
  typedef Transform                            TransformType;

public:

  /** Method to check whether a valid calibration is calculated */
  igstkGetMacro( ValidMR3DImageToUS3DImageRegistration, bool );

  /** Method to get the number of samples used to calibrate */
  unsigned int GetNumberOfSamples() const;

  /** Method invoked by the user to reset the calibration process */
  void RequestReset();

  /** Set the moving MR 3D Image */
  void RequestSetMovingMR3D(MRImageSpatialObject* MRImage);

  /** Set the fixed US 3D Image */
  void RequestSetFixedUS3D(USImageObject* MRImage);

  /** Method invoked by the user to start the registration */
  void RequestCalculateRegistration(); 

  /** Request to get the final transformation */
  void RequestGetRegistrationTransform(); 

  /** Request to set the initial transformation */
  igstkSetMacro( InitialTransform, TransformType );
  igstkGetMacro( InitialTransform, TransformType );
 
  /** Declare the ImageReaderToImageSpatialObject class to be a friend 
   *  in order to give it access to the private method GetITKImage(). */
  igstkFriendClassMacro( igstk::USImageObject );

protected:

  typedef TransformType::VersorType       VersorType;
  typedef TransformType::VectorType       VectorType;
  typedef TransformType::PointType        PointType;
  typedef VersorType::MatrixType          MatrixType;
  typedef itk::Index< 3 >                 IndexType;
  typedef itk::Matrix< double, 4, 4 >     Matrix4x4Type;
  typedef itk::Image<double,3>            ImageType;
  typedef ImageType::SpacingType          SpacingType;
  typedef double                          ErrorType;


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

protected:

  /** Constructor */
  MR3DImageToUS3DImageRegistration();

  /** Destructor */
  virtual ~MR3DImageToUS3DImageRegistration();

  /** Print the object information in a stream */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const;


private:
  
  /** Null operation for a transition in the State Machine */
  void NoProcessing();

  /** Reset the registration transform */
  void ResetProcessing();
 
  /** Set the 3D fixed US image */
  void SetFixedUS3DProcessing();

  /** Set the 3D moving MR image */
  void SetMovingMR3DProcessing();
  
  /** Compute the registration transform */
  void CalculateRegistrationProcessing();

  /** Return the final transformation as an event */
  void ReportRegistrationTransformProcessing();

  /** Observers for internal events */
  typedef USImageObject::ImageType             USImageType;
  typedef USImageObject::ITKImageModifiedEvent USITKImageModifiedEvent;

  igstkObserverConstObjectMacro(ITKUSImage,
    USITKImageModifiedEvent,USImageType)

  igstkObserverConstObjectMacro(ITKMRImage,
    MRImageSpatialObject::ITKImageModifiedEvent,MRImageSpatialObject::ImageType)
  
  igstkObserverMacro(USImageTransform,CoordinateSystemTransformToEvent,
                     CoordinateSystemTransformToResult )
  igstkObserverMacro(MRImageTransform,CoordinateSystemTransformToEvent,
                     CoordinateSystemTransformToResult )


private:

  /** List of States */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( MRImageSet );
  igstkDeclareStateMacro( USImageSet );
  igstkDeclareStateMacro( ImagesSet );
  igstkDeclareStateMacro( RegistrationCalculated ); 

  /** List of Inputs */
  igstkDeclareInputMacro( ResetRegistration );
  igstkDeclareInputMacro( ValidMovingMR3D );
  igstkDeclareInputMacro( ValidFixedUS3D );
  igstkDeclareInputMacro( MRImageTransform );
  igstkDeclareInputMacro( USImageTransform  );
  igstkDeclareInputMacro( ValidRegistration );
  igstkDeclareInputMacro( CalculateRegistration );
  igstkDeclareInputMacro( RequestRegistrationTransform );

  
  /** Methods for Converting Events into State Machine Inputs */
  igstkLoadedEventTransductionMacro( CoordinateSystemTransformTo,
                                     MRImageTransform );
  igstkLoadedEventTransductionMacro( CoordinateSystemTransformTo ,
                                     USImageTransform );

  /** Variable to save the calibration transform */
  TransformType                     m_RegistrationTransform;

  /** Variable to indicate whether a valid calibration matrix is computed */
  bool                              m_ValidMR3DImageToUS3DImageRegistration;

  /** Variable to indicate the RMS error */
  ErrorType                         m_RootMeanSquareError;

  USImageObject*           m_USFixedImageToBeSet;
  USImageObject*           m_USFixedImage;
  MRImageSpatialObject*    m_MRMovingImageToBeSet;
  MRImageSpatialObject*    m_MRMovingImage;
  TransformType            m_InitialTransform;

};

}

#endif // _igstkUltrasoundCalibration_h
