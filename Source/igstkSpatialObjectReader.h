/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSpatialObjectReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkSpatialObjectReader_h
#define __igstkSpatialObjectReader_h

#include "igstkMacros.h"

#include "itkSpatialObjectReader.h"
#include "igstkObject.h"
#include "igstkStateMachine.h"
#include "igstkEvents.h"
#include "itkDefaultDynamicMeshTraits.h"

namespace igstk
{

/** \class SpatialObjectReader
 * 
 * \brief This class reads spatial object data stored in files.
 * 
 * This class reads spatial object data stored in files and provide pointers to
 * the spatial object data for use in an ITK pipeline. This class is template
 * over the dimension of the object to read
 *
 * \image html  igstkSpatialObjectReader.png 
 *              "Spatial Object Reader State Machine Diagram"
 *
 * \image latex igstkSpatialObjectReader.eps 
 *              "Spatial Object Reader State Machine Diagram"
 
 * \ingroup Readers
 */

template <unsigned int TDimension = 3, 
          typename TPixelType = unsigned char>
class SpatialObjectReader : public Object
{

public:
  
  /** Macro with standard traits declarations. */
  igstkStandardTemplatedClassTraitsMacro( SpatialObjectReader, Object )

public:

  itkStaticConstMacro(ObjectDimension, unsigned int, TDimension);

  /** Some convenient typedefs for input Object */
  typedef TPixelType                                       PixelType;
  typedef ::itk::DefaultDynamicMeshTraits< TPixelType , 
                                           TDimension, 
                                           TDimension> MeshTraitsType;
  
  typedef ::itk::SpatialObjectReader< TDimension, 
                                      TPixelType, 
                                      MeshTraitsType >  SpatialObjectReaderType;

  typedef ::itk::SpatialObject<TDimension>              SpatialObjectType;
  typedef ::itk::GroupSpatialObject<TDimension>         GroupSpatialObjectType;

  /** Type for representing the string of the filename. */
  typedef std::string    FileNameType;

  /** Method to pass the directory name containing the spatial object data */
  void RequestSetFileName( const FileNameType & filename );

  /** This method request Object read */
  void RequestReadObject();

  /** This method will invoke a event from derived reader class, such as:
   *  MeshModifiedEvent for igstkMeshReader
   *  TubeModifiedEvent for igstkTubeReader
   */
  void RequestGetOutput();

  igstkEventMacro( ObjectReaderEvent,              IGSTKEvent        );
  igstkEventMacro( ObjectReadingErrorEvent,        IGSTKErrorEvent   );
  igstkEventMacro( ObjectReadingSuccessEvent,      ObjectReaderEvent );
  igstkEventMacro( ObjectInvalidRequestErrorEvent, ObjectReadingErrorEvent );

protected:

  SpatialObjectReader( void );
  virtual ~SpatialObjectReader( void );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, ::itk::Indent indent ) const;

  /* Internal itkObjectFileReader */
  typename SpatialObjectReaderType::Pointer  m_SpatialObjectReader;
  std::string                                m_FileNameToBeSet;
  std::string                                m_FileName;

  virtual void AttemptReadObjectProcessing();

  virtual void ReportObjectProcessing();

private:

  SpatialObjectReader(const Self&);   //purposely not implemented
  void operator=(const Self&);        //purposely not implemented

  /** List of States */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( ObjectFileNameRead );
  igstkDeclareStateMacro( ObjectRead );
  igstkDeclareStateMacro( ObjectAttemptingRead );

  /** List of State Inputs */
  igstkDeclareInputMacro( ReadObjectRequest );
  igstkDeclareInputMacro( ObjectFileNameValid ); 
  igstkDeclareInputMacro( ObjectFileNameIsEmpty ); 
  igstkDeclareInputMacro( ObjectFileNameIsDirectory ); 
  igstkDeclareInputMacro( ObjectFileNameDoesNotExist ); 
  igstkDeclareInputMacro( GetOutput ); 

  /** Error related state inputs */
  igstkDeclareInputMacro( ObjectReadingError );
  igstkDeclareInputMacro( ObjectReadingSuccess );

  /** Method to be invoked only by the StateMachine. Accepts the filename */
  void SetFileNameProcessing();

  /** Method to be invoked only by the StateMachine. Reports that an input was
   * received during a State where that input is not a valid request. For
   * example, asking to read the file before providing the filename */
  void ReportInvalidRequestProcessing();

  /** Method to be invoked only by the StateMachine. This function reports an
   * error while reading */
  void ReportObjectReadingErrorProcessing();

  /** Method to be invoked only by the StateMachine. This function reports the
   * success of the reading process. */
  void ReportObjectReadingSuccessProcessing();


};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkSpatialObjectReader.txx"
#endif

#endif // __igstkSpatialObjectReader_h
