/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    PivotCalibrationFLTKWidgetExample.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __PivotCalibrationFLTKWidgetExample_h
#define __PivotCalibrationFLTKWidgetExample_h

#include "itkCommand.h"
#include "igstkPolarisTracker.h"
#include "igstkPolarisTrackerTool.h"
#include "igstkSerialCommunication.h"
#include "PivotCalibrationFLTKWidgetExampleUI.h"


class PivotCalibrationFLTKWidgetExample :
  public PivotCalibrationFLTKWidgetExampleUI
{
public:
  PivotCalibrationFLTKWidgetExample();
  virtual void InitializeTrackingAndCalibration();

private:

  /* Observe all tracking related errors.*/
  class TrackingErrorObserver : public itk::Command
    {
    public:
      typedef TrackingErrorObserver            Self;
      typedef ::itk::Command                   Superclass;
      typedef ::itk::SmartPointer<Self>        Pointer;
      typedef ::itk::SmartPointer<const Self>  ConstPointer;

      igstkNewMacro(Self)
      igstkTypeMacro(IGSTKErrorObserver, itk::Command)

      /*
       * When an error occurs in an IGSTK component it will invoke this method
       * with the appropriate error event object as a parameter. */
      void Execute(itk::Object *caller, const itk::EventObject & event);

      /**
      * When an error occurs in an IGSTK component it will invoke this method
      * with the appropriate error event object as a parameter. */
      void Execute(const itk::Object *caller, const itk::EventObject & event);

      /**
       * Clear the current error. */
      void ClearError();

      /**
      * If an error occurs in one of the observed IGSTK components this method
      * will return true. */
      bool Error();

    protected:

      /**
       * Construct an error observer for all the possible errors that occur in
       * IGSTK components used in the DataAcqusition class. */
      TrackingErrorObserver();

      virtual ~TrackingErrorObserver(){}

    private:

      /**
       *  member variables
       */
      bool                                m_ErrorOccured;
      std::map<std::string,std::string>   m_ErrorEvent2ErrorMessage;

      //purposely not implemented
      TrackingErrorObserver(const Self&);
      void operator=(const Self&);
    };

                         //constant settings for the serial communication
  static const igstk::SerialCommunication::ParityType PARITY;
  static const igstk::SerialCommunication::BaudRateType BAUD_RATE;
  static const igstk::SerialCommunication::DataBitsType DATA_BITS;
  static const igstk::SerialCommunication::StopBitsType STOP_BITS;
  static const igstk::SerialCommunication::HandshakeType HAND_SHAKE;


  igstk::PolarisTracker::Pointer m_tracker;
  igstk::PolarisTrackerTool::Pointer m_tool;
  igstk::SerialCommunication::Pointer m_serialCommunication;
  TrackingErrorObserver::Pointer m_errorObserver;
  bool m_initialized;

};

#endif //_PIVOT_CALIBRATION_FLTK_WIDGET_EXAMPLE_H_
