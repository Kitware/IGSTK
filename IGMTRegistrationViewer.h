
#ifndef _IGSTK_IGMTRegistrationViewer_H_
#define _IGSTK_IGMTRegistrationViewer_H_

#include "IGMTVolumeViewer.h"

#include "itkPointSet.h"

#include "vtkActor.h"
#include "vtkAssembly.h"

namespace IGSTK
{

class IGMTRegistrationViewer : public IGMTVolumeViewer
{
public:

  typedef itk::PointSet<double, 3> PointSetType;

public:
  
  IGMTRegistrationViewer(void);
  
  virtual ~IGMTRegistrationViewer(void);

  void SetFixedPointSet(PointSetType::Pointer pointset);

  void SetFixedPointRadius(double radius);

private:

  vtkAssembly* m_FixedPointSetAssembly;
  
  vtkAssembly* m_MovingPointSetAssembly;

  double m_FixedPointRadius;


};

}

#endif


