/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    igstkQTWidget.h
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __igstkQTWidget_h
#define __igstkQTWidget_h

#ifdef _MSC_VER
#pragma warning ( disable : 4018 )
//Warning about: identifier was truncated to '255' characters in the debug
//information (MVC6.0 Debug)
#pragma warning( disable : 4284 )
#endif

// QT dared to define macro called DEBUG!!
#define QT_NO_DEBUG 1

// ITK headers
#include "itkCommand.h"
#include "itkLogger.h"

// IGSTK headers
#include "igstkMacros.h"
#include "igstkStateMachine.h"
1;5D

#include "QVTKWidget.h"

namespace igstk {

/** \class QTWidget
 * 
 * \brief Widget class to develop QT based GUI application 
 */
class QTWidget : public QVTKWidget
{

public:
    
  typedef QTWidget          Self;
  typedef QVTKWidget     Superclass;

  igstkTypeMacro( QTWidget, QVTKWidget );
  
  /** Declarations needed for the State Machine */
  igstkStateMachineMacro();

  /** Print the object information in a stream. */
  void Print( std::ostream& os, ::itk::Indent indent=0) const;

  /** Set up variables, types and methods related to the Logger */
  igstkLoggerMacro()

  /** Enable interaction */
  void EnableInteractions();

  /** Disable interaction */
  void DisableInteractions();

  /** Constructor */
#if QT_VERSION < 0x040000
    //! constructor for Qt 3
    QTWidget(QWidget* parent = NULL, const char* name = NULL, Qt::WFlags f = 0);
#else
    //! constructor for Qt 4
    QTWidget(QWidget* parent = NULL, Qt::WFlags f = 0);
#endif

  /** Destructor */
  virtual ~QTWidget( void );
  
  /** Update the display in order to render the new content of the scene */
  void Update();
 
protected:
  
  /** Print the object information in a stream. */
  virtual void PrintSelf( std::ostream& os, itk::Indent indent ) const; 
 
  /** Set the interactor style in the derived classes */
  void SetInteractorStyle( vtkInteractorStyle * style );

  /** Overload the method that manages mouse events in order
      to provide picker functionalities */
  void mouseReleaseEvent(QMouseEvent* e);

/** Overload the mouse move event to send transform events
    when the mouse is moved while the left button is down */
  void mouseMoveEvent(QMouseEvent* e);

private:

  /** Disable keyboard and mouse interactions */
  void DisableInteractionsProcessing();

  /** Enable keyboard and mouse interactions */
  void EnableInteractionsProcessing();

private:
  bool                    m_InteractionHandling;

};

std::ostream& operator<<(std::ostream& os, const QTWidget& o);

} // end namespace igstk

#endif
