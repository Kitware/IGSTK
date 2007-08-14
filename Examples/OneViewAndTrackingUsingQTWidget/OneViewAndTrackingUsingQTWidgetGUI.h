/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    OneViewAndTrackingUsingQTWidgetGUI.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#
#ifndef __OneViewAndTrackingUsingQTWidgetGUI_h
#define __OneViewAndTrackingUsingQTWidgetGUI_h

#include <QtGui>

#include "igstkEllipsoidObject.h"
#include "igstkCylinderObject.h"
#include "igstkEllipsoidObjectRepresentation.h"
#include "igstkCylinderObjectRepresentation.h"
#include "igstkQMouseTracker.h"
#include "igstkViewNew.h"

#ifdef WIN32
#include "igstkSerialCommunicationForWindows.h"
#else
#include "igstkSerialCommunicationForPosix.h"
#endif

#include "itkLogger.h"
#include "itkStdStreamLogOutput.h"

#include "ui_OneViewAndTrackingUsingQTWidgetGUI.h"

class OneViewAndTrackingUsingQTWidgetGUI : public QMainWindow
{
  Q_OBJECT

  typedef itk::Logger              LoggerType; 
  typedef itk::StdStreamLogOutput  LogOutputType;

  typedef igstk::QMouseTracker     TrackerType;

#ifdef WIN32
  typedef igstk::SerialCommunicationForWindows  CommunicationType;
#else
  typedef igstk::SerialCommunicationForPosix    CommunicationType;
#endif

public:
  OneViewAndTrackingUsingQTWidgetGUI();
  ~OneViewAndTrackingUsingQTWidgetGUI();
  void AttachObjectToTrack( igstk::SpatialObject *objectToTrack);
  void SetView( igstk::ViewNew * view );
  bool HasQuitted();

public slots:
  void OnQuitAction();
  void OnTrackingAction( int state);
  void OnInteractionAction( int state);

private:
  void CreateActions();

  Ui::MainWindow ui;

  LoggerType::Pointer     m_Logger;
  LogOutputType::Pointer  m_LogOutput;
  TrackerType::Pointer    m_Tracker;

  CommunicationType::Pointer m_Communication;

  bool                    m_Tracking;
  
  std::ofstream           m_LogFile;

  bool                    m_GUIQuit;
};

#endif
