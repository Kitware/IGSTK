/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkToolCalibrationReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISIS Georgetown University. All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkToolCalibrationReader_h
#define __igstkToolCalibrationReader_h

#include "igstkMacros.h"
#include "igstkObject.h"
#include "igstkEvents.h"

#include "igstkTransform.h"
#include "itksys/SystemTools.hxx"
#include "itkXMLFile.h"

namespace igstk
{

namespace Friends 
{

/** \class ToolCalibrationReaderToToolCalibration
 * 
 * \brief This class is intended to make the connection between the 
 * ToolCalibrationReader and its output, the ToolCalibration. 
 *
 * With this class it is possible to enforce encapsulation of the Reader and
 * the ToolCalibration, and make the SetTranslationAndRotation() private, so
 * that developers cannot gain access to the ITK or VTK layers of these two
 * classes.
 *
 */
class ToolCalibrationReaderToToolCalibration
{
public:

  template < class TToolCalibrationReader, class TToolCalibration >
  static void 
  ConnectToolCalibration( const TToolCalibrationReader * reader, 
                TToolCalibration * toolCalibration )
    {
    toolCalibration->SetTransform( reader->GetTransform() );  
    }

}; // end of ToolCalibrationReaderToToolCalibration class

} // end of Friend namespace


class XMLToolCalibrationReader : public itk::XMLReaderBase
{

public: 
  
  /** Standard typedefs */ 
  typedef XMLToolCalibrationReader           Self;
  typedef itk::XMLReaderBase                 Superclass;
  typedef itk::SmartPointer<Self>            Pointer;
  typedef std::pair<std::string,std::string> PairType;

  struct tag
    {
    std::string           name;
    std::vector<PairType> values;
    };

  /** Run-time type information (and related methods). */
  itkTypeMacro(XMLToolCalibrationReader, itk::XMLReaderBase);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** determine whether a file can be opened and read */
  int CanReadFile(const char* name)
    {
    try
      {
      this->parse();
      }
    catch( itk::ExceptionObject & excp )
      {
      std::cerr << "Error while reading XML file" << std::endl;
      std::cerr << excp << std::endl;
      return 0;
      }
    return 1;
    }

  /** Callback function -- called from XML parser with start-of-element
   * information. */
  virtual void StartElement(const char * name,const char **atts)
    {
    tag currentTag;
    currentTag.name = name;

    PairType value;
    for(int i = 0; atts[i] != 0; i++)
      {
      if(i%2==0)
        {
        value.first = atts[i];
        }
      else
        {
        value.second = atts[i];
        currentTag.values.push_back(value);
        }
      }
    m_Tags.push_back(currentTag);
    }
  /** Callback function -- called from XML parser when ending tag
   * encountered */
  virtual void EndElement(const char *name)
    {
    }
  /** Callback function -- called from XML parser with the character data
   * for an XML element */
  virtual void CharacterDataHandler(const char *inData, int inLength)
    {
    PairType value;
    for(int i = 0; i < inLength; i++)
      {
      if(inData[i]!= ' ' && inData[i]!= '\n' && inData[i]!= '\r')
        {
        value.first += inData[i];
        }
      else
        {
        value.second = "";
        if(value.first.size()>0)
          {
          m_Tags[m_Tags.size()-1].values.push_back(value);
          }
        value.first = "";
        }   
      }
    if(value.first.size()>0)
      {
      m_Tags[m_Tags.size()-1].values.push_back(value);
      }
    }

  const std::vector<tag> & GetTags() const {return m_Tags;}

protected:

  XMLToolCalibrationReader() 
    {
    }
  ~XMLToolCalibrationReader() {};

  std::vector<tag> m_Tags;

};

  
/** \class ToolCalibrationReader
 * \brief This class reads an XML calibration file and returns a calibration
 *        transform.
 * 
 *  This class is the base class for readers of calibration files. The files
 *  use an XML format and are specialiazed for particular calibration objects.
 *  This class defines the generic interface of a reader.
 *
 * \image html igstkToolCalibrationReader.png 
 *           "Tool Calibration Reader State Machine Diagram"
 * 
 * \image latex igstkToolCalibrationReader.eps
 *           "Tool Calibration Reader State Machine Diagram" 
 *
 * \ingroup Readers */
template <class TCalibration>
class ToolCalibrationReader : public Object
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardTemplatedClassTraitsMacro( ToolCalibrationReader, Object )

  /** Transform typedef */
  typedef TCalibration                            CalibrationType;
  typedef typename CalibrationType::TransformType TransformType;

  /** Type for representing the string of the filename. */
  typedef std::string    FileNameType;
  
  /** Method to pass the directory name containing the spatial object data */
  void RequestSetFileName( const FileNameType & filename );

  /** This method request Object read **/
  void RequestReadObject();

  /** This method request the calibration **/
  void RequestGetCalibration();

  /** Declare the ToolCalibrationReaderToToolCalibration class to be a friend 
   *  in order to give it access to the private method GetTransform(). */
  igstkFriendClassMacro( 
                     igstk::Friends::ToolCalibrationReaderToToolCalibration );

protected:

  ToolCalibrationReader();
  ~ToolCalibrationReader();

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, ::itk::Indent indent ) const;

  bool ParseXML();

  /** Event types */
  itkEventMacro( ObjectReaderEvent,              IGSTKEvent        );
  itkEventMacro( ObjectReadingErrorEvent,        ObjectReaderEvent );
  itkEventMacro( ObjectReadingSuccessEvent,      ObjectReaderEvent );
  itkEventMacro( ObjectInvalidRequestErrorEvent, ObjectReaderEvent );

  /** Type for the name of the parameters */
  typedef std::string  ParameterNameType;

  /** Add a parameter and its value */
  bool AddParameter( const ParameterNameType & name, float value);

  /** Add an error and its value */
  bool AddError( const ParameterNameType & name, float value);

  /** Get a parameter value given its name */
  bool GetParameter( const ParameterNameType & name, float & param );

  /** Get an error value given its name */
  bool GetError( const ParameterNameType & name, float & error );

  /** This transform is used to pass it to the calibration */
  const TransformType & GetTransform() const;

  /** Return the Calibration */
  CalibrationType * GetCalibration();
  const CalibrationType * GetCalibration() const;

  /** Service function to be overloaded by derived classes */
  virtual bool RetrieveParametersFromFile( TransformType & transform );

  /** Method to be invoked only by the StateMachine. This function reports the
   * success of the reading process. */
  void ReportObjectReadingSuccessProcessing();

private:
  
  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  ToolCalibrationReader(const Self&);         //purposely not implemented
  void operator=(const Self&);      //purposely not implemented

  /** Convert a buffer to windows file type */
  void ConvertBufferToWindowsFileType(std::string & buffer);

  /** Method to be invoked only by the StateMachine. Reports that an input was
   * received during a State where that input is not a valid request. For
   * example, asking to read the file before providing the filename */
  void ReportInvalidRequestProcessing();

  /** Method to be invoked only by the StateMachine. This function reports an
   * error while reading */
  void ReportObjectReadingErrorProcessing();

  /** Return the calibration via event */
  virtual void ReportCalibrationProcessing();

  /** Attempt to read the calibration file */
  virtual void AttemptReadObjectProcessing();


  /** Method to be invoked only by the StateMachine. Accepts the filename */
  void SetFileNameProcessing();

  /** List of States */
  igstkDeclareStateMacro( Idle );
  igstkDeclareStateMacro( ObjectFileNameRead );
  igstkDeclareStateMacro( ObjectRead );
  igstkDeclareStateMacro( ObjectAttemptingRead );
  igstkDeclareInputMacro( ObjectReadingError );
  igstkDeclareInputMacro( ObjectReadingSuccess );

  /** List of State Inputs */
  igstkDeclareInputMacro( ObjectFileNameValid ); 
  igstkDeclareInputMacro( ObjectFileNameIsEmpty ); 
  igstkDeclareInputMacro( ObjectFileNameIsDirectory ); 
  igstkDeclareInputMacro( ObjectFileNameDoesNotExist ); 
  igstkDeclareInputMacro( RequestCalibration ); 
  igstkDeclareInputMacro( ReadObjectRequest );

  /** Name of the file to read */
  FileNameType                       m_FileName;
  FileNameType                       m_FileNameToBeSet;

  /** Variable storing the callibration */
  typename CalibrationType::Pointer  m_Calibration;

  /** Variable storing the Transform */
  TransformType                   m_Transform;

  /** Type for array of parameters */
  typedef std::pair<ParameterNameType,float> ParameterType;

  /** Type for array of errors */
  typedef std::pair<ParameterNameType,float> ErrorType;

  /** Calibration parameters */
  std::vector< ParameterType >    m_Parameters;

  /** Array of Errors */
  std::vector< ErrorType >        m_Errors;
};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkToolCalibrationReader.txx"
#endif

#endif // __igstkToolCalibrationReader_h
