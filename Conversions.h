////////////////////////////////////////////////////////////////////////
// FROM NDI: Copyright information removed for easy readability
////////////////////////////////////////////////////////////////////////
#ifndef __Conversions_h__
#define __Conversions_h__


unsigned int uASCIIToHex( char szStr[], int nLen );
bool bExtractValue( char *pszVal, unsigned uLen,
				   float fDivisor, float *pfValue );

int nGetHex2(char *sz);
int nGetHex1(char *sz);
int nGetHex4(char *sz);
float fGetFloat(char *sz);
////////////////////////////////////////////////////////////////////////
typedef float RotationMatrix[3][3];

typedef struct Rotation
{
	float	m_Roll;		// Rotation about the object's Z-axis (euler angle)
	float	m_Pitch;	// Rotation about the object's Y-axis (euler angle)
	float	m_Yaw;		// Rotation about the object's X-axis (euler angle)
} Rotation;

typedef struct QuatRotation
{
	float m_Q0, m_Qx, m_Qy, m_Qz;
} QuatRotation;
			
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
void EulerAngleTrig( Rotation *pRotationAngle, Rotation *pSinAngle,
						    Rotation *pCosAngle );

void DetermineR( Rotation *pRotationAngle, RotationMatrix rotationMatrix );

void QuatToRotationMatrix( const QuatRotation *pQuatRotation, RotationMatrix rotationMatrix );

#endif



