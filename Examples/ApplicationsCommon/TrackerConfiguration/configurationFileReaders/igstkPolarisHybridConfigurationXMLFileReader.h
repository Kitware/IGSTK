#ifndef __igstkPolarisHybridConfigurationXMLFileReader_h
#define __igstkPolarisHybridConfigurationXMLFileReader_h

#include <itkXMLFile.h>
#include "igstkSerialCommunicatingTrackerConfigurationXMLFileReader.h"


namespace igstk
{
class PolarisHybridTrackerConfiguration;

  
class PolarisHybridConfigurationXMLFileReader : 
   public SerialCommunicatingTrackerConfigurationXMLFileReader 
{
public:

    //standard typedefs
  typedef PolarisHybridConfigurationXMLFileReader                  Self;
  typedef SerialCommunicatingTrackerConfigurationXMLFileReader    Superclass;
  typedef itk::SmartPointer<Self>                                 Pointer;

           //run-time type information (and related methods)
  itkTypeMacro( PolarisHybridConfigurationXMLFileReader, 
                SerialCommunicatingTrackerConfigurationXMLFileReader );

         //method for creation through the object factory
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
  virtual const igstk::TrackerConfiguration * GetTrackerConfigurationData();


protected:

  virtual double GetMaximalRefreshRate();
  virtual std::string GetSystemType();

          //this is the constructor that is called by the factory to 
         //create a new object
  PolarisHybridConfigurationXMLFileReader() : 
         SerialCommunicatingTrackerConfigurationXMLFileReader()
         {}
  virtual ~PolarisHybridConfigurationXMLFileReader() {}

  void ProcessSromFile() 
    throw ( FileFormatException );

  void 
  ProcessControlBoxPort() 
    throw ( FileFormatException );

  virtual void ProcessToolData() 
    throw ( FileFormatException );

  virtual igstk::PolarisHybridTrackerConfiguration *
    GetPolarisConfiguration();

  std::string m_CurrentSromFileName;
  unsigned int m_CurrentControlBoxPort;
  bool m_HaveCurrentControlBoxPort;

private:
  PolarisHybridConfigurationXMLFileReader( 
    const PolarisHybridConfigurationXMLFileReader & other );


  const PolarisHybridConfigurationXMLFileReader & operator=( 
    const PolarisHybridConfigurationXMLFileReader & right );
};


}
#endif //__igstkPolarisHybridConfigurationXMLFileReader_h
