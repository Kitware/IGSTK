#include "igstkAscensionTrackerConfiguration.h"

namespace igstk
{
               //maximal refresh rate for the 3D Guidance system with short/mid
               //range transmitters
const double AscensionTrackerConfiguration::MAXIMAL_REFERESH_RATE = 375;


AscensionTrackerConfiguration::AscensionTrackerConfiguration()  
{
  this->m_Frequency = this->MAXIMAL_REFERESH_RATE;
}


AscensionTrackerConfiguration::~AscensionTrackerConfiguration()
{
}


double
AscensionTrackerConfiguration::GetMaximalRefreshRate()
{
  return this->MAXIMAL_REFERESH_RATE;
}


void 
AscensionTrackerConfiguration::InternalAddTool( const
  TrackerToolConfiguration *tool, bool isReference )
{
  AddToolFailureEvent fe;
  const AscensionToolConfiguration *wiredTool = 
    dynamic_cast<const AscensionToolConfiguration *>( tool );
  

  if( wiredTool == NULL )
  {
    fe.Set( "Given tool configuration type not compatible with tracker type." );
    this->InvokeEvent( fe );
    return;
  }
   //check for valid bird name???

 /* if( wiredTool->GetPortNumber()>= this->MAXIMAL_PORT_NUMBER )
  {
    fe.Set( "Specified physical port number is invalid." );
    this->InvokeEvent( fe );
    return;
  }
  */
  
        //copy the tool and add it as a standard or dynamic reference tool
  AscensionToolConfiguration *newTool = 
    new AscensionToolConfiguration( *wiredTool );
  
  if( !isReference )
  {
    this->m_TrackerToolList.push_back( newTool );
  }
  else
  {
    delete this->m_ReferenceTool;
    this->m_ReferenceTool = newTool; 
  }
  this->InvokeEvent( AddToolSuccessEvent() );
}


AscensionToolConfiguration::AscensionToolConfiguration() : m_BirdName( "" ),
                                                           m_PortNumber( 0 )
{
}                                                                


AscensionToolConfiguration::AscensionToolConfiguration( const 
  AscensionToolConfiguration &other ) : TrackerToolConfiguration( other ) 
{
  this->m_BirdName = other.m_BirdName;
  this->m_PortNumber = other.m_PortNumber;
}                                                                


AscensionToolConfiguration::~AscensionToolConfiguration()
{
}


std::string 
AscensionToolConfiguration::GetToolTypeAsString()
{
  return "AscensionToolConfiguration";
}

} // end of name space
