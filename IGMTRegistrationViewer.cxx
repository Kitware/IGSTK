#include "IGMTRegistrationViewer.h"

#include "vtkProp3DCollection.h"
#include "vtkProperty.h"

namespace IGSTK
{

IGMTRegistrationViewer::IGMTRegistrationViewer(void)
{
  IGMTVolumeViewer::IGMTVolumeViewer();

  m_FixedPointRadius = 1.0;

  m_FixedPointSetAssembly = vtkAssembly::New();
  m_MovingPointSetAssembly = vtkAssembly::New();

//  this->GetRoot()->AddPart(m_FixedPointSetAssembly);
//  this->GetRoot()->AddPart(m_MovingPointSetAssembly);
}

IGMTRegistrationViewer::~IGMTRegistrationViewer(void)
{
}

void IGMTRegistrationViewer::SetFixedPointSet(PointSetType::Pointer pointset)
{
  unsigned int i, num = pointset->GetNumberOfPoints();
  PointSetType::PointType point;
  vtkSphereSource* sphere;
  vtkPolyDataMapper* mapper;
  vtkActor* actor;

  m_FixedPointSetAssembly->GetParts()->RemoveAllItems();

  for (i = 0; i < num; i++)
  {
    pointset->GetPoint(i, &point);
    
    sphere = vtkSphereSource::New();
    sphere->SetCenter(point[0], point[1], point[2]);
    sphere->SetRadius(m_FixedPointRadius);

    mapper = vtkPolyDataMapper::New();
    mapper->SetInput(sphere->GetOutput());

    actor = vtkActor::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetOpacity(0.1);
    actor->GetProperty()->SetColor(0.0, 1.0, 0.0);

    m_FixedPointSetAssembly->AddPart(actor);
  }

}

void IGMTRegistrationViewer::SetFixedPointRadius(double radius)
{
  m_FixedPointRadius = radius;
}

}
