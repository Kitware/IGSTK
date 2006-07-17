/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    FiducialSegmentationTest.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
//Warning about: identifier was truncated to '255' characters 
//in the debug information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif


#include "itkImageFileWriter.h"
#include "FiducialSegmentation.h"
#include "igstkCTImageReader.h"
#include "igstkMacros.h"
#include "igstkCTImageSpatialObject.h"

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

typedef igstk::CTImageReader                      ImageReaderType;
typedef igstk::CTImageSpatialObject               ImageSpatialObjectType;
typedef ImageSpatialObjectType::ImageType         ITKImageType;

igstkObserverObjectMacro( ImageSpatialObject,
                    ImageReaderType::ImageModifiedEvent,ImageSpatialObjectType)
igstkObserverConstObjectMacro( ITKImage,
                   ImageSpatialObjectType::ITKImageModifiedEvent, ITKImageType)

int main(int argc , char * argv [] )
{
  typedef itk::Logger                    LoggerType; 
  typedef itk::StdStreamLogOutput        LogOutputType;
  

  LoggerType::Pointer      logger = LoggerType::New();
  LogOutputType::Pointer   logOutput = LogOutputType::New();  

  logOutput->SetStream( std::cout );
  logger->AddLogOutput( logOutput );
  logger->SetPriorityLevel( itk::Logger::DEBUG );
  
  igstk::FiducialSegmentation::Pointer segmenter = 
                              igstk::FiducialSegmentation::New();

  if( argc < 3 ) 
    {
    std::cerr << "Usage: " << argv[0] << " CT Image"
              << " Segmented image " << std::endl;
    return EXIT_FAILURE;
    }  

  typedef igstk::CTImageReader         ReaderType;
  ReaderType::Pointer   reader = ReaderType::New();

  reader->SetLogger( logger );

  ImageSpatialObjectType::Pointer imageSpatialObject =
                                       ImageSpatialObjectType::New();
  
  std::cout << "Reading CT image : " << argv[1] << std::endl;

  ReaderType::DirectoryNameType directoryName = argv[1];

  reader->RequestSetDirectory( directoryName );  // Set the DICOM directory
 
  reader->RequestReadImage();
  
  ImageSpatialObjectObserver::Pointer imageSpatialObjectObserver 
                                           = ImageSpatialObjectObserver::New();

  reader->AddObserver( ImageReaderType::ImageModifiedEvent(), 
                                             imageSpatialObjectObserver );
 
  reader->RequestGetImage(); // Request to send the image as an event.

  if(!imageSpatialObjectObserver->GotImageSpatialObject())
    {
    std::cerr << "Cannot read image " << std::endl;
    return EXIT_FAILURE;
    }
  else
    {
    imageSpatialObject = 
           imageSpatialObjectObserver->GetImageSpatialObject();
    }
 
  ITKImageObserver::Pointer itkImageObserver = ITKImageObserver::New();
  
  imageSpatialObject->AddObserver( 
      ImageSpatialObjectType::ITKImageModifiedEvent(), itkImageObserver );

  imageSpatialObject->RequestGetITKImage();
  
  segmenter->SetITKImage( itkImageObserver->GetITKImage() );
  segmenter->SetThreshold( 3000 );
  segmenter->SetMaxSize( 20 );
  segmenter->SetMinSize( 0 );
  segmenter->SetMergeDistance( 1 );
  segmenter->Execute();

  typedef itk::Image < int, 3 >                     OutputImageType;
  typedef itk::ImageFileWriter< OutputImageType >   ImageFileWriterType;

  ImageFileWriterType::Pointer   writer = ImageFileWriterType::New();

  writer->SetFileName( argv[2] );
  writer->SetInput( segmenter->GetSegmentedImage() ); 
  writer->Update();  

  return EXIT_SUCCESS;
}
