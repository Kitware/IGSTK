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

	void SetDestinationFiducial( unsigned int index, double posx, double posy, double posz );

	void SetDestinationFiducial( unsigned int index, double *position );
	
    void SetSourceFiducial( unsigned int index, double posx, double posy, double posz );

    void SetSourceFiducial( unsigned int index, double *position );

	unsigned int GetNumberOfFiducials( void );

	void FromDestinationSpaceToSourceSpace(double *input, double *output);

	void FromSourceSpaceToDestinationSpace(double *input, double *output);

	void PrintDetails( void );

	double GetRMSError( void );

	bool AreSourceFiducialsInitialized( void );

	bool AreDestinationFiducialsInitialized( void );

public:

	unsigned int		m_NumberOfFiducials;

	vtkPoints			* m_DestinationFiducialArray;

	vtkPoints			* m_SourceFiducialArray;

	std::vector<bool>		m_DestinationFiducialInitialized, m_SourceFiducialInitialized;

	vtkLandmarkTransform * m_LandmarkTransform;
};




