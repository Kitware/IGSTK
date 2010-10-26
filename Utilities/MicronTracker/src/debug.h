/******************************************************************************
 * Debug.h
 *
 * Common debugging macros.  Many of these macros are similar to those provided
 * by MFC and are designed to allow for "MFC neutral" code (code than can be
 * compiled with or without MFC support).  Other macros add even more debugging
 * facilities.
 *
 * Description of available macros:
 *
 * ASSERT      : Identical to the MFC macro of the same name.
 * VERIFY      : Identical to the MFC macro of the same name.
 * TRACE      : Identical to the MFC macro of the same name.
 * INFO        : Similar to TRACE but includes a preamble specifying the
 *            file name and line number where the INFO is called as
 *            well as forces a line break at the end.
 * DBG        : Code contained within this macro is included only in
 *            _DEBUG builds.
 * BREAK      : Forces a break in _DEBUG builds via DebugBreak.
 * DECL_DOG_TAG    : Declares a "dog tag" within a class definition (see the
 *            discussion on dog tags below) in _DEBUG builds.
 * CHECK_DOG_TAG  : Checks the validity of a "dog tag" in _DEBUG builds.
 *
 *
 * Dog Tags
 *
 * Dog tags are a technique that can be used to verify that an object has
 * not been stepped on by a memory overrun.  To use them you simply use the
 * DECL_DOG_TAG macro to declare a dog tag at the beginning and ending of
 * the class definition.  After this, you can check the validity of an
 * object by using the CHECK_DOG_TAG macro.  For example:
 *
 * class MyClass
 * {
 * public:
 *    DECL_DOG_TAG(tagBegin);
 *    // code removed for brevity...
 *    DECL_DOG_TAG(tagEnd);
 * };
 *
 * and later:
 *
 *    MyClass obj;
 *    // code removed for brevity...
 *    CHECK_DOG_TAG(obj.tagBegin);
 *    CHECK_DOG_TAG(obj.tagEnd);
 *
 *****************************************************************************/

#if !defined(_DEBUG_H_0DBFB267_INCLUDED_)
#define _DEBUG_H_0DBFB267_INCLUDED_

#if WIN32

#if _MSC_VER >= 1000
#pragma once
#endif  // _MSC_VER >= 1000

#ifndef  _MFC_VER

#include <stdio.h>
#include <crtdbg.h>

#define  ASSERT  _ASSERT

#ifdef  _DEBUG
#define  VERIFY(f)  ASSERT(f)

inline void _cdecl Trace0DBFB266_B244_11D3_A459_000629B2F85(char* lpszFormat, ...)
{
  va_list args;
  va_start(args, lpszFormat);

  int nBuf;
  char szBuffer[512];

  nBuf = _vsnprintf(szBuffer, sizeof(szBuffer) / sizeof(WCHAR), lpszFormat, args);
  ASSERT(nBuf < sizeof(szBuffer));//Output truncated as it was > sizeof(szBuffer)

  OutputDebugStringA(szBuffer);
  va_end(args);
}

#define  TRACE  Trace0DBFB266_B244_11D3_A459_000629B2F85

#else  // !_DEBUG

#define  VERIFY(f)  ((void)(f))
#define TRACE    1 ? (void)0 : Trace0DBFB266_B244_11D3_A459_000629B2F85

#endif  // _DEBUG

#endif  // _MFC_VER

#ifdef  _DEBUG

#define  DBG(f)  (f)
#define  BREAK()  DebugBreak()
#define INFO(f)  TRACE("%s (%d): ", __FILE__, __LINE__); TRACE(f); TRACE("\n")

class CDogTag
{
public:
  CDogTag() { _this = this; }
  CDogTag(const CDogTag& copy) { _this = this; ASSERT(copy.IsValid()); }
  ~CDogTag() { ASSERT(IsValid()); _this = 0; }

  CDogTag& operator=(const CDogTag& rhs)
    { ASSERT(IsValid() && rhs.IsValid()); return *this; }

  bool IsValid() const { return _this == this; }

private:
   const CDogTag *_this;
};

#define DECL_DOG_TAG(dogTag) CDogTag dogTag;
#define CHECK_DOG_TAG(dogTag) ASSERT((dogTag).IsValid());

#else  // !_DEBUG

#define  DBG(f)
#define  BREAK()
#define INFO(f)

#define DECL_DOG_TAG(dogTag)
#define CHECK_DOG_TAG(dogTag) ((void)0)

#endif  // _DEBUG
#else // WIN32
#define  DBG(f)
#define  BREAK()
#define INFO(f)

#define DECL_DOG_TAG(dogTag)
#define CHECK_DOG_TAG(dogTag) ((void)0)
#define  VERIFY(f)  ((void)(f))
#define TRACE(f) (void)0

#endif
#endif  // !defined(_DEBUG_H_0DBFB267_B244_11D3_A459_000629B2F85_INCLUDED_)
