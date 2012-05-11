/*=========================================================================

Program:   Image Guided Surgery Software Toolkit
Module:    $RCSfile: igstkCertusTrackerConfiguration.h,v $
Language:  C++
Date:      $Date: 2012-01-27 17:48:01 $
Version:   $Revision: 1.1 $

Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __igstkCertusTrackerConfiguration_h
#define __igstkCertusTrackerConfiguration_h

#include "igstkTrackerConfiguration.h"

namespace igstk
{


/**
 * \class CertusToolConfiguration 
 * \brief This class represents the configuration data required by a Certus 
 * tool. 
 *
 * This class represents the configuration data required by a Certus tool.
 */	
class CertusToolConfiguration : public TrackerToolConfiguration
{
public:
  CertusToolConfiguration();
  CertusToolConfiguration(const CertusToolConfiguration &other);
  virtual ~CertusToolConfiguration();
 
  igstkSetMacro( RigidBodyName, std::string );
  igstkGetMacro( RigidBodyName, std::string );

  igstkSetMacro( StartMarker, std::string );
  igstkGetMacro( StartMarker, std::string );

  igstkSetMacro( NumberOfMarkers, std::string );
  igstkGetMacro( NumberOfMarkers, std::string );

   
  
  virtual std::string GetToolTypeAsString();

protected:

   std::string        m_RigidBodyName;
   std::string        m_StartMarker;
   std::string        m_NumberOfMarkers;
};


/**
 * \class CertusTrackerConfiguration This class represents the configuration 
 *        data required by the Certus tracker.
 */
class CertusTrackerConfiguration : public TrackerConfiguration
{
public:
  //standard typedefs
  igstkStandardClassBasicTraitsMacro( CertusTrackerConfiguration, 
                                      TrackerConfiguration )

  //method for creation through the object factory
  igstkNewMacro( Self );

  /**
   * Get the manufacturer specified maximal refresh rate.
   */
  virtual double GetMaximalRefreshRate();

  igstkSetMacro( SetupFile, std::string );
  igstkGetMacro( SetupFile, std::string );

  igstkSetMacro( NumberOfRigidBodies, std::string );
  igstkGetMacro( NumberOfRigidBodies, std::string );
  
protected:
  virtual void InternalAddTool( const TrackerToolConfiguration *tool, 
                                bool isReference );
  CertusTrackerConfiguration();
  virtual ~CertusTrackerConfiguration();

private:
  std::string     m_SetupFile;

  std::string     m_NumberOfRigidBodies;

  //manufacturer specified maximal refresh rate [Hz]
  static const double MAXIMAL_REFERESH_RATE;
};


} // end of name space
#endif
