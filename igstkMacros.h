
#ifndef __igstk_Macros_h__
#define __igstk_Macros_h__


#define  SetMacro(name,type) \
  virtual void Set##name (const type _arg) \
  { \
  if (this->m_##name != _arg) \
      { \
      this->m_##name = _arg; \
      } \
  } 

#define GetMacro(name,type) \
  virtual type Get##name () \
  { \
     return this->m_##name; \
  }



#endif




