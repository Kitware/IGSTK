/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisSpectraConfigurationXMLFileReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkPolarisSpectraConfigurationXMLFileReader_h
#define __igstkPolarisSpectraConfigurationXMLFileReader_h

#include <itkXMLFile.h>
#include "igstkPolarisWirelessConfigurationXMLFileReader.h"


namespace igstk
{

/**
 * \class PolarisSpectraConfigurationXMLFileReader 
 *
 * \brief This class is used to read 
 *        the xml configuration file for NDI's Polaris Spectra tracker.
 *
 *        This class is used to read 
 *        the xml configuration file for NDI's Polaris Spectra tracker.
 *
 */ 
class PolarisSpectraConfigurationXMLFileReader : 
   public PolarisWirelessConfigurationXMLFileReader 
{
public:

  //standard typedefs
  typedef PolarisSpectraConfigurationXMLFileReader                Self;
  typedef PolarisWirelessConfigurationXMLFileReader               Superclass;
  typedef itk::SmartPointer<Self>                                 Pointer;

  //run-time type information (and related methods)
  itkTypeMacro( PolarisSpectraConfigurationXMLFileReader, 
                PolarisWirelessConfigurationXMLFileReader );

  //method for creation through the object factory
  itkNewMacro( Self );

protected:
          //this is the constructor that is called by the factory to 
         //create a new object
  PolarisSpectraConfigurationXMLFileReader() : 
    PolarisWirelessConfigurationXMLFileReader() { }

  virtual ~PolarisSpectraConfigurationXMLFileReader() { }

  virtual double GetMaximalRefreshRate();
  virtual std::string GetSystemType();
  virtual igstk::PolarisWirelessTrackerConfiguration::Pointer 
    GetPolarisConfiguration();

private:
  PolarisSpectraConfigurationXMLFileReader( 
    const PolarisSpectraConfigurationXMLFileReader & other );


  const PolarisSpectraConfigurationXMLFileReader & operator=( 
    const PolarisSpectraConfigurationXMLFileReader & right );
};


}
#endif //__igstkPolarisSpectraConfigurationXMLFileReader_h
