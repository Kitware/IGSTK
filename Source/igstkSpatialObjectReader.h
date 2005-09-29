
#ifndef __igstkSpatialObjectReader_h
#define __igstkSpatialObjectReader_h

#include "igstkMacros.h"

#include "itkObject.h"
#include "itkMacro.h"
#include "itkSpatialObjectReader.h"
#include "itkLogger.h"
#include "igstkStateMachine.h"
#include "igstkStringEvents.h"
#include "itkDefaultDynamicMeshTraits.h"

namespace igstk
{

/** \class SpatialObjectReader
 * 
 * \brief This class reads spatial object data stored in files and provide
 * pointers to the spatial object data for use in an ITK
 * pipeline. This class is templated over the dimension of the object to read
 *
 * \ingroup Object
 */

template <unsigned int TDimension = 3, 
          typename TPixelType = unsigned char>
class SpatialObjectReader : public itk::Object
{

public:
  /** typedef for LoggerType */
  typedef itk::Logger                    LoggerType;

  /** Typedefs */
  typedef SpatialObjectReader                   Self;
  typedef itk::Object                           Superclass;
  typedef itk::SmartPointer<Self>               Pointer;
  typedef itk::SmartPointer<const Self>         ConstPointer;

  itkStaticConstMacro(ObjectDimension, unsigned int, TDimension);

  /** Some convenient typedefs for input Object */
  typedef TPixelType                                       PixelType;
  typedef itk::DefaultDynamicMeshTraits< TPixelType , TDimension, TDimension> MeshTraitsType;
  typedef itk::SpatialObjectReader<TDimension,TPixelType,MeshTraitsType>  
                                                           SpatialObjectReaderType;
  typedef itk::SpatialObject<TDimension>                   SpatialObjectType;
  typedef itk::SpatialObject<TDimension>                   GroupSpatialObjectType;

  /**  Run-time type information (and related methods). */
  igstkTypeMacro( SpatialObjectReader, itk::Object );
  /** Method for creation of a reference counted object. */
  igstkNewMacro( Self );


  /** Method to pass the directory name containing the spatial object data */
  void RequestSetFileName( const char *filename );

  /** This method request Object read **/
  void RequestReadObject();

  /** Returns true if the reader can open the file and the header is valid */
  bool CheckFileIsValid();
  
  /** Declarations needed for the State Machine */
  igstkStateMachineTemplatedMacro();

  /** Declarations needed for the Logger */
  igstkLoggerMacro();

protected:

  SpatialObjectReader( void );
  ~SpatialObjectReader( void );

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, itk::Indent indent ) const;

  /* Internal itkObjectFileReader */
  typename SpatialObjectReaderType::Pointer  m_SpatialObjectReader;
  std::string m_FileNameToBeSet;
  std::string m_FileName;

  itkEventMacro( ObjectReaderEvent,             IGSTKEvent);
  itkEventMacro( ObjectReadingErrorEvent,       ObjectReaderEvent );
  itkEventMacro( ObjectInvalidRequestErrorEvent,ObjectReaderEvent );

  virtual void AttemptReadObject();

private:
 
  /** List of States */
  StateType                    m_IdleState;
  StateType                    m_ObjectFileNameReadState;
  StateType                    m_ObjectReadState;

  /** List of State Inputs */
  InputType                    m_ReadObjectRequestInput;
  InputType                    m_ObjectFileNameValidInput; 

  /** Error related state inputs */
  InputType                    m_ObjectReadingErrorInput;

  void SetFileName();

  void ReportInvalidRequest();

  /** This function reports an error while image reading */
  void ReportObjectReadingError();

};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkSpatialObjectReader.txx"
#endif

#endif // __igstkSpatialObjectReader_h
