
#include "FiducialRegistration.h"
#include <math.h>
#include <iostream>

FiducialRegistration::FiducialRegistration()
{
	this->m_NumberOfFiducials = 0;
	this->m_DestinationFiducialArray = vtkPoints::New();
	this->m_SourceFiducialArray = vtkPoints::New();
	this->m_LandmarkTransform = vtkLandmarkTransform::New();
	this->m_LandmarkTransform->SetSourceLandmarks(m_SourceFiducialArray);
	this->m_LandmarkTransform->SetTargetLandmarks(m_DestinationFiducialArray);
//	this->m_LandmarkTransform->SetModeToAffine();
	this->m_LandmarkTransform->SetModeToSimilarity();
}

FiducialRegistration::~FiducialRegistration()
{
	this->m_DestinationFiducialArray->Delete();
	this->m_SourceFiducialArray->Delete();
	this->m_LandmarkTransform->Delete();
}

void FiducialRegistration::DeleteFiducials()
{
	this->m_NumberOfFiducials = 0;
	//this->m_DestinationFiducialArray = vtkPoints::New();
	//this->m_SourceFiducialArray = vtkPoints::New();
	//this->m_LandmarkTransform = vtkLandmarkTransform::New();
}

void FiducialRegistration::SetNumberOfFiducials(unsigned int number_fiducials)
{
	if ( number_fiducials > 0)
	{
		this->m_NumberOfFiducials = number_fiducials;
		this->m_DestinationFiducialArray->SetNumberOfPoints(m_NumberOfFiducials);
		this->m_SourceFiducialArray->SetNumberOfPoints(m_NumberOfFiducials);
		this->m_DestinationFiducialArray->Modified();
		this->m_SourceFiducialArray->Modified();
		m_DestinationFiducialInitialized.resize(m_NumberOfFiducials);
		m_SourceFiducialInitialized.resize(m_NumberOfFiducials);
		for(int i=0; i<m_NumberOfFiducials; i++)
		{
			m_DestinationFiducialInitialized[i] = false;
			m_SourceFiducialInitialized[i] = false;
		}
	}
}

void FiducialRegistration::SetDestinationFiducial(unsigned int index, float x, float y, float z)
{
	if (index < this->m_NumberOfFiducials)
	{
		this->m_DestinationFiducialArray->SetPoint(index, x, y, z);
		this->m_DestinationFiducialArray->Modified();
		m_DestinationFiducialInitialized[index] = true;
	}
}

void FiducialRegistration::SetDestinationFiducial(unsigned int index, float *position )
{
	if (index < this->m_NumberOfFiducials)
	{
		this->m_DestinationFiducialArray->SetPoint( index, position);
		this->m_DestinationFiducialArray->Modified();
		m_DestinationFiducialInitialized[index] = true;
	}
}

void FiducialRegistration::SetSourceFiducial( unsigned int index, float x, float y,float z)
{
	if (index < this->m_NumberOfFiducials)
	{
		this->m_SourceFiducialArray->SetPoint( index, x, y, z );
		this->m_SourceFiducialArray->Modified();
		m_SourceFiducialInitialized[index] = true;
	}
}

void FiducialRegistration::SetSourceFiducial( unsigned int index, float *position )
{
	if (index < this->m_NumberOfFiducials)
	{
		this->m_SourceFiducialArray->SetPoint( index, position );
		this->m_SourceFiducialArray->Modified();
		m_SourceFiducialInitialized[index] = true;
	}
}


bool FiducialRegistration::AreSourceFiducialsInitialized( void )
{
	if ( this->m_NumberOfFiducials > 0 )
	{
		bool result = true;
		for(int i=0; i<this->m_NumberOfFiducials; i++)
		{
			result = (result && this->m_SourceFiducialInitialized[i]);
		}
		return result;
	}
	else
	{
		return false;
	}
}


bool FiducialRegistration::AreDestinationFiducialsInitialized( void )
{
	if ( this->m_NumberOfFiducials > 0 )
	{
		bool result = true;
		for(int i=0; i<this->m_NumberOfFiducials; i++)
		{
			result = (result && this->m_DestinationFiducialInitialized[i]);
		}
		return result;
	}
	else
	{
		return false;
	}
}



float FiducialRegistration::GetRMSError()
{
	if ( !(this->AreSourceFiducialsInitialized()) || !(this->AreDestinationFiducialsInitialized()))
		return -1;

	this->PrintDetails();

	m_LandmarkTransform->Update(); // Not needed

	float input[3];
	float output[3];
	float target[3];
	float dist, sum_dist;
	sum_dist = 0.0;

	for (int i=0; i<m_NumberOfFiducials; i++)
	{
		this->m_SourceFiducialArray->GetPoint(i, input);
		this->m_DestinationFiducialArray->GetPoint(i, target);
		this->FromSourceSpaceToDestinationSpace(input, output);
		//cout << "input: " << input[0] << "," << input[1] << "," << input[2] << endl;
		//cout << "output: " << output[0] << "," << output[1] << "," << output[2] << endl;
		//cout << "target: " << target[0] << "," << target[1] << "," << target[2] << endl;
		dist = ( pow(output[0]-target[0],2) + pow(output[1]-target[1],2) + pow(output[2]-target[2],2));
		//cout << "square error: " << dist << endl << endl;
		sum_dist = sum_dist + dist;
	}

	for (i=0; i<m_NumberOfFiducials; i++)
	{
		this->m_DestinationFiducialArray->GetPoint(i, input);
		this->m_SourceFiducialArray->GetPoint(i, target);
		this->FromDestinationSpaceToSourceSpace(input, output);
		//cout << "input: " << input[0] << "," << input[1] << "," << input[2] << endl;
		//cout << "output: " << output[0] << "," << output[1] << "," << output[2] << endl;
		//cout << "target: " << target[0] << "," << target[1] << "," << target[2] << endl;
		dist = ( pow(output[0]-target[0],2) + pow(output[1]-target[1],2) + pow(output[2]-target[2],2));
		//cout << "square error: " << dist << endl << endl;
		sum_dist = sum_dist + dist;
	}

	float rms_error = pow(sum_dist/(2*m_NumberOfFiducials), 0.5);
	return rms_error;
}


unsigned int FiducialRegistration::GetNumberOfFiducials( void )
{
	return m_NumberOfFiducials;
}

void FiducialRegistration::PrintDetails( void )
{
	cout << "Number of fiducials = " << m_NumberOfFiducials << endl;
	cout << "Image Fiducials at" << endl;
	float point[3];
	for(int i=0; i<m_NumberOfFiducials; i++)
	{
		this->m_DestinationFiducialArray->GetPoint(i, point);
		cout << "   Fiducial " << i << ": " << point[0] << "," << point[1] << "," << point[2] << endl;
	}
	cout << "Tracker Fiducials at" << endl;
	for(i=0; i<m_NumberOfFiducials; i++)
	{
		this->m_SourceFiducialArray->GetPoint(i, point);
		cout << "   Fiducial " << i << ": " << point[0] << "," << point[1] << "," << point[2] << endl;
	}
	cout << "Image Fiducial distances: " << endl;
	float to[3], from[3], distance;
	for(i=0; i<(m_NumberOfFiducials-1); i++)
	{
		this->m_DestinationFiducialArray->GetPoint(i, to);
		this->m_DestinationFiducialArray->GetPoint(i+1, from);
		distance = sqrt((to[0]-from[0])*(to[0]-from[0]) + (to[1]-from[1])*(to[1]-from[1]) + (to[2]-from[2])*(to[2]-from[2]));
		cout << "   From " << i << " to " << i + 1 << " = " << distance << endl;
	}
	this->m_DestinationFiducialArray->GetPoint(0, to);
	this->m_DestinationFiducialArray->GetPoint(m_NumberOfFiducials-1, from);
	distance = sqrt((to[0]-from[0])*(to[0]-from[0]) + (to[1]-from[1])*(to[1]-from[1]) + (to[2]-from[2])*(to[2]-from[2]));
	cout << "   From " << 0 << " to " << m_NumberOfFiducials-1 << " = " << distance << endl;
	cout << "Tracker Fiducial distances: " << endl;
	for(i=0; i<(m_NumberOfFiducials-1); i++)
	{
		this->m_SourceFiducialArray->GetPoint(i, to);
		this->m_SourceFiducialArray->GetPoint(i+1, from);
		distance = sqrt((to[0]-from[0])*(to[0]-from[0]) + (to[1]-from[1])*(to[1]-from[1]) + (to[2]-from[2])*(to[2]-from[2]));
		cout << "   From " << i << " to " << i + 1 << " = " << distance << endl;
	}
	this->m_SourceFiducialArray->GetPoint(0, to);
	this->m_SourceFiducialArray->GetPoint(m_NumberOfFiducials-1, from);
	distance = sqrt((to[0]-from[0])*(to[0]-from[0]) + (to[1]-from[1])*(to[1]-from[1]) + (to[2]-from[2])*(to[2]-from[2]));
	cout << "   From " << 0 << " to " << m_NumberOfFiducials-1 << " = " << distance << endl;

	cout << "The LandMarkTransform Matrix is " << endl;
	this->m_LandmarkTransform->GetMatrix()->Print(cout);
}

void FiducialRegistration::FromDestinationSpaceToSourceSpace(float *input, float *output)
{
	this->m_LandmarkTransform->Inverse();
	this->m_LandmarkTransform->TransformPoint(input, output);
	this->m_LandmarkTransform->Inverse();
}

void FiducialRegistration::FromSourceSpaceToDestinationSpace(float *input, float *output)
{
	this->m_LandmarkTransform->TransformPoint(input, output);
}
