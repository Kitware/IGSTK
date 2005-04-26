/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkFileLogOutput.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itk_FileLogOutput_h_
#define __itk_FileLogOutput_h_

#include<fstream>
#include<string>
#include"itkMacro.h"
#include"itkObject.h"
#include"itkObjectFactory.h"
#include"itkSimpleFastMutexLock.h"
#include"itkLogOutput.h"


namespace itk
{

/** \class FileLogOutput
    \brief Class FileLogOutput represents a file output stream.
    This class provides thread safety for the file output stream.
*/

class FileLogOutput : public LogOutput
{

public:

  typedef FileLogOutput Self;
  typedef LogOutput  Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  typedef std::ofstream* FilePointerType;

  itkTypeMacro(FileLogOutput, LogOutput);
  
  itkNewMacro(FileLogOutput);

  itkGetMacro(File, FilePointerType);

  /** Set file stream */
  void SetFile(FilePointerType FileStream);
  
  /** flush a buffer */
  virtual void Flush();

  /** Write to multiple outputs */
  virtual void Write(double timestamp);

  /** Write to a buffer */
  virtual void Write(std::string const &content);

  /** Write to a buffer */
  virtual void Write(std::string const &content, double timestamp);

protected:
  /** Constructor */
  FileLogOutput();

  /** Destructor */
  virtual ~FileLogOutput();

private:

  FilePointerType m_File;

  SimpleFastMutexLock m_Mutex;
};

}

#endif //__itk_FileLogOutput_h_
