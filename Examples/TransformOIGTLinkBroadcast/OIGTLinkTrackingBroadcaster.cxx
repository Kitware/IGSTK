#include "OIGTLinkTrackingBroadcaster.h"

OIGTLinkTrackingBroadcaster::OIGTLinkTrackingBroadcaster( 
std::string &trackerXMLConfigurationFileName ) throw ( ExceptionWithMessage )
{
  igstk::OIGTLinkTrackerConfigurationFileReader::OIGTLinkConfigurationDataType 
                    //if there is an error reading a recognized tracker's 
                    //configuration an exception is thrown , otherwise the method
                    //returns NULL
    *oigtLinkTrackerConfiguration = GetTrackerConfiguration( trackerXMLConfigurationFileName );
  if( !oigtLinkTrackerConfiguration )
  {
    throw ExceptionWithMessage( std::string( "Unknown tracker type." ) );
  }
  //initialize the tracker controller, and setup the observers
  this->m_TrackerController = 
    igstk::TrackerController::New();

  InitializeErrorObserver::Pointer ieo = 
    InitializeErrorObserver::New();
  this->m_TrackerController->AddObserver( 
    igstk::TrackerController::InitializeErrorEvent(), ieo );
  this->m_TrackerController->RequestInitialize( 
    oigtLinkTrackerConfiguration->m_TrackerConfiguration );
  if( ieo->GotInitializeError() )
  {
    throw ExceptionWithMessage( ieo->GetInitializeError() );      
  }
  //to deal with the optional use of a reference we use
  //the "world" coordinate system as our anchor. if there is no
  //reference it is connected as the parent of the tracker 
  // otherwise it is connected as a parent of the reference tool.
  //in both cases the transformation to parent is the identity.
  igstk::SpatialObject::Pointer world = 
    igstk::SpatialObject::New();  
  igstk::Transform I;

  //check if we have a reference and setup the observers
  ToolObserver::Pointer requestReferenceToolObserver = 
    ToolObserver::New();
  this->m_TrackerController->AddObserver( 
    igstk::TrackerController::RequestToolEvent(), 
    requestReferenceToolObserver );
  this->m_TrackerController->RequestGetReferenceTool();
  //we have a reference
  if( requestReferenceToolObserver->GotTool() )
  {
    igstk::TrackerController::ToolEntryType referenceToolData = 
      requestReferenceToolObserver->GetTool();
    referenceToolData.second->RequestSetTransformAndParent( I, world );
  }
  else 
  {
    this->m_TrackerController->RequestSetParentSpatialObject( I, world );
  }

  ToolListObserver::Pointer requestNonReferenceToolsObserver = 
    ToolListObserver::New();
  this->m_TrackerController->AddObserver( igstk::TrackerController::RequestToolsEvent(),
    requestNonReferenceToolsObserver );

  this->m_TrackerController->RequestGetNonReferenceToolList();
  if( requestNonReferenceToolsObserver->GotToolList() )
  {                   //get the tools from the tracker controller
    igstk::TrackerController::ToolContainerType toolContainer = 
      requestNonReferenceToolsObserver->GetToolList();
    igstk::TrackerController::ToolContainerType::iterator toolNamesAndToolsIt;
    //pair the tracker tools with the oigtlink data
    igstk::OIGTLinkTrackerConfigurationFileReader::OIGTLinkDataType::iterator 
      toolNamesAndDestinationsIt, toolNamesAndDestinationsEnd;

    toolNamesAndDestinationsIt =
      oigtLinkTrackerConfiguration->m_ToolNamesAndConnections.begin();
    toolNamesAndDestinationsEnd =
      oigtLinkTrackerConfiguration->m_ToolNamesAndConnections.end();

    for(; toolNamesAndDestinationsIt!=toolNamesAndDestinationsEnd; 
      toolNamesAndDestinationsIt++ )
    {
      toolNamesAndToolsIt = toolContainer.find( toolNamesAndDestinationsIt->first );   
      if( toolNamesAndToolsIt != toolContainer.end() )
      {
        ToolUpdatedObserver::Pointer updateObserver = ToolUpdatedObserver::New();
        //this method can throw an exception, the user is responsible for
        //dealing with this
        updateObserver->Initialize( toolNamesAndDestinationsIt->first, 
          toolNamesAndToolsIt->second, 
          world, 
          toolNamesAndDestinationsIt->second );          

        toolNamesAndToolsIt->second->AddObserver( igstk::TrackerToolTransformUpdateEvent(), 
          updateObserver );
      }
    }
  }
}


void 
OIGTLinkTrackingBroadcaster::StartTracking()
{
  StartTrackingErrorObserver::Pointer steo= 
    StartTrackingErrorObserver::New();

  unsigned long observerID = this->m_TrackerController->AddObserver( 
    igstk::TrackerStartTrackingErrorEvent(),
    steo );
  this->m_TrackerController->RequestStartTracking();
  this->m_TrackerController->RemoveObserver( observerID );

  if( steo->GotStartTrackingError() )
  {
    throw ExceptionWithMessage( std::string( "Failed to start tracking." ) );
  }
}
  

void 
OIGTLinkTrackingBroadcaster::StopTracking()
{
  StopTrackingErrorObserver::Pointer steo= 
    StopTrackingErrorObserver::New();

  unsigned long observerID = this->m_TrackerController->AddObserver( 
    igstk::TrackerStopTrackingErrorEvent(),
    steo );
  this->m_TrackerController->RequestStopTracking();
  this->m_TrackerController->RemoveObserver( observerID );

  if( steo->GotStopTrackingError() )
  {
    throw ExceptionWithMessage( std::string( "Failed to stop tracking." ) );
  }
}


igstk::OIGTLinkTrackerConfigurationFileReader::OIGTLinkConfigurationDataType 
* OIGTLinkTrackingBroadcaster::GetTrackerConfiguration( 
  std::string &configurationFileName) throw ( ExceptionWithMessage )
{
  const unsigned int NUM_TRACKER_TYPES = 5;
  igstk::TrackerConfigurationXMLFileReaderBase::Pointer 
    trackerCofigurationXMLReaders[NUM_TRACKER_TYPES];
  trackerCofigurationXMLReaders[0] = 
    igstk::PolarisVicraConfigurationXMLFileReader::New();
  trackerCofigurationXMLReaders[1] = 
    igstk::PolarisSpectraConfigurationXMLFileReader::New();
  trackerCofigurationXMLReaders[2] = 
    igstk::PolarisHybridConfigurationXMLFileReader::New();
  trackerCofigurationXMLReaders[3] = 
    igstk::AuroraConfigurationXMLFileReader::New();
  trackerCofigurationXMLReaders[4] = 
    igstk::MicronConfigurationXMLFileReader::New();


  igstk::OIGTLinkTrackerConfigurationFileReader::Pointer trackerConfigReader = 
    igstk::OIGTLinkTrackerConfigurationFileReader::New();

                //need to observe if the request read succeeds or fails
                //there is a third option that the read is invalid, if the
                //file name or xml reader weren't set
  igstk::OIGTLinkTrackerConfigurationFileReader::ReadFailSuccessObserver::Pointer 
    rfso = igstk::OIGTLinkTrackerConfigurationFileReader::ReadFailSuccessObserver::New();
  trackerConfigReader->AddObserver( igstk::OIGTLinkTrackerConfigurationFileReader::ReadSuccessEvent(),
                                    rfso );
  trackerConfigReader->AddObserver( igstk::OIGTLinkTrackerConfigurationFileReader::ReadFailureEvent(),
                                    rfso );
  trackerConfigReader->AddObserver( igstk::OIGTLinkTrackerConfigurationFileReader::UnexpectedTrackerTypeEvent(),
                                    rfso );

             //setting the file name and reader always succeeds so I don't
             //observe for success event
  trackerConfigReader->RequestSetFileName( configurationFileName );

  OIGTLinkTrackerConfigurationObserver::Pointer tco = 
    OIGTLinkTrackerConfigurationObserver::New();

  for( unsigned int i=0; i<NUM_TRACKER_TYPES; i++ )
  {
    //setting the xml reader always succeeds so I don't
    //observe the success event
    trackerConfigReader->RequestSetReader( trackerCofigurationXMLReaders[i] );  

    trackerConfigReader->RequestRead();


    if( rfso->GotUnexpectedTrackerType() )
    {
      rfso->Reset();
    }
    else if( rfso->GotFailure() && !rfso->GotUnexpectedTrackerType() )
    {
      throw ExceptionWithMessage( rfso->GetFailureMessage() );          
    }
    else if( rfso->GotSuccess() )
    {
      //get the configuration data from the reader
      trackerConfigReader->AddObserver( 
        igstk::OIGTLinkTrackerConfigurationFileReader::OIGTLinkConfigurationDataEvent(),
        tco );
      trackerConfigReader->RequestGetData();

      if( tco->GotOIGTLinkTrackerConfiguration() )
      {
        igstk::OIGTLinkTrackerConfigurationFileReader::OIGTLinkConfigurationDataType *returnedResult =
          new igstk::OIGTLinkTrackerConfigurationFileReader::OIGTLinkConfigurationDataType();
        igstk::OIGTLinkTrackerConfigurationFileReader::OIGTLinkConfigurationDataType *
          result = tco->GetOIGTLinkTrackerConfiguration();

        returnedResult->m_TrackerConfiguration = result->m_TrackerConfiguration;
        returnedResult->m_ToolNamesAndConnections.insert( result->m_ToolNamesAndConnections.begin(), 
                                                          result->m_ToolNamesAndConnections.end() );
        return returnedResult;
      }
    }
  }
  return NULL;
}
