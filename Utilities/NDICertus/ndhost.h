/***************************************************************
Name:            NDHOST.H

Description:
    IBMPC version of the ndhost.h include file for the Windows
    version of the API.

Modified:

****************************************************************/

#ifndef NDHOST_H_INCLUDED
#define NDHOST_H_INCLUDED

/****************************************************************
  Defines
****************************************************************/

/*
 * The following #define statements can be used to select the version
 * Pack/UnPack routines required for the appropriate host computer.
 * If your computer is listed, change the defined constant to a 1.
 * If your computer is not listed, you will have to add the appropriate
 * defines for it.
 */

#define IBMPC           0
#define WIN16           0
#define HOST_WIN32      1
#define APOLLO          0
#define AMIGA           0
#define MACINTOSH       0
#define BSD386          0
#define SGI             0
#define SUN             0

#define NDI_LITTLE_ENDIAN
#define IEEE_FLOATS
#define BITFIELD_LTOM
#define BYTE_DEFINED
#define PACK_UNPACK

/*
 * Define NDI_DECL1 and NDI_DECL2 for accessing the 32-bit DLL.
 */

/**
#ifdef __BORLANDC__
#define NDI_DECL1       __declspec( dllimport )
#define NDI_DECL2       __stdcall
#elif _MSC_VER
#define NDI_DECL1       __declspec( dllimport )
#define NDI_DECL2       __stdcall
#define inp             _inp
#define outp            _outp
#elif __WATCOMC__
#define NDI_DECL1       __declspec( dllimport )
#define NDI_DECL2       __stdcall
#define inp             _inp
#define outp            _outp
#else
#error unknown compiler
#endif
**/

#if (defined(_WIN32) || defined(WIN32))
  #define NDI_DECL1       __declspec( dllimport )
  #define NDI_DECL2       __stdcall
  #define inp             _inp
  #define outp            _outp
#else
  #define NDI_DECL1      
  #define NDI_DECL2    
  #define inp         
  #define outp        
#endif


#define HOSTDEFINED

/****************************************************************
  Macros
****************************************************************/

/****************************************************************
 Global Structures
****************************************************************/
/****************************************************************
 External Variables
****************************************************************/

/****************************************************************
 Routine Definitions
****************************************************************/

#endif
