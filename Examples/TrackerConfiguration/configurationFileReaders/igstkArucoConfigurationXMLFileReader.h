/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkArucoConfigurationXMLFileReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkArucoConfigurationXMLFileReader_h
#define __igstkArucoConfigurationXMLFileReader_h

#include <itkXMLFile.h>
#include "igstkTrackerConfigurationXMLFileReaderBase.h"

namespace igstk
{
/**
 * \class ArucoConfigurationXMLFileReader
 *
 * \brief This class is used to read the xml
 *        configuration file for the Aruco monocular tracking implementation.
 *
 */
class ArucoConfigurationXMLFileReader :
   public TrackerConfigurationXMLFileReaderBase
{
public:

  //standard typedefs
  typedef ArucoConfigurationXMLFileReader           Self;
  typedef TrackerConfigurationXMLFileReaderBase     Superclass;
  typedef itk::SmartPointer<Self>                   Pointer;

  //run-time type information (and related methods)
  itkTypeMacro( ArucoConfigurationXMLFileReader,
                TrackerConfigurationXMLFileReaderBase );

  itkNewMacro( Self );

  /**
   * Method called when a new xml tag start is encountered.
   */
  virtual void StartElement( const char * name, const char **atts );


  /**
   * Method called when an xml tag end is encountered.
   */
  virtual void EndElement( const char *name );


  /**
   * Method for checking if the configuration data has been read. When the xml
   * file is empty or doesn't contain the relevant tags the read operation is
   * successful, but we don't have the information we need.
   */
  virtual bool HaveConfigurationData();


  /**
   * Return a pointer to the object containing the configuration data.
   */
  virtual const igstk::TrackerConfiguration::Pointer
    GetTrackerConfigurationData() throw ( FileFormatException );


protected:

  virtual double GetMaximalRefreshRate();
  virtual std::string GetSystemType();

  //this is the constructor that is called by the factory to
  //create a new object
  ArucoConfigurationXMLFileReader() : TrackerConfigurationXMLFileReaderBase()
    {
    m_HaveCurrentMarkerID = false;
    m_HaveMarkerSize = false;
    }
  virtual ~ArucoConfigurationXMLFileReader() {}

  void ProcessArucoCalibrationFile() throw ( FileFormatException );

  void ProcessArucoMarkerSize() throw ( FileFormatException );

  void ProcessMarkerID() throw ( FileFormatException );

  virtual void ProcessToolData() throw ( FileFormatException );

  std::string m_ArucoCalibrationFile;
  unsigned int m_MarkerSize;
  bool m_HaveMarkerSize;
  unsigned int m_CurrentMarkerID;
  bool m_HaveCurrentMarkerID;

private:
  ArucoConfigurationXMLFileReader(
    const ArucoConfigurationXMLFileReader & other );


  const ArucoConfigurationXMLFileReader & operator=(
    const ArucoConfigurationXMLFileReader & right );
};


}
#endif //__igstkMicronConfigurationXMLFileReader_h
