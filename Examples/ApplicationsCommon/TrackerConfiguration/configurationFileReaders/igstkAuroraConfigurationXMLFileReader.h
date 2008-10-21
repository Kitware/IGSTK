#ifndef __igstkAuroraConfigurationXMLFileReader_h
#define __igstkAuroraConfigurationXMLFileReader_h

#include <itkXMLFile.h>
#include "igstkSerialCommunicatingTrackerConfigurationXMLFileReader.h"


namespace igstk
{
class AuroraConfigurationXMLFileReader : 
   public SerialCommunicatingTrackerConfigurationXMLFileReader 
{
public:

    //standard typedefs
  typedef AuroraConfigurationXMLFileReader                        Self;
  typedef SerialCommunicatingTrackerConfigurationXMLFileReader    Superclass;
  typedef itk::SmartPointer<Self>                                 Pointer;

           //run-time type information (and related methods)
  itkTypeMacro( AuroraConfigurationXMLFileReader, 
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
  virtual igstk::TrackerConfiguration * GetTrackerConfigurationData();


protected:
          //this is the constructor that is called by the factory to 
         //create a new object
  AuroraConfigurationXMLFileReader() : 
    SerialCommunicatingTrackerConfigurationXMLFileReader(),     
    m_HaveCurrentControlBoxPort( false ),
    m_HaveCurrentControlBoxChannel( false )    
         {}
  virtual ~AuroraConfigurationXMLFileReader() {}

  void ProcessSromFile() 
    throw ( FileFormatException );

  void ProcessControlBoxPort() 
    throw ( FileFormatException );

  void ProcessControlBoxChannel() 
    throw ( FileFormatException );

  virtual void ProcessToolData() 
    throw ( FileFormatException );

  virtual double GetMaximalRefreshRate();
  virtual std::string GetSystemType();

  std::string m_CurrentSromFileName;
  unsigned int m_CurrentControlBoxPort;
  unsigned int m_CurrentControlBoxChannel;

  bool m_HaveCurrentControlBoxPort;
  bool m_HaveCurrentControlBoxChannel;

private:
  AuroraConfigurationXMLFileReader( 
    const AuroraConfigurationXMLFileReader & other );


  const AuroraConfigurationXMLFileReader & operator=( 
    const AuroraConfigurationXMLFileReader & right );
};


}
#endif //__igstkAuroraConfigurationXMLFileReader_h
