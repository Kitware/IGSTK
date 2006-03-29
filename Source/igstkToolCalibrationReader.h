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
    std::string name;
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
   * for an XML element
   */
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
 * \brief This class read an XML calibration file and returns a calibration
 *        transform.
 * 
 *
 * \ingroup Object
 */
template <class TCalibration>
class ToolCalibrationReader : public Object
{

public:

  /** Macro with standard traits declarations. */
  igstkStandardTemplatedClassTraitsMacro( ToolCalibrationReader, Object )

  /** Transform typedef */
  typedef TCalibration        CalibrationType;

  /** Return the output of the reader as a Transform */
  const CalibrationType * GetCalibration() const;
  
  /** Type for representing the string of the filename. */
  typedef std::string    FileNameType;

protected:

  ToolCalibrationReader();
  ~ToolCalibrationReader();

  /** Print the object information in a stream. */
  void PrintSelf( std::ostream& os, ::itk::Indent indent ) const;

  bool ParseXML();
  FileNameType                       m_FileName;
  typename CalibrationType::Pointer  m_Calibration;

private:
  
  /** These two methods must be declared and note be implemented
   *  in order to enforce the protocol of smart pointers. */
  ToolCalibrationReader(const Self&);         //purposely not implemented
  void operator=(const Self&);      //purposely not implemented
};

} // end namespace igstk

#ifndef IGSTK_MANUAL_INSTANTIATION
#include "igstkToolCalibrationReader.txx"
#endif

#endif // __igstkToolCalibrationReader_h
