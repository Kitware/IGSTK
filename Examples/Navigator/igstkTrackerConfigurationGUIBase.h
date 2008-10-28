/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTrackerConfigurationGUIBase.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkTrackerConfigurationGUIBase_h
#define __igstkTrackerConfigurationGUIBase_h

#include "itkCommand.h"
#include "igstkTrackerConfiguration.h"

class Navigator;


namespace igstk
{

class TrackerConfigurationGUIBase : public itk::Command 
{
public:

  TrackerConfigurationGUIBase();

  void SetParent( Navigator *parent );

  void UpdateParentConfiguration();

  int InitializeAsMicronDefault();

  virtual igstk::TrackerConfiguration *GetConfiguration()=0;

  virtual void Show()=0;
  virtual void Hide()=0;

  virtual void Execute(itk::Object *caller, const itk::EventObject & event);
  virtual void Execute(const itk::Object *caller, const itk::EventObject & event);


  virtual ~TrackerConfigurationGUIBase();

protected:
  Navigator *m_Parent;
  bool m_ConfigurationErrorOccured;
};

} // end of name space
#endif
