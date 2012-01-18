/***************************************************************
Name:            NDPACK.H

Description:
    Macros and prototypes for  Packing/unpacking of data
    structures between different host platforms

Modified:
    SCO-00-0001: Added typedef for UInt32 and the related
    pack and unpack routines.
    EC-04-0121: Removed double definition of FAR and NEAR

****************************************************************/

#ifndef NDPACK_H_INCLUDED
#define NDPACK_H_INCLUDED

/*
 * There is an appropriate version of ndhost.h for each platform!
 */
#ifndef NDHOST_H_INCLUDED
#include <ndhost.h>
#endif

#ifndef HOSTDEFINED
#error You must define the type of system you are using...
#endif

/****************************************************************
  Defines
****************************************************************/

#if !(IBMPC || WIN16)
#undef FAR
#undef NEAR
#define far
#define FAR
#define near
#define NEAR
#define huge
#if !(SUN)
#define HUGE
#endif
#endif

#ifdef PACK_UNPACK
#define PC_CHAR_SIZE        1
#define PC_INT_SIZE         2
#define PC_LONG_SIZE        4
#define PC_FLOAT_SIZE       4
#define PC_DOUBLE_SIZE      8
typedef unsigned int        UInt32;
#else
#define PC_CHAR_SIZE        sizeof(char)
#define PC_INT_SIZE         sizeof(int)
#define PC_LONG_SIZE        sizeof(long)
#define PC_FLOAT_SIZE       sizeof(float)
#define PC_DOUBLE_SIZE      sizeof(double)
typedef unsigned long       UInt32;
#endif


/****************************************************************
  Macros
****************************************************************/
#ifdef  REAL64
#define PackRealType( p, q, s )     PackDouble( (p), (q), (s) )
#define UnPackRealType( p, q, s )   UnPackDouble( (p), (q), (s) )
#else
#define PackRealType( p, q, s )     PackFloat( (p), (q), (s) )
#define UnPackRealType( p, q, s )   UnPackFloat( (p), (q), (s) )
#endif  /* REAL64 */

#define UnPackUChar( p, q, s )  (*(p) = *(q), 1 )
#define UnPackChar( p, q, s )   UnPackUChar( (unsigned char *)(p), (q), (s) )
#define UnPackUnsigned(p, q, s) (*(p) = (unsigned)(q)[1] << 8 | *(q), 2 )
#define UnPackInt( p, q, s )    (*(p) = (int)(((signed char *)(q))[1]) << 8 | \
                                        *(q), 2 )
#define UnPackULong( p, q, s )  (*(p) = (unsigned long)(q)[3] << 24 |       \
                                        (unsigned long)(q)[2] << 16 |       \
                                        (unsigned long)(q)[1] << 8 | *(q), 4 )
#define UnPackLong( p, q, s )   (*(p) = (long)(((signed char *)(q))[3]) << 24 |\
                                        (unsigned long)(q)[2] << 16 |       \
                                        (unsigned long)(q)[1] << 8 | *(q), 4 )
#define UnPackUInt32( p, q, s ) (*(p) = (UInt32)(q)[3] << 24 |       \
                                        (UInt32)(q)[2] << 16 |       \
                                        (UInt32)(q)[1] << 8 | *(q), 4 )

#define PackUChar( p, q, s )    (*(q) = *(p), 1 )
#define PackChar( p, q, s )     PackUChar( (unsigned char *)(p), (q), (s) )
#define PackUnsigned( p, q, s ) ((q)[1] = (*(p) >> 8) & 0xff,               \
                                 (q)[0] = *(p) & 0xff, 2 )
#define PackInt( p, q, s )      PackUnsigned( (unsigned *)(p), (q), (s) )
#define PackULong( p, q, s )    ((q)[3] = (*(p) >> 24) & 0xff,              \
                                 (q)[2] = (*(p) >> 16) & 0xff,              \
                                 (q)[1] = (*(p) >> 8) & 0xff,               \
                                 (q)[0] = *(p) & 0xff, 4 )

#define PackLong( p, q, s )     PackULong( (unsigned long *)(p), (q), (s) )
#define PackUInt32( p, q, s )   PackULong( p, q, s )

/*
 * SCO-97-0050: Modified to support 64-bit machines.
 */
#ifdef NDI_BIG_ENDIAN
#define PackFloat( p, q, s )    ((q)[0] = ((unsigned char *)(p))[3],        \
                                 (q)[1] = ((unsigned char *)(p))[2],        \
                                 (q)[2] = ((unsigned char *)(p))[1],        \
                                 (q)[3] = ((unsigned char *)(p))[0], 4 )
#define UnPackFloat( p, q, s )  (((unsigned char *)(p))[0] = (q)[3],        \
                                 ((unsigned char *)(p))[1] = (q)[2],        \
                                 ((unsigned char *)(p))[2] = (q)[1],        \
                                 ((unsigned char *)(p))[3] = (q)[0], 4 )
#else
#define PackFloat( p, q, s )    ((q)[0] = ((unsigned char *)(p))[0],        \
                                 (q)[1] = ((unsigned char *)(p))[1],        \
                                 (q)[2] = ((unsigned char *)(p))[2],        \
                                 (q)[3] = ((unsigned char *)(p))[3], 4 )
#define UnPackFloat( p, q, s )  (((unsigned char *)(p))[0] = (q)[0],        \
                                 ((unsigned char *)(p))[1] = (q)[1],        \
                                 ((unsigned char *)(p))[2] = (q)[2],        \
                                 ((unsigned char *)(p))[3] = (q)[3], 4 )
#endif /* NDI_BIG_ENDIAN */

/****************************************************************
 Global Structures
****************************************************************/

/****************************************************************
 External Variables
****************************************************************/
extern char
    szErrOverRun[];

/****************************************************************
 Routine Definitions
****************************************************************/

CPLUSPLUS_START
void
    HandleOverrun( char *pszStr );

unsigned
    PackDouble( double *pDbl, unsigned char *pchBuff, unsigned uBufSiz ),
    UnPackDouble( double *pDbl, unsigned char *pchBuff, unsigned uBufSiz ),
    PackRotationMatrix( RotationMatrixType *p, unsigned char *pchBuff, unsigned uBuffSize ),
    UnPackRotationMatrix( RotationMatrixType *p, unsigned char *pchBuff, unsigned uBuffSize ),
    PackComplex( struct ComplexStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    UnPackComplex( struct ComplexStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    PackRotation( struct RotationStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    UnPackRotation( struct RotationStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    PackPosition3d( struct Position3dStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    UnPackPosition3d( struct Position3dStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    PackTransformation( struct TransformationStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    UnPackTransformation( struct TransformationStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    PackRotationTransformation( struct RotationTransformationStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    UnPackRotationTransformation( struct RotationTransformationStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    PackQuatRotation( struct QuatRotationStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    UnPackQuatRotation( struct QuatRotationStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    PackQuatTransformation( struct QuatTransformationStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    UnPackQuatTransformation( struct QuatTransformationStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    PackData3D( struct data3DStruct *p, unsigned char *pchBuff, unsigned uBuffSize ),
    UnPackData3D( struct data3DStruct *p, unsigned char *pchBuff, unsigned uBuffSize );

CPLUSPLUS_END

#endif
