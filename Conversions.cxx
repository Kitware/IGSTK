#include <stdlib.h>
#include <string.h> 
#include <math.h>
#include "Conversions.h"
/***************************************************************************
Name:               uASCIIToHex

Input Values:
	char
		szStr[]				:String of ASCII characters that make up a hex
							 integer value to be converted.
							 Note that the string may NOT be null-terminated.
	int
		nLen				:Length of input string.

Output Values:
	None.

Returned Value:
	unsigned int			:Integer equivalent of input array.

Description:
	This routine translates a character ASCII array which is
	hex to its equivalent integer value.

***************************************************************************/

#define BAD_FLOAT    (float)-3.697314E28

unsigned int uASCIIToHex( char szStr[], int nLen )
{
	char
		chTemp;
	unsigned int
		uVal;
	int
		nCnt;

	uVal = 0;
	for ( nCnt = 0; nCnt < nLen; ++nCnt )
	{
		chTemp = szStr[nCnt];
		/*
		 * Convert hex ASCII digits to values to add to total value.
		 */
		if ( (chTemp >= '0') && (chTemp <= '9') )
		{
			chTemp -= '0';
		} /* if */
		else if ( (chTemp >= 'A') && (chTemp <= 'F') )
		{
			chTemp = 0x0000000a + (chTemp - 'A');
		} /* else if */
		else if ( (chTemp >= 'a') && (chTemp <= 'f') )
		{
			chTemp = 0x0000000a + (chTemp - 'a');
		} /* else if */
		else
		{
			/*
			 * We've hit a non-hex character.
			 */
			return( 0 );
		} /* else */

		/*
		 * Shift result into position of total value.
		 */
		uVal |= (chTemp << (4 * (nLen - 1 - nCnt)));
	} /* for */

	return( uVal );
} /* uASCIIToHex */
/*****************************************************************
Routine:    bExtractValue

Inputs:	pszVal - value to be extracted, uLen - length of value
		fDivisor - how to break up information, pfValue - data

Returns:

Description: This routine breaks up the transformation into
			 there individual components

*****************************************************************/
bool bExtractValue( char *pszVal, unsigned uLen,
					float fDivisor, float *pfValue )
{
    unsigned
        i;
    char
        szBuff[ 10 ];

    *pfValue = BAD_FLOAT;

    /*
     * Make sure that the first character is either a + or -.
     */
    if( *pszVal != '-' && *pszVal != '+' )
	{
      return false;
	} /* if */

    /*
     * Copy the + or - character to the buffer
     */
    szBuff[0] = *pszVal;

    /*
     * Copy the rest of the value.  Make sure that the remainder of
     * the value string contains only digits 0 - 9.
     */
    for( i = 1; i < uLen; i++ )
    {
        if( pszVal[i] < '0' || pszVal[i] > '9' )
		{
             return false;
		} /* if */

        szBuff[i] = pszVal[i];
    } /* for */

    /*
     * Null terminate the string.
     */
    szBuff[i] = '\0';

    *pfValue = float(atof( szBuff ) / fDivisor);
    
    return true;
} /* bExtractValue */
/*****************************************************************
Routine:   nGetHex1

Inputs:		char *sz - the buffer that contains the 2 hex chars
			of information to be decoded
	
Returns:	int u - the decimal value of the decoding
	
Description:
	Converts two (2) hex characters to its decimal equivialent

*****************************************************************/
int nGetHex1(char *sz)
{
    unsigned int u;

    u=sz[0] & 0xff;

    return u;
} /* nGetHex1 */
/*****************************************************************
Routine:   nGetHex2

Inputs:		char *sz - the buffer that contains the 2 hex chars
			of information to be decoded
	
Returns:	int u - the decimal value of the decoding
	
Description:
	Converts two (2) hex characters to its decimal equivialent

*****************************************************************/
int nGetHex2(char *sz)
{
    unsigned int u;

    u=sz[0] & 0xff;
    u|=((sz[1] & 0xFF) << 8);

    return u;
} /* nGetHex2 */
/*****************************************************************
Routine:   nGetHex4

Inputs:		char *sz - the buffer that contains the 4 hex chars
			of information to be decoded
	
Returns:	int u - the decimal value of the decoding
	
Description:
	Converts two (4) hex characters to its decimal equivialent

*****************************************************************/
int nGetHex4(char *sz)
{
    unsigned int u;

    u=sz[0] & 0xff;
    u|=((sz[1] & 0xFF) << 8);
    u|=((sz[2] & 0xFF) << 16);
    u|=((sz[3] & 0xFF) << 24);

    return (int) u;
} /* nGetHex4 */
/*****************************************************************
Routine:   fGetFloat

Inputs:		char *sz - the buffer that contains the 2 hex chars
			of information to be decoded
	
Returns:	int f - the float value of the decoding
	
Description:
	Converts two (2) hex characters to its float equivialent

*****************************************************************/
float fGetFloat(char *sz)
{
    float f;
    unsigned int *pu;

    pu =((unsigned int *)&f);

    (*pu)=sz[0] & 0xff;
    (*pu)|=((sz[1] & 0xFF) << 8);
    (*pu)|=((sz[2] & 0xFF) << 16);
    (*pu)|=((sz[3] & 0xFF) << 24);

    return f;
} /* fGetFloat */
/**************************END OF FILE***************************/
void EulerAngleTrig( Rotation *pRotationAngle, Rotation *pSinAngle,
						    Rotation *pCosAngle )
{
	pSinAngle->m_Roll = float(sin( pRotationAngle->m_Roll ));
	pSinAngle->m_Pitch = float(sin( pRotationAngle->m_Pitch ));
	pSinAngle->m_Yaw = float(sin( pRotationAngle->m_Yaw ));
	pCosAngle->m_Roll = float(cos( pRotationAngle->m_Roll ));
	pCosAngle->m_Pitch = float(cos( pRotationAngle->m_Pitch ));
	pCosAngle->m_Yaw = float(cos( pRotationAngle->m_Yaw ));
}

void DetermineR( Rotation *pRotationAngle, RotationMatrix rotationMatrix )
{
	Rotation  sinAngle, cosAngle;

	EulerAngleTrig( pRotationAngle, &sinAngle, &cosAngle );

	rotationMatrix[0][0] = cosAngle.m_Roll * cosAngle.m_Pitch;
	rotationMatrix[0][1] = cosAngle.m_Roll * sinAngle.m_Pitch * sinAngle.m_Yaw
						-  sinAngle.m_Roll * cosAngle.m_Yaw;
	rotationMatrix[0][2] = cosAngle.m_Roll * sinAngle.m_Pitch * cosAngle.m_Yaw
						+  sinAngle.m_Roll * sinAngle.m_Yaw;
	rotationMatrix[1][0] = sinAngle.m_Roll * cosAngle.m_Pitch;
	rotationMatrix[1][1] = sinAngle.m_Roll * sinAngle.m_Pitch * sinAngle.m_Yaw
						+  cosAngle.m_Roll * cosAngle.m_Yaw;
	rotationMatrix[1][2] = sinAngle.m_Roll * sinAngle.m_Pitch * cosAngle.m_Yaw
						-  cosAngle.m_Roll * sinAngle.m_Yaw;
	rotationMatrix[2][0] = - sinAngle.m_Pitch;
	rotationMatrix[2][1] = cosAngle.m_Pitch * sinAngle.m_Yaw;
	rotationMatrix[2][2] = cosAngle.m_Pitch * cosAngle.m_Yaw;
}

void QuatToRotationMatrix( const QuatRotation *pQuatRotation, RotationMatrix rotationMatrix )
{
	float Q0Q0 = pQuatRotation->m_Q0 * pQuatRotation->m_Q0 ;
	float QxQx = pQuatRotation->m_Qx * pQuatRotation->m_Qx ;
	float QyQy = pQuatRotation->m_Qy * pQuatRotation->m_Qy ;
	float QzQz = pQuatRotation->m_Qz * pQuatRotation->m_Qz ;

	float Q0Qx = pQuatRotation->m_Q0 * pQuatRotation->m_Qx ;
	float Q0Qy = pQuatRotation->m_Q0 * pQuatRotation->m_Qy ;
	float Q0Qz = pQuatRotation->m_Q0 * pQuatRotation->m_Qz ;

	float QxQy = pQuatRotation->m_Qx * pQuatRotation->m_Qy ;
	float QxQz = pQuatRotation->m_Qx * pQuatRotation->m_Qz ;
	float QyQz = pQuatRotation->m_Qy * pQuatRotation->m_Qz ;

	rotationMatrix[0][0] = Q0Q0 + QxQx - QyQy - QzQz ;
	rotationMatrix[0][1] = 2.0 * ( - Q0Qz + QxQy );
	rotationMatrix[0][2] = 2.0 * (   Q0Qy + QxQz );
	rotationMatrix[1][0] = 2.0 * (   Q0Qz + QxQy );
	rotationMatrix[1][1] = Q0Q0 - QxQx + QyQy - QzQz ;
	rotationMatrix[1][2] = 2.0 * ( - Q0Qx + QyQz );
	rotationMatrix[2][0] = 2.0 * ( - Q0Qy + QxQz );
	rotationMatrix[2][1] = 2.0 * (   Q0Qx + QyQz );
	rotationMatrix[2][2] = Q0Q0 - QxQx - QyQy + QzQz ;
}

/**************************END OF FILE***************************/
