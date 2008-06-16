#ifndef __igstkTrackerConfigurationGUIBase_h
#define __igstkTrackerConfigurationGUIBase_h

#include "itkCommand.h"
#include "igstkTrackerConfiguration.h"

class NeedleBiopsyUnimodality;


//#include "PivotCalibrationFLTKWidgetExample.h"

namespace igstk
{

class TrackerConfigurationGUIBase : public itk::Command 
{
public:

  TrackerConfigurationGUIBase();

  void SetParent( NeedleBiopsyUnimodality *parent );

  void UpdateParentConfiguration();
  virtual igstk::TrackerConfiguration *GetConfiguration()=0;

  virtual void Show()=0;
  virtual void Hide()=0;

  virtual void Execute(itk::Object *caller, const itk::EventObject & event);
  virtual void Execute(const itk::Object *caller, const itk::EventObject & event);


  virtual ~TrackerConfigurationGUIBase();

protected:
  NeedleBiopsyUnimodality *m_Parent;
  bool m_ConfigurationErrorOccured;
};

} // end of name space
#endif
