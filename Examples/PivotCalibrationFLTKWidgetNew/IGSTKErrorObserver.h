#ifndef _IGSTK_ERROR_OBSERVER_H_
#define _IGSTK_ERROR_OBSERVER_H_

/**************************************************************************
* Copyright (c) 2006
* Computer Aided Interventions and Medical Robotics (CAIMR) group,
* Imaging Science and Information Systems (ISIS) research center,
* Georgetown University.
*
* All rights reserved.
*
* Permission to use, copy, modify, distribute, and sell this software
* and its documentation for any purpose is hereby granted without fee,
* provided that:
* (1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
* (2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
* (3) The author's name may not be used to endorse or promote products derived
*     from this software without specific prior written permission.
* (4) Modified source versions must be plainly marked as such, and must not be
*     misrepresented as being the original software.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER ``AS IS'' AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
**************************************************************************/

#include <itkCommand.h>
#include <itkSmartPointer.h>
#include <igstkMacros.h>

/**
 * This class serves a similar function as the errno variable and perror error printing function.
 * It is an adaptor between the IGSTK event driven error reporting and exception throwing as
 * used in standard C++.
 *
 * After each call to RequestXXX() on a IGSTK component the user should check if an error occured,
 * using the observer's boolean Error() method, get the associated error message using the observer's 
 * GetErrorMessage() method, and then clear the error using the ClearError() method.
 *
 * @author Ziv Yaniv
 *
 */
class IGSTKErrorObserver : public itk::Command
{
public:
  typedef IGSTKErrorObserver               Self;
  typedef ::itk::Command                   Superclass;
  typedef ::itk::SmartPointer<Self>        Pointer;
  typedef ::itk::SmartPointer<const Self>  ConstPointer;

  igstkNewMacro(Self)
  igstkTypeMacro(DataAcquisitionErrorObserver,itk::Command)

  /**
   * When an error occurs in an IGSTK component it will invoke this method with the appropriate
   * error event object as a parameter.
   */
  virtual void Execute(itk::Object *caller, const itk::EventObject & event) throw (std::exception);
  /**
   * When an error occurs in an IGSTK component it will invoke this method with the appropriate
   * error event object as a parameter.
   */
  virtual void Execute(const itk::Object *caller, const itk::EventObject & event) throw (std::exception);
  /**
   * Clear the current error.
   */
  void ClearError() {this->errorOccured = false; this->errorMessage.clear();}
  /**
   * If an error occurs in one of the observed IGSTK components this method will return true.
   */
  bool Error() {return this->errorOccured;}
  /**
   * Get the error message associated with the last IGSTK error.
   */
  void GetErrorMessage(std::string &errorMessage) {errorMessage = this->errorMessage;}

protected:

  /**
   * Construct an error observer for all the possible errors that occur in IGSTK components
   * used in the DataAcqusition class.
   */
  IGSTKErrorObserver();
  virtual ~IGSTKErrorObserver(){}
private:
  bool errorOccured;
  std::string errorMessage;
  std::map<std::string,std::string> errorEvent2ErrorMessage;

               //purposely not implemented
  IGSTKErrorObserver(const Self&);
  void operator=(const Self&); 
};

#endif //_IGSTK_ERROR_OBSERVER_H_
