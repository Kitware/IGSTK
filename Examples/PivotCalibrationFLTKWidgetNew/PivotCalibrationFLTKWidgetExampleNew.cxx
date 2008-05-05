/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    $$
  Language:  C++
  Date:      $$
  Version:   $$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "PivotCalibrationFLTKWidgetExampleNew.h"


#include "igstkSerialCommunication.h"

#include "PolarisTrackerConfigurationGUI.h"
#include "AuroraTrackerConfigurationGUI.h"
#include "MicronTrackerConfigurationGUI.h"
#include "FlockOfBirdsTrackerConfigurationGUI.h"

/** -----------------------------------------------------------------
*  Choose to connect to a tracker
*  A tracker specific GUI is initialized, and the observer is hooked
*  to it to catch the configuration. When the "confirm" button is
*  pressed, an event loaded with tracker configuration will be sent out.
*  And it will trigger the RequestInitializeTracker(), which does the 
*  actual tracker initialization
*---------------------------------------------------------------------
*/
void PivotCalibrationFLTKWidgetExample::RequestConnectToTracker()
{
    switch( ConnectToTrackerBtn->value() )
    {
    case 0:
        {
            igstk::TrackerConfigurationNew config = igstk::TrackerConfigurationNew();
            config.SetTrackerType( igstk::TrackerConfigurationNew::Polaris );
            PolarisTrackerConfigurationGUI * gui;
            m_TrackerConfigurationGUI = gui = new PolarisTrackerConfigurationGUI();
            m_TrackerConfigurationGUI->SetConfiguration( config );
            m_TrackerConfigurationGUI->RemoveAllObservers();
            m_TrackerConfigurationGUI->AddObserver(
                igstk::TrackerConfigurationGUIBase::ConfigurationEvent(),
                m_TrackerConfigurationObserver);
            break;
        }
    case 1:
        {
            igstk::TrackerConfigurationNew config = igstk::TrackerConfigurationNew();
            config.SetTrackerType( igstk::TrackerConfigurationNew::Aurora );
            AuroraTrackerConfigurationGUI * gui;
            m_TrackerConfigurationGUI = gui = new AuroraTrackerConfigurationGUI();
            m_TrackerConfigurationGUI->SetConfiguration( config );
            m_TrackerConfigurationGUI->RemoveAllObservers();
            m_TrackerConfigurationGUI->AddObserver(
                igstk::TrackerConfigurationGUIBase::ConfigurationEvent(),
                m_TrackerConfigurationObserver);
            break;
        }
    case 2:
        {
            igstk::TrackerConfigurationNew config = igstk::TrackerConfigurationNew();
            config.SetTrackerType( igstk::TrackerConfigurationNew::Micron );
            MicronTrackerConfigurationGUI * gui;
            m_TrackerConfigurationGUI = gui = new MicronTrackerConfigurationGUI();
            m_TrackerConfigurationGUI->SetConfiguration( config );
            m_TrackerConfigurationGUI->RemoveAllObservers();
            m_TrackerConfigurationGUI->AddObserver(
                igstk::TrackerConfigurationGUIBase::ConfigurationEvent(),
                m_TrackerConfigurationObserver);
            break;
        }
    case 3:
        {
            igstk::TrackerConfigurationNew config = igstk::TrackerConfigurationNew();
            config.SetTrackerType( igstk::TrackerConfigurationNew::FlockOfBirds );
            FlockOfBirdsTrackerConfigurationGUI * gui;
            m_TrackerConfigurationGUI = gui = new FlockOfBirdsTrackerConfigurationGUI();
            m_TrackerConfigurationGUI->SetConfiguration( config );
            m_TrackerConfigurationGUI->RemoveAllObservers();
            m_TrackerConfigurationGUI->AddObserver(
                igstk::TrackerConfigurationGUIBase::ConfigurationEvent(),
                m_TrackerConfigurationObserver);
            break;
        }
    }
}

/** -----------------------------------------------------------------
*  Choose to disconnect a tracker
*---------------------------------------------------------------------
*/
void PivotCalibrationFLTKWidgetExample::RequestDisconnectTracker()
{
    if (m_TrackerInitializer)
    {
        m_TrackerInitializer->RequestStopAndDisconnectTracker();
        this->m_initialized = false;
        TrackerList->clear();
    }
}

/** -----------------------------------------------------------------
*  Call back function for ConfigurationEvent observer
*---------------------------------------------------------------------
*/
void PivotCalibrationFLTKWidgetExample::RequestInitializeTracker(const itk::EventObject & event)
{
    typedef igstk::TrackerConfigurationGUIBase  GUIType;
    if ( GUIType::ConfigurationEvent().CheckEvent( &event ) )
    {
        GUIType::ConfigurationEvent *confEvent =
            ( GUIType::ConfigurationEvent *) & event;

        igstk::TrackerConfigurationNew  tc = confEvent->Get();

        m_TrackerInitializer = new igstk::TrackerInitializerNew;
        m_TrackerInitializer->SetTrackerConfiguration( & tc );
        m_TrackerInitializer->SetLogger(this->GetLogger());

        if ( m_TrackerInitializer->RequestInitializeTracker() )
        {

            m_Tracker = m_TrackerInitializer->GetTracker();

            m_TrackerTool = m_TrackerInitializer->GetNonReferenceToolList()[0];

            TrackerList->clear();

            std::string s;
            s = "[" + m_TrackerInitializer->GetTrackerTypeAsString() + "]";
            TrackerList->add( s.c_str() );

            TrackerList->value(0);

            this->m_initialized = true;
        }
    }
}

PivotCalibrationFLTKWidgetExample::PivotCalibrationFLTKWidgetExample(): m_initialized(false)
{
    /** 
    *  This observer catches the tracker configuration sent out by
    *  TrackerConfiguration GUI
    */
    m_TrackerConfigurationObserver = LoadedObserverType::New();
    m_TrackerConfigurationObserver->SetCallbackFunction( this,
         &PivotCalibrationFLTKWidgetExample::RequestInitializeTracker);

    m_TrackerInitializer = NULL;
}

/** -----------------------------------------------------------------
*     Destructor
*  -----------------------------------------------------------------
*/
PivotCalibrationFLTKWidgetExample::~PivotCalibrationFLTKWidgetExample()
{
}

void
PivotCalibrationFLTKWidgetExample::InitializeTrackingAndCalibration()
{
  unsigned int delay =
            static_cast<unsigned int>( this->m_delaySlider->value() );

  unsigned int numberOfFrames =
            static_cast<unsigned int>( this->m_numberOfFramesSlider->value() );

   //if already initialized, shutdown everything and reinitialize
  if( this->m_initialized )
  {
    if ( m_TrackerInitializer->RequestInitializeTracker() )
        {
            m_Tracker = m_TrackerInitializer->GetTracker();
            m_TrackerTool = m_TrackerInitializer->GetNonReferenceToolList()[0];
        }
  }

  this->m_initialized = true;

  this->m_pivotCalibrationFLTKWidget->RequestSetDelay( delay );

  igstk::Tracker * genericTracker = this->m_Tracker.GetPointer();
  igstk::TrackerTool * genericTrackerTool = this->m_TrackerTool.GetPointer();

  this->m_pivotCalibrationFLTKWidget->RequestInitialize(numberOfFrames,
                                                        genericTracker,
                                                        genericTrackerTool );
}
