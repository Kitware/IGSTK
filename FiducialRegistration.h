#include "vtkPoints.h"
#include "vtkMatrix4x4.h"
#include "vtkLinearTransform.h"
#include "vtkLandmarkTransform.h"
#include <vector>
class FiducialRegistration
{
public:

	FiducialRegistration( void );

	~FiducialRegistration( void );

	void DeleteFiducials( void );

	void SetNumberOfFiducials( unsigned int number);

	void SetDestinationFiducial( unsigned int index, float posx, float posy, float posz );

	void SetDestinationFiducial( unsigned int index, float *position );
	
    void SetSourceFiducial( unsigned int index, float posx, float posy, float posz );

    void SetSourceFiducial( unsigned int index, float *position );

	unsigned int GetNumberOfFiducials( void );

	void FromDestinationSpaceToSourceSpace(float *input, float *output);

	void FromSourceSpaceToDestinationSpace(float *input, float *output);

	void PrintDetails( void );

	float GetRMSError( void );

	bool AreSourceFiducialsInitialized( void );

	bool AreDestinationFiducialsInitialized( void );

protected:

	unsigned int		m_NumberOfFiducials;

	vtkPoints			* m_DestinationFiducialArray;

	vtkPoints			* m_SourceFiducialArray;

	std::vector<bool>		m_DestinationFiducialInitialized, m_SourceFiducialInitialized;

	vtkLandmarkTransform * m_LandmarkTransform;
};