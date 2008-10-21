#ifndef __igstkPolarisWirelessConfigurationXMLFileReader_h
#define __igstkPolarisWirelessConfigurationXMLFileReader_h

#include <itkXMLFile.h>
#include "igstkSerialCommunicatingTrackerConfigurationXMLFileReader.h"


namespace igstk
{
class PolarisWirelessTrackerConfiguration;

  
class PolarisWirelessConfigurationXMLFileReader : 
   public SerialCommunicatingTrackerConfigurationXMLFileReader 
{
public:

    //standard typedefs
  typedef PolarisWirelessConfigurationXMLFileReader                  Self;
  typedef SerialCommunicatingTrackerConfigurationXMLFileReader    Superclass;
  typedef itk::SmartPointer<Self>                                 Pointer;

           //run-time type information (and related methods)
  itkTypeMacro( PolarisWirelessConfigurationXMLFileReader, 
                SerialCommunicatingTrackerConfigurationXMLFileReader );

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
  virtual igstk::TrackerConfiguration * GetTrackerConfigurationData();


protected:
          //this is the constructor that is called by the factory to 
         //create a new object
  PolarisWirelessConfigurationXMLFileReader() : 
         SerialCommunicatingTrackerConfigurationXMLFileReader()
         {}
  virtual ~PolarisWirelessConfigurationXMLFileReader() {}

  void ProcessSromFile() 
    throw ( FileFormatException );

  virtual void ProcessToolData() 
    throw ( FileFormatException );

  virtual igstk::PolarisWirelessTrackerConfiguration *
    GetPolarisConfiguration()=0;

  std::string m_CurrentSromFileName;

private:
  PolarisWirelessConfigurationXMLFileReader( 
    const PolarisWirelessConfigurationXMLFileReader & other );


  const PolarisWirelessConfigurationXMLFileReader & operator=( 
    const PolarisWirelessConfigurationXMLFileReader & right );
};


}
#endif //__igstkPolarisWirelessConfigurationXMLFileReader_h
