/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkSpatialObjectReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more DEBUGrmation.

=========================================================================*/

#ifndef __igstkSpatialObjectReader_h
#define __igstkSpatialObjectReader_h

#include "igstkMacros.h"

#include "itkObject.h"
#include "itkSpatialObjectReader.h"
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
 * the spatial object data for use in an ITK pipeline. This class is templated
 * over the dimension of the object to read
 *
 * \ingroup Readers
 */

template <unsigned int TDimension = 3, 
          typename TPixelType = unsigned char>
class SpatialObjectReader : public ::itk::Object
{

public:
  
  /** Macro with standard traits declarations. */
  igstkStandardTemplatedClassTraitsMacro( SpatialObjectReader, ::itk::Object );

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

  /** This method request Object read **/
  void RequestReadObject();

protected:

  SpatialObjectReader( void );
  virtual ~SpatialObjectReader( void );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, ::itk::Indent indent ) const;

  /* Internal itkObjectFileReader */
  typename SpatialObjectReaderType::Pointer  m_SpatialObjectReader;
  std::string m_FileNameToBeSet;
  std::string m_FileName;

  itkEventMacro( ObjectReaderEvent,              IGSTKEvent        );
  itkEventMacro( ObjectReadingErrorEvent,        ObjectReaderEvent );
  itkEventMacro( ObjectReadingSuccessEvent,      ObjectReaderEvent );
  itkEventMacro( ObjectInvalidRequestErrorEvent, ObjectReaderEvent );

  virtual void AttemptReadObject();

private:

  SpatialObjectReader(const Self&);   //purposely not implemented
  void operator=(const Self&);        //purposely not implemented

  /** List of States */
  StateType                    m_IdleState;
  StateType                    m_ObjectFileNameReadState;
  StateType                    m_ObjectReadState;
  StateType                    m_ObjectAttemptingReadState;

  /** List of State Inputs */
  InputType                    m_ReadObjectRequestInput;
  InputType                    m_ObjectFileNameValidInput; 
  InputType                    m_ObjectFileNameIsEmptyInput; 
  InputType                    m_ObjectFileNameIsDirectoryInput; 
  InputType                    m_ObjectFileNameDoesNotExistInput; 

  /** Error related state inputs */
  InputType                    m_ObjectReadingErrorInput;
  InputType                    m_ObjectReadingSuccessInput;

  /** Method to be invoked only by the StateMachine. Accepts the filename */
  void SetFileName();

  /** Method to be invoked only by the StateMachine. Reports that an input was
   * received during a State where that input is not a valid request. For
   * example, asking to read the file before providing the filename */
  void ReportInvalidRequest();

  /** Method to be invoked only by the StateMachine. This function reports an
   * error while reading */
  void ReportObjectReadingError();

  /** Method to be invoked only by the StateMachine. This function reports the
   * success of the reading process. */
  void ReportObjectReadingSuccess();


};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkSpatialObjectReader.txx"
#endif

#endif // __igstkSpatialObjectReader_h
