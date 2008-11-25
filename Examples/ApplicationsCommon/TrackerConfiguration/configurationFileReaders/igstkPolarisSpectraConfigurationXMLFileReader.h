#ifndef __igstkPolarisSpectraConfigurationXMLFileReader_h
#define __igstkPolarisSpectraConfigurationXMLFileReader_h

#include <itkXMLFile.h>
#include "igstkPolarisWirelessConfigurationXMLFileReader.h"


namespace igstk
{

/**
 * \class PolarisSpectraConfigurationXMLFileReader This class is used to read 
 *        the xml configuration file for NDI's Polaris Spectra tracker.
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
