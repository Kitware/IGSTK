
#ifndef _IGSTK_ANNOTATION_H_
#define _IGSTK_ANNOTATION_H_

#include "vtkActor2D.h"
#include "vtkAssembly.h"
#include "vtkProp.h"
#include "vtkRenderer.h"
#include "vtkTextMapper.h"

namespace IGSTK
{

class Annotation  
{
public:

  char m_PatientID[128];
  char m_PatientName[128];
  char m_PatientSex[128];
  char m_PatientAge[128];
  
  char m_StudyDate[128];
  char m_Modality[128];
  char m_Manufacturer[128];
  char m_Institution[128];
  char m_Model[128];

  void SetPatientID(char* s);
  void SetPatientName(char* s);
  void SetPatientSex(char* s);
  void SetPatientAge(char* s);

  void SetStudyDate(char* s);
  void SetModality(char* s);
  void SetManufacturer(char* s);
  void SetInstitution(char* s);
  void SetModel(char* s);

	void AddActorTo(vtkRenderer* pRender);
	vtkProp* GetAnnotationActor();
	void ShowDirection(int s);
  void ShowInfo(int s);
	void SetImageOrientation(int i);
  
  vtkTextMapper*	m_ppDirectionMapper[4];
		
	vtkActor2D*		m_ppDirectionActor[4];

  vtkTextMapper*	m_ppInfoMapper[9];
		
	vtkActor2D*		m_ppInfoActor[9];

  Annotation();
	
  virtual ~Annotation();

};

}
#endif 
