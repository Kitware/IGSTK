/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    OpenIGTLinkTrackingBroadcaster.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "OpenIGTLinkTrackingBroadcaster.h"

OpenIGTLinkTrackingBroadcaster::OpenIGTLinkTrackingBroadcaster( 
std::string &trackerXMLConfigurationFileName ) throw ( ExceptionWithMessage )
{
  igstk::OpenIGTLinkTrackerConfigurationFileReader::
    OpenIGTLinkConfigurationDataType
                    //if there is an error reading a recognized tracker's 
                    //configuration an exception is thrown , otherwise the method
                    //returns NULL
    *oigtLinkTrackerConfiguration =
     GetTrackerConfiguration( trackerXMLConfigurationFileName );
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
  this->m_TrackerController->AddObserver(
    igstk::TrackerController::RequestToolsEvent(),
    requestNonReferenceToolsObserver );

  this->m_TrackerController->RequestGetNonReferenceToolList();
  if( requestNonReferenceToolsObserver->GotToolList() )
    {                   //get the tools from the tracker controller
    igstk::TrackerController::ToolContainerType toolContainer = 
      requestNonReferenceToolsObserver->GetToolList();
    igstk::TrackerController::ToolContainerType::iterator toolNamesAndToolsIt;
    //pair the tracker tools with the oigtlink data
    igstk::OpenIGTLinkTrackerConfigurationFileReader::
      OpenIGTLinkDataType::iterator
      toolNamesAndDestinationsIt, toolNamesAndDestinationsEnd;

    toolNamesAndDestinationsIt =
      oigtLinkTrackerConfiguration->m_ToolNamesAndConnections.begin();
    toolNamesAndDestinationsEnd =
      oigtLinkTrackerConfiguration->m_ToolNamesAndConnections.end();

    for(; toolNamesAndDestinationsIt!=toolNamesAndDestinationsEnd; 
      toolNamesAndDestinationsIt++ )
      {
      toolNamesAndToolsIt = toolContainer.find(
        toolNamesAndDestinationsIt->first );   
      if( toolNamesAndToolsIt != toolContainer.end() )
        {
        ToolUpdatedObserver::Pointer updateObserver = ToolUpdatedObserver::New();
        //this method can throw an exception, the user is responsible for
        //dealing with this
        updateObserver->Initialize( toolNamesAndDestinationsIt->first, 
          toolNamesAndToolsIt->second, 
          world, 
          toolNamesAndDestinationsIt->second );

        toolNamesAndToolsIt->second->AddObserver(
          igstk::TrackerToolTransformUpdateEvent(), 
          updateObserver );
        } 
      }
    }
}


void 
OpenIGTLinkTrackingBroadcaster::StartTracking()
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
OpenIGTLinkTrackingBroadcaster::StopTracking()
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


igstk::OpenIGTLinkTrackerConfigurationFileReader::
  OpenIGTLinkConfigurationDataType
* OpenIGTLinkTrackingBroadcaster::GetTrackerConfiguration( 
  std::string &configurationFileName) throw ( ExceptionWithMessage )
{
  const unsigned int NUM_TRACKER_TYPES = 7;
  igstk::TrackerConfigurationXMLFileReaderBase::Pointer 
    trackerConfigurationXMLReaders[NUM_TRACKER_TYPES];
  trackerConfigurationXMLReaders[0] = 
    igstk::PolarisVicraConfigurationXMLFileReader::New();
  trackerConfigurationXMLReaders[1] = 
    igstk::PolarisSpectraConfigurationXMLFileReader::New();
  trackerConfigurationXMLReaders[2] = 
    igstk::PolarisHybridConfigurationXMLFileReader::New();
  trackerConfigurationXMLReaders[3] = 
    igstk::AuroraConfigurationXMLFileReader::New();
  trackerConfigurationXMLReaders[4] = 
    igstk::MicronConfigurationXMLFileReader::New();
  trackerConfigurationXMLReaders[5] =
       igstk::AscensionConfigurationXMLFileReader::New();
  trackerConfigurationXMLReaders[6] = 
    igstk::Ascension3DGConfigurationXMLFileReader::New();

  igstk::OpenIGTLinkTrackerConfigurationFileReader::Pointer trackerConfigReader =
    igstk::OpenIGTLinkTrackerConfigurationFileReader::New();

                //need to observe if the request read succeeds or fails
                //there is a third option that the read is invalid, if the
                //file name or xml reader weren't set
  igstk::OpenIGTLinkTrackerConfigurationFileReader::
    ReadFailSuccessObserver::Pointer 
    rfso = igstk::OpenIGTLinkTrackerConfigurationFileReader::
    ReadFailSuccessObserver::New();
  trackerConfigReader->AddObserver(
     igstk::OpenIGTLinkTrackerConfigurationFileReader::ReadSuccessEvent(),
     rfso );
  trackerConfigReader->AddObserver(
     igstk::OpenIGTLinkTrackerConfigurationFileReader::ReadFailureEvent(),
     rfso );
  trackerConfigReader->AddObserver(
     igstk::OpenIGTLinkTrackerConfigurationFileReader::
     UnexpectedTrackerTypeEvent(),
     rfso );

             //setting the file name and reader always succeeds so I don't
             //observe for success event
  trackerConfigReader->RequestSetFileName( configurationFileName );

  OpenIGTLinkTrackerConfigurationObserver::Pointer tco = 
    OpenIGTLinkTrackerConfigurationObserver::New();

  for( unsigned int i=0; i<NUM_TRACKER_TYPES; i++ )
    {
    //setting the xml reader always succeeds so I don't
    //observe the success event
    trackerConfigReader->RequestSetReader( trackerConfigurationXMLReaders[i] );  

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
        igstk::OpenIGTLinkTrackerConfigurationFileReader::
        OpenIGTLinkConfigurationDataEvent(),
        tco );
      trackerConfigReader->RequestGetData();

      if( tco->GotOpenIGTLinkTrackerConfiguration() )
        {
        igstk::OpenIGTLinkTrackerConfigurationFileReader::
          OpenIGTLinkConfigurationDataType *returnedResult =
          new igstk::OpenIGTLinkTrackerConfigurationFileReader::
          OpenIGTLinkConfigurationDataType();
        igstk::OpenIGTLinkTrackerConfigurationFileReader::
          OpenIGTLinkConfigurationDataType *
          result = tco->GetOpenIGTLinkTrackerConfiguration();

        returnedResult->m_TrackerConfiguration = result->m_TrackerConfiguration;
        returnedResult->m_ToolNamesAndConnections.insert(
          result->m_ToolNamesAndConnections.begin(), 
          result->m_ToolNamesAndConnections.end() );
        return returnedResult;
        }
      }
    }
  return NULL;
}
