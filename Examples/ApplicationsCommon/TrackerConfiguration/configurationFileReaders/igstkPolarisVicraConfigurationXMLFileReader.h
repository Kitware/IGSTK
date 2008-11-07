#ifndef __igstkPolarisVicraConfigurationXMLFileReader_h
#define __igstkPolarisVicraConfigurationXMLFileReader_h

#include <itkXMLFile.h>
#include "igstkPolarisWirelessConfigurationXMLFileReader.h"


namespace igstk
{
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
         PolarisWirelessConfigurationXMLFileReader()
         {}
  virtual ~PolarisVicraConfigurationXMLFileReader() {}

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
