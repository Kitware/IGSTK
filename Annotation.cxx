#include "Annotation.h"

#include "vtkProperty2D.h"
#include "vtkTextProperty.h"

namespace IGSTK
{

Annotation::Annotation()
{
  int i;

  for (i = 0; i < 4; i++)
  {
    m_ppDirectionMapper[i] = vtkTextMapper::New();
    m_ppDirectionMapper[i]->SetInput("");
    m_ppDirectionMapper[i]->GetTextProperty()->SetFontSize(12);
    m_ppDirectionMapper[i]->GetTextProperty()->SetFontFamilyToArial();
    m_ppDirectionMapper[i]->GetTextProperty()->BoldOn();
    m_ppDirectionMapper[i]->GetTextProperty()->ItalicOff();
    m_ppDirectionMapper[i]->GetTextProperty()->ShadowOff();
    m_ppDirectionMapper[i]->GetTextProperty()->SetJustificationToCentered();
    m_ppDirectionMapper[i]->GetTextProperty()->SetVerticalJustificationToCentered();
    m_ppDirectionMapper[i]->GetTextProperty()->SetLineSpacing(0.6);
    
    m_ppDirectionActor[i] = vtkActor2D::New();
    m_ppDirectionActor[i]->SetMapper(m_ppDirectionMapper[i]);
    m_ppDirectionActor[i]->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
    m_ppDirectionActor[i]->GetProperty()->SetColor(1, 0, 0);
  }

  m_ppDirectionActor[0]->GetPositionCoordinate()->SetValue(0.5, 0.95);
  m_ppDirectionActor[1]->GetPositionCoordinate()->SetValue(0.95, 0.5);
  m_ppDirectionActor[2]->GetPositionCoordinate()->SetValue(0.5, 0.05);
  m_ppDirectionActor[3]->GetPositionCoordinate()->SetValue(0.05, 0.5);

  for (i = 0; i < 9; i++)
  {
    m_ppInfoMapper[i] = vtkTextMapper::New();
    m_ppInfoMapper[i]->SetInput("");
    m_ppInfoMapper[i]->GetTextProperty()->SetFontSize(9);
    m_ppInfoMapper[i]->GetTextProperty()->SetFontFamilyToArial();
    m_ppInfoMapper[i]->GetTextProperty()->BoldOn();
    m_ppInfoMapper[i]->GetTextProperty()->ItalicOff();
    m_ppInfoMapper[i]->GetTextProperty()->ShadowOff();
    m_ppInfoMapper[i]->GetTextProperty()->SetJustificationToCentered();
    m_ppInfoMapper[i]->GetTextProperty()->SetVerticalJustificationToCentered();
    m_ppInfoMapper[i]->GetTextProperty()->SetLineSpacing(0.6);
    
    m_ppInfoActor[i] = vtkActor2D::New();
    m_ppInfoActor[i]->SetMapper(m_ppInfoMapper[i]);
    m_ppInfoActor[i]->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
    m_ppInfoActor[i]->GetProperty()->SetColor(0, 0, 1);
  }

  m_ppInfoMapper[0]->GetTextProperty()->SetJustificationToRight();
  m_ppInfoMapper[1]->GetTextProperty()->SetJustificationToRight();
  m_ppInfoMapper[2]->GetTextProperty()->SetJustificationToRight();
  m_ppInfoMapper[3]->GetTextProperty()->SetJustificationToRight();
  m_ppInfoMapper[4]->GetTextProperty()->SetJustificationToRight();

  m_ppInfoMapper[5]->GetTextProperty()->SetJustificationToLeft();
  m_ppInfoMapper[6]->GetTextProperty()->SetJustificationToLeft();
  m_ppInfoMapper[7]->GetTextProperty()->SetJustificationToLeft();
  m_ppInfoMapper[8]->GetTextProperty()->SetJustificationToLeft();
  
  m_ppInfoActor[0]->GetPositionCoordinate()->SetValue(0.98, 0.95);
  m_ppInfoActor[1]->GetPositionCoordinate()->SetValue(0.98, 0.90);
  m_ppInfoActor[2]->GetPositionCoordinate()->SetValue(0.98, 0.85);
  m_ppInfoActor[3]->GetPositionCoordinate()->SetValue(0.98, 0.80);
  m_ppInfoActor[4]->GetPositionCoordinate()->SetValue(0.98, 0.75);

  m_ppInfoActor[5]->GetPositionCoordinate()->SetValue(0.02, 0.95);
  m_ppInfoActor[6]->GetPositionCoordinate()->SetValue(0.02, 0.90);
  m_ppInfoActor[7]->GetPositionCoordinate()->SetValue(0.02, 0.85);
  m_ppInfoActor[8]->GetPositionCoordinate()->SetValue(0.02, 0.80);

}

Annotation::~Annotation()
{
  int i;

  for (i = 0; i < 4; i++)
  {
    if (m_ppDirectionMapper[i])
    {
      m_ppDirectionMapper[i]->Delete();
    }
    if (m_ppDirectionActor[i])
    {
      m_ppDirectionActor[i]->Delete();
    }
  }

  for (i = 0; i < 9; i++)
  {
    if (m_ppInfoMapper[i])
    {
      m_ppInfoMapper[i]->Delete();
    }
    if (m_ppInfoActor[i])
    {
      m_ppInfoActor[i]->Delete();
    }
  }
}

void Annotation::SetImageOrientation(int i)
{
  switch (i)
  {
  case 0: // Axial
    m_ppDirectionMapper[0]->SetInput("A");
    m_ppDirectionMapper[1]->SetInput("L");
    m_ppDirectionMapper[2]->SetInput("P");
    m_ppDirectionMapper[3]->SetInput("R");
    break;
  case 1: // Coronal
    m_ppDirectionMapper[0]->SetInput("S");
    m_ppDirectionMapper[1]->SetInput("L");
    m_ppDirectionMapper[2]->SetInput("I");
    m_ppDirectionMapper[3]->SetInput("R");
    break;
  case 2: // Sagittal
    m_ppDirectionMapper[0]->SetInput("S");
    m_ppDirectionMapper[1]->SetInput("P");
    m_ppDirectionMapper[2]->SetInput("I");
    m_ppDirectionMapper[3]->SetInput("A");
    break;
  }
}

void Annotation::ShowDirection(int s)
{
  unsigned int i;

  switch (s)
  {
  case 0:
    for (i = 0; i < 4; i++)
    {
      m_ppDirectionActor[i]->VisibilityOff();
    }
    break;
  case 1:
    for (i = 0; i < 4; i++)
    {
      m_ppDirectionActor[i]->VisibilityOn();
    }
    break;
  }
}

void Annotation::ShowInfo(int s)
{
  unsigned int i;

  switch (s)
  {
  case 0:
    for (i = 0; i < 9; i++)
    {
      m_ppInfoActor[i]->VisibilityOff();
    }
    break;
  case 1:
    for (i = 0; i < 9; i++)
    {
      m_ppInfoActor[i]->VisibilityOn();
    }
    break;
  }
}

vtkProp* Annotation::GetAnnotationActor()
{
  return NULL;
}

void Annotation::AddActorTo(vtkRenderer *pRender)
{
  unsigned int i;

  for (i = 0; i < 4; i++)
  {
    pRender->AddActor(m_ppDirectionActor[i]);
  }  

  for (i = 0; i < 9; i++)
  {
    pRender->AddActor(m_ppInfoActor[i]);
  }  
}

void Annotation::SetPatientID(char* s)
{
  strcpy(m_PatientID, s);
  m_ppInfoMapper[0]->SetInput(s);
}

void Annotation::SetPatientName(char* s)
{
  strcpy(m_PatientName, s);
  m_ppInfoMapper[1]->SetInput(s);
}

void Annotation::SetPatientSex(char* s)
{
  strcpy(m_PatientSex, s);
  m_ppInfoMapper[2]->SetInput(s);
}

void Annotation::SetPatientAge(char* s)
{
  strcpy(m_PatientAge, s);
  m_ppInfoMapper[3]->SetInput(s);
}

void Annotation::SetStudyDate(char* s)
{
  strcpy(m_StudyDate, s);
  m_ppInfoMapper[4]->SetInput(s);
}

void Annotation::SetModality(char* s)
{
  strcpy(m_Modality, s);
  m_ppInfoMapper[5]->SetInput(s);
}

void Annotation::SetManufacturer(char* s)
{
  strcpy(m_Manufacturer, s);
  m_ppInfoMapper[6]->SetInput(s);
}

void Annotation::SetInstitution(char* s)
{
  strcpy(m_Institution, s);
  m_ppInfoMapper[7]->SetInput(s);
}

void Annotation::SetModel(char* s)
{
  strcpy(m_Model, s);
  m_ppInfoMapper[8]->SetInput(s);
}

}


