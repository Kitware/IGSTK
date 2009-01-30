/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkPolarisVicraConfigurationXMLFileReader.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __igstkPolarisVicraConfigurationXMLFileReader_h
#define __igstkPolarisVicraConfigurationXMLFileReader_h

#include <itkXMLFile.h>
#include "igstkPolarisWirelessConfigurationXMLFileReader.h"


namespace igstk
{
/**
 * \class PolarisVicraConfigurationXMLFileReader 
 *
 * \brief This class is used to read the 
 *        xml configuration file for NDI's Polaris vicra tracker.
 *
 *        This class is used to read the 
 *        xml configuration file for NDI's Polaris vicra tracker.
 */  
class PolarisVicraConfigurationXMLFileReader : 
   public PolarisWirelessConfigurationXMLFileReader 
{
public:

  //standard typedefs
  typedef PolarisVicraConfigurationXMLFileReader                  Self;
  typedef PolarisWirelessConfigurationXMLFileReader               Superclass;
  typedef itk::SmartPointer<Self>                                 Pointer;

  //run-time type information (and related methods)
  itkTypeMacro( PolarisVicraConfigurationXMLFileReader, 
                PolarisWirelessConfigurationXMLFileReader );

  //method for creation through the object factory
  itkNewMacro( Self );

protected:
  //this is the constructor that is called by the factory to 
  //create a new object
  PolarisVicraConfigurationXMLFileReader() : 
    PolarisWirelessConfigurationXMLFileReader() { }
  virtual ~PolarisVicraConfigurationXMLFileReader() { }

  virtual double GetMaximalRefreshRate();
  virtual std::string GetSystemType();
  virtual igstk::PolarisWirelessTrackerConfiguration::Pointer 
    GetPolarisConfiguration();


private:
  PolarisVicraConfigurationXMLFileReader( 
    const PolarisVicraConfigurationXMLFileReader & other );


  const PolarisVicraConfigurationXMLFileReader & operator=( 
    const PolarisVicraConfigurationXMLFileReader & right );
};


}
#endif //__igstkPolarisVicraConfigurationXMLFileReader_h
