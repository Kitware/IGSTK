/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    PolarisTrackingUsingQTWidgetGUI.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __PolarisTrackingUsingQTWidgetGUI_h
#define __PolarisTrackingUsingQTWidgetGUI_h

#include <QtGui>

#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkPolarisTracker.h"
#include "igstkPolarisTrackerTool.h"
#include "igstkView.h"
#include "igstkSerialCommunication.h"
#include "igstkLogger.h"
#include "itkStdStreamLogOutput.h"

#include "ui_PolarisTrackingUsingQTWidgetGUI.h"

class PolarisTrackingUsingQTWidgetGUI : public QMainWindow
{
  Q_OBJECT

  typedef igstk::Object::LoggerType    LoggerType; 
  typedef itk::StdStreamLogOutput      LogOutputType;
  typedef igstk::PolarisTracker        TrackerType;
  typedef igstk::PolarisTrackerTool    TrackerToolType;
  typedef igstk::SerialCommunication   CommunicationType;

public:
  PolarisTrackingUsingQTWidgetGUI(int portNo=0, const std::string & romFile="");
  ~PolarisTrackingUsingQTWidgetGUI();
  void AttachObjectToTrack( igstk::SpatialObject *objectToTrack);
  void SetView( igstk::View * view );
  bool HasQuitted();
  TrackerType::Pointer GetTracker();

public slots:
  void OnQuitAction();
  void OnTrackingAction( int state);
  void OnInteractionAction( int state);

private:
  void CreateActions();

  Ui::MainWindow              m_GUI;

  LoggerType::Pointer         m_Logger;
  LogOutputType::Pointer      m_LogOutput;
  TrackerType::Pointer        m_Tracker;
  TrackerToolType::Pointer    m_TrackerTool;
  
  igstk::SpatialObject::Pointer  m_ObjectToTrack;

  CommunicationType::Pointer m_Communication;

  bool                    m_Tracking;
  
  std::ofstream           m_LogFile;

  bool                    m_GUIQuit;

};

#endif
