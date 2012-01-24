/*************************************************************************
*   NDTYPES.H                                                            *
*************************************************************************/
#ifndef _NDTYPES_INCLUDED_H
#define _NDTYPES_INCLUDED_H

#include <ndhost.h>


typedef unsigned char       boolean;

#if !defined( AMIGA ) & !defined( __WINDOWS_H )
typedef char    BYTE;
#endif

#ifndef FALSE
#define FALSE   0
#define TRUE    1
#endif

#define ERR     (-1)

/*
 * EC-010340 - Moved these macros to ndmath.h
 *
#ifndef PI
#define PI      (RealType)3.1415926535897932384626433832795
#endif
#define TWO_PI  (RealType)6.2831853071795864769252867665590
#define RADIANS (RealType)57.295779513082320876798154814105
*/

#define BAD_FLOAT    (float)-3.697314E28
#define MAX_NEGATIVE (float)-3.0E28

#ifndef REAL64
typedef  float          RealType;
#define NDFS(fn)        fn
#else
typedef  double         RealType;
#define NDFS(fn)        fn##_d
#endif

/*
 * SCO-98-0020: removed defines for fabsf and sqrtf
 *
#ifndef SGI
 */
#if 0
#define fabsf( x ) (RealType)fabs( x )
#define sqrtf( x ) (RealType)sqrt( x )
#endif

struct ComplexStruct
{
    double  re;
    double  im;
};

/*
 * Structures originally from eulerd.h.
 */
typedef struct RotationStruct
{
    float   roll;           /* rotation about the object's z-axis */
    float   pitch;          /* rotation about the object's y-axis */
    float   yaw;            /* rotation about the object's x-axis */
} RotationAngleType;

typedef struct Position3dStruct
{
    float   x;
    float   y;
    float   z;
} Position3d;

typedef struct TransformationStruct
{
    RotationAngleType   rotation;
    Position3d translation;
} transformation;

typedef float
    RotationMatrixType[ 3 ][ 3 ];

typedef struct RotationTransformationStruct
{
    RotationMatrixType      matrix;
    Position3d              translation;
} RotationTransformation;

/*
 * Structure defined in quatern.h.
 */
typedef struct QuatRotationStruct
{
    float   q0;
    float   qx;
    float   qy;
    float   qz;
} QuatRotation;

typedef struct QuatTransformationStruct
{
    QuatRotation     rotation;
    Position3d       translation;
} QuatTransformation;

/*
 * EC-010340 - Added PhiThetaRotation and PhiThetaTransformation
 */
typedef struct PhiThetaRotationStruct
{
    float phi;           /* azimuthal rotation about the z-axis */
    float theta;         /* angle from the z-axis */
    float gamma;         /* optional twist angle about the vector specified by phi
                              and theta (for 6D) */
} PhiThetaRotation;

typedef struct PhiThetaTransformationStruct
{
    PhiThetaRotation rotation;
    Position3d       translation;
} PhiThetaTransformation;

typedef struct RotationStruct_d
{
    double   roll;           /* rotation about the object's z-axis */
    double   pitch;          /* rotation about the object's y-axis */
    double   yaw;            /* rotation about the object's x-axis */
} rotation_d;

typedef struct Position3dStruct_d
{
    double   x;
    double   y;
    double   z;
} Position3d_d;

typedef struct TransformationStruct_d
{
    rotation_d   rotation;
    Position3d_d translation;
} transformation_d;

typedef double
    RotationMatrixType_d[ 3 ][ 3 ];

typedef struct RotationTransformationStruct_d
{
    RotationMatrixType_d      matrix;
    Position3d_d              translation;
} RotationTransformation_d;

/*
 * Structure defined in quatern.h.
 */
typedef struct QuatRotationStruct_d
{
    double   q0;
    double   qx;
    double   qy;
    double   qz;
} QuatRotation_d;

typedef struct QuatTransformationStruct_d
{
    QuatRotation_d     rotation;
    Position3d_d       translation;
} QuatTransformation_d;

typedef struct PhiThetaRotationStruct_d
{
    double phi;
    double theta;
    double gamma;

} PhiThetaRotation_d;

typedef struct PhiThetaTransformationStruct_d
{
    PhiThetaRotation_d rotation;
    Position3d_d       translation;
} PhiThetaTransformation_d;

#define VIEW_NAME_LENGTH        12
#define VIEW_NAME_SPACE         13
#define MARKER_NAME_LENGTH      12
#define MARKER_NAME_SPACE       13
struct data3DStruct
{
    unsigned        NumRigidViews;
    char            ObjectName[ 80];
    unsigned        RigidMarkers;
    unsigned char   *RigidViews;
    Position3d      *RigidPositions;
    char            (*ViewNames)[VIEW_NAME_SPACE];
    char            (*RigidMarkerNames)[MARKER_NAME_SPACE];
    unsigned        ImagMarkers;
    Position3d      *ImagPositions;
    char            (*ImagMarkerNames)[MARKER_NAME_SPACE];
    unsigned        nNormalMarkers;
    Position3d      *pNormals;
    float           fMaxSensorError;
    float           fMax3dError;
    int             nMarkerAngle;
    float           fMax3dRmsError;
    float           fMaxSensorRmsError;
    int             nMinimumMarkers;
    int             nFlags;
    float           fMinDist1;
    float           fMinDist2;
    float           fMinDist3;
};


#ifdef __cplusplus
#define CPLUSPLUS( x ) x
#else
#define CPLUSPLUS( x )
#endif
#ifdef __cplusplus
#define CPLUSPLUS_START extern "C" {
#define CPLUSPLUS_END }
#else
#define CPLUSPLUS_START
#define CPLUSPLUS_END

#endif



#endif /* _INCLUDE_NDPACK_H */
