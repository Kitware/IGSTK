/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    itkPETDicomImageReader.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkPETDicomImageReader_txx
#define __itkPETDicomImageReader_txx

#include "itkPETDicomImageReader.h"

#include "itksys/SystemTools.hxx"
#include "itksys/Directory.hxx"

#include "gdcmFile.h"
#include "gdcmGlobal.h"
#include "gdcmUtil.h"
#include "gdcmValEntry.h"
#include "gdcmBinEntry.h"
#include "gdcmSeqEntry.h"
#include "gdcmSQItem.h"

namespace itk
{ 

/** Constructor */
template <class TImageType>
PETDicomImageReader<TImageType>::PETDicomImageReader() 
{ 
  // Create the DICOM GDCM file reader
  m_FileNames = itk::GDCMSeriesFileNames::New();
  m_FileNames->SetRecursive(false);
  m_FileNames->SetGlobalWarningDisplay(this->GetGlobalWarningDisplay());

  // add more criteria to distinguish between different series
  m_FileNames->SetUseSeriesDetails( true );

  // add acquisition number
  // Commented out by Patrick Cheng,
  // Note: This is too restrictive, it fails to load a full 
  // series in some cases.
  // m_FileNames->AddSeriesRestriction( "0020|0012" );

  m_ImageIO = itk::GDCMImageIO::New();
  m_ImageIO->SetGlobalWarningDisplay(this->GetGlobalWarningDisplay());
  m_ImageSeriesReader = ImageSeriesReaderType::New();
  m_ImageSeriesReader->SetImageIO( m_ImageIO );

  // In the case we only have one file we create a standard file writer
  m_ImageFileReader = ImageReaderType::New();
  m_ImageFileReader->SetImageIO( m_ImageIO );

} 

/** Destructor */
template< class TImageType>
PETDicomImageReader<TImageType>::~PETDicomImageReader()  
{

}

template< class TImageType>
void PETDicomImageReader<TImageType>
::SetDirectory( const DirectoryNameType & directory )
{
  m_ImageDirectoryName = directory;
 
  if( directory.empty() )
    {
    std::cerr << "Directory name is empty" << std::endl;
    return;
    }

  if( !itksys::SystemTools::FileExists( directory.c_str() ) )
    {
    std::cerr << "Directory does not exist" << std::endl;
    return;
    }

  if( !itksys::SystemTools::FileIsDirectory(  directory.c_str() ))
    {
    std::cerr << "Directory name is not a directory " << std::endl;
    return;
    }

  // Attempt to load the directory listing.
  itksys::Directory directoryClass;
  directoryClass.Load( directory.c_str() );

  // To count for  "." and ".." and at least two dicom slices 
  if( directoryClass.GetNumberOfFiles() < 4 ) 
    {
    return;
    }
}

template < class TImageType >
void PETDicomImageReader<TImageType>::ReadImage()
{
  m_FileNames->SetInputDirectory( m_ImageDirectoryName );
 
  const std::vector< std::string > & seriesUID = 
                                               m_FileNames -> GetSeriesUIDs();
   
  std::vector< std::string >::const_iterator iter = seriesUID.begin();
  
  for (; iter != seriesUID.end(); iter++)
    {
    std::cout<< "SeriesUID" << (*iter) << "\n";
    }
              
  if ( seriesUID.empty() ) 
    {
    return;
    } 
 
  m_ImageSeriesReader->SetFileNames( m_FileNames->GetFileNames( 
                                                 seriesUID.front().c_str() )  );
  
  try
    {
    m_ImageSeriesReader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown  " << excp.GetDescription() << std::endl;
    return;
    }
}

/*
SOME NOTES on SUV and parameters of interest:

This is the first-pass implementation we'll make:

Standardized uptake value, SUV, (also referred to as the dose uptake ratio, DUR) is a widely used, simple PET quantifier, calculated as a ratio of tissue radioactivity concentration (e.g. in units kBq/ml) at time T, CPET(T) and injected dose (e.g. in units MBq) at the time of injection divided by body weight (e.g. in units kg).

SUVbw = CPET(T) / (Injected dose / Patient's weight)

Instead of body weight, the injected dose may also be corrected by the lean body mass, or body surface area (BSA) (Kim et al., 1994). Verbraecken et al. (2006) review the different formulas for calculating the BSA.

SUVbsa= CPET(T) / (Injected dose / BSA)

If the above mentioned units are used, the unit of SUV will be g/ml.

===

Later, we can try a more careful computation that includes decay correction:

Most PET systems record their pixels in units of activity concentration (MBq/ml) (once Rescale Slope has been applied, and the units are specified in the Units attribute).

To compute SUVbw, for example, it is necessary to apply the decay formula and account for the patient's weight. For that to be possible, during de-identification, the Patient's Weight must not have been removed, and even though dates may have been removed, it is important not to remove the times, since the difference between the time of injection and the acquisition time is what is important.

In particular, DO NOT REMOVE THE FOLLOWING DICOM TAGS:
Radiopharmaceutical Start Time (0018,1072) Decay Correction (0054,1102) Decay Factor (0054,1321) Frame Reference Time (0054,1300) Radionuclide Half Life (0018,1075) Series Time (0008,0031) Patient's Weight (0010,1030)

Note that to calculate other common SUV values like SUVlbm and SUVbsa, you also need to retain:
Patient's Sex (0010,0040)
Patient's Size (0010,1020)

If there is a strong need to remove times from an identity leakage perspective, then one can normalize all times to some epoch, but it has to be done consistently across all images in the entire study (preferably including the CT reference images); note though, that the time of injection may be EARLIER than the Study Time, which you might assume would be the earliest, so it takes a lot of effort to get this right.

For Philips images, none of this applies, and the images are in COUNTS and the private tag (7053,1000) SUV Factor must be used.
To calculate the SUV of a particular pixel, you just have to calculate [pixel _value * tag 7053|1000 ]

The tag 7053|1000 is a number (double) taking into account the patient's weight, the injection quantity.
We get the tag from the original file with:

double suv;
itk::ExposeMetaData<double>(*dictionary[i], "7053|1000", suv);

*/

template <class TImageType>
int 
PETDicomImageReader<TImageType>::
GetParametersFromDicomHeader( )
{
  const std::vector< std::string > & seriesUID = 
                                               m_FileNames -> GetSeriesUIDs();
 
 
  const typename ImageSeriesReaderType::FileNamesContainer & 
    filenames = m_ImageSeriesReader->SetFileNames( m_FileNames->GetFileNames( seriesUID.front().c_str() ));
    std::string tag;
    std::string yearstr;
    std::string monthstr;
    std::string daystr;
    std::string hourstr;
    std::string minutestr;
    std::string secondstr;
    int len;
    
// Nuclear Medicine DICOM info:
/*
    0054,0016  Radiopharmaceutical Information Sequence:
    0018,1072  Radionuclide Start Time: 090748.000000
    0018,1074  Radionuclide Total Dose: 370500000
    0018,1075  Radionuclide Half Life: 6586.2
    0018,1076  Radionuclide Positron Fraction: 0
*/
    int parsingDICOM = 0;
    gdcm::File *f = new gdcm::File();
    if ( f != NULL )
      {
      const char *fn = filenames[0].c_str();
      f->SetFileName( fn );
      //bool res = f->Load();   // FIXME: commented out for now to avoid compile warnings
      f->Load();   // FIXME: handle res

      gdcm::SeqEntry *seq = f->GetSeqEntry(0x0054,0x0016);
      if ( seq != NULL )
        {
        parsingDICOM = 1;
        gdcm::SQItem *sqItem = seq->GetFirstSQItem();
        while ( sqItem )
          {
          //---
          //--- Radiopharmaceutical Start Time
          tag.clear();
          tag = sqItem->GetEntryValue(0x0018,0x1072);
          //--- expect A string of characters of the format hhmmss.frac;
          //---where hh contains hours (range "00" - "23"), mm contains minutes
          //---(range "00" - "59"), ss contains seconds (range "00" - "59"), and frac
          //---contains a fractional part of a second as small as 1 millionth of a
          //---second (range "000000" - "999999"). A 24 hour clock is assumed.
          //---Midnight can be represented by only "0000" since "2400" would
          //---violate the hour range. The string may be padded with trailing
          //---spaces. Leading and embedded spaces are not allowed. One
          //---or more of the components mm, ss, or frac may be unspecified
          //---as long as every component to the right of an unspecified
          //---component is also unspecified. If frac is unspecified the preceding "."
          //---may not be included. Frac shall be held to six decimal places or
          //---less to ensure its format conforms to the ANSI 
          //---Examples -
          //---1. "070907.0705" represents a time of 7 hours, 9 minutes and 7.0705 seconds.
          //---2. "1010" represents a time of 10 hours, and 10 minutes.
          //---3. "021" is an invalid value. 
          if ( tag.c_str() == NULL || *(tag.c_str()) == '\0' )
            {
             this->SetRadiopharmaceuticalStartTime ("no value found");
            }
          else
            {
            len = tag.length();
            hourstr.clear();
            minutestr.clear();
            secondstr.clear();
            if ( len >= 2 )
              {
              hourstr = tag.substr(0, 2);
              }
            else
              {
              hourstr = "00";
              }
            if ( len >= 4 )
              {
              minutestr = tag.substr(2, 2);
              }
            else
              {
              minutestr = "00";
              }
            if ( len >= 6 )
              {
              secondstr = tag.substr(4);
              }
            else
              {
              secondstr = "00";
              }
            tag.clear();
            tag = hourstr.c_str();
            tag += ":";
            tag += minutestr.c_str();
            tag += ":";
            tag += secondstr.c_str();
            this->SetRadiopharmaceuticalStartTime( tag.c_str() );
            }

          //---
          //--- Radionuclide Total Dose 
          tag.clear();
          tag = sqItem->GetEntryValue(0x0018,0x1074);
          if ( tag.c_str() == NULL || *(tag.c_str()) == '\0' )
            {
            this->SetInjectedDose( 0.0 );
            }
          else
            {
            this->SetInjectedDose( atof ( tag.c_str() ) );
            }


          //---
          //--- RadionuclideHalfLife
          tag.clear();
          tag = sqItem->GetEntryValue(0x0018,0x1075);
          //--- Expect a Decimal String
          //--- A string of characters representing either
          //--- a fixed point number or a floating point number.
          //--- A fixed point number shall contain only the characters 0-9
          //--- with an optional leading "+" or "-" and an optional "." to mark
          //--- the decimal point. A floating point number shall be conveyed
          //--- as defined in ANSI X3.9, with an "E" or "e" to indicate the start
          //--- of the exponent. Decimal Strings may be padded with leading
          //--- or trailing spaces. Embedded spaces are not allowed. 
          if ( tag.c_str() == NULL || *(tag.c_str()) == '\0' )
            {
            this->SetRadionuclideHalfLife( "no value found" );
            }
          else
            {
            this->SetRadionuclideHalfLife(  tag.c_str() );
            }

          //---
          //---Radionuclide Positron Fraction
          tag.clear();
          tag = sqItem->GetEntryValue(0x0018,0x1076);
          //--- not currently using this one?

          sqItem = seq->GetNextSQItem();
          }

        //--
        //--- UNITS: something like BQML:
        //--- CNTS, NONE, CM2, PCNT, CPS, BQML,
        //--- MGMINML, UMOLMINML, MLMING, MLG,
        //--- 1CM, UMOLML, PROPCNTS, PROPCPS,
        //--- MLMINML, MLML, GML, STDDEV      
        //---
        tag.clear();
        tag = f->GetEntryValue (0x0054,0x1001);
        if ( tag.c_str() != NULL && strcmp(tag.c_str(), "" ) )
          {
          //--- I think these are piled together. MBq ml... search for all.
          std::string units = tag.c_str();
          if ( ( units.find ("BQML") != std::string::npos) ||
               ( units.find ("BQML") != std::string::npos) )
            {
            this->SetDoseRadioactivityUnits ("Bq");
            this->SetTissueRadioactivityUnits ("Bq");
            }
          else if ( ( units.find ("MBq") != std::string::npos) ||
                    ( units.find ("MBQ") != std::string::npos) )
            {
            this->SetDoseRadioactivityUnits ("MBq");
            this->SetTissueRadioactivityUnits ("MBq");
            }
          else if ( (units.find ("kBq") != std::string::npos) ||
                    (units.find ("kBQ") != std::string::npos) ||
                    (units.find ("KBQ") != std::string::npos) )
            {
            this->SetDoseRadioactivityUnits ("kBq");
            this->SetTissueRadioactivityUnits ("kBq");
            }
          else if ( (units.find ("mBq") != std::string::npos) ||
                    (units.find ("mBQ") != std::string::npos) )
            {
            this->SetDoseRadioactivityUnits ("mBq");
            this->SetTissueRadioactivityUnits ("mBq");
            }
          else if ( (units.find ("uBq") != std::string::npos) ||
                    (units.find ("uBQ") != std::string::npos) )
            {
            this->SetDoseRadioactivityUnits ("uBq");
            this->SetTissueRadioactivityUnits ("uBq");
            }
          else if ( (units.find ("Bq") != std::string::npos) ||
                    (units.find ("BQ") != std::string::npos) )
            {
            this->SetDoseRadioactivityUnits ("Bq");
            this->SetTissueRadioactivityUnits ("Bq");
            }
          else if ( (units.find ("MCi") != std::string::npos) ||
                    ( units.find ("MCI") != std::string::npos) )
            {
            this->SetDoseRadioactivityUnits ("MCi");
            this->SetTissueRadioactivityUnits ("MCi");

            }
          else if ( (units.find ("kCi") != std::string::npos) ||
                    (units.find ("kCI") != std::string::npos)  ||
                    (units.find ("KCI") != std::string::npos) )                
            {
            this->SetDoseRadioactivityUnits ("kCi");
            this->SetTissueRadioactivityUnits ("kCi");
            }
          else if ( (units.find ("mCi") != std::string::npos) ||
                    (units.find ("mCI") != std::string::npos) )                
            {
            this->SetDoseRadioactivityUnits ("mCi");
            this->SetTissueRadioactivityUnits ("mCi");
            }
          else if ( (units.find ("uCi") != std::string::npos) ||
                    (units.find ("uCI") != std::string::npos) )                
            {
            this->SetDoseRadioactivityUnits ("uCi");
            this->SetTissueRadioactivityUnits ("uCi");
            }
          else if ( (units.find ("Ci") != std::string::npos) ||
                    (units.find ("CI") != std::string::npos) )                
            {
            this->SetDoseRadioactivityUnits ("Ci");
            this->SetTissueRadioactivityUnits ("Ci");
            }
          this->SetVolumeUnits ( "ml" );
          }
        else
          {
          //--- default values.
          this->SetDoseRadioactivityUnits( "MBq" );
          this->SetTissueRadioactivityUnits( "MBq" );
          this->SetVolumeUnits ( "ml");        
          }

    
        //---
        //--- DecayCorrection
        //--- Possible values are:
        //--- NONE = no decay correction
        //--- START= acquisition start time
        //--- ADMIN = radiopharmaceutical administration time
        //--- Frame Reference Time  is the time that the pixel values in the Image occurred. 
        //--- It's defined as the time offset, in msec, from the Series Reference Time.
        //--- Series Reference Time is defined by the combination of:
        //--- Series Date (0008,0021) and
        //--- Series Time (0008,0031).      
        //--- We don't pull these out now, but can if we have to.
        tag.clear();
        tag = f->GetEntryValue (0x0054,0x1102);
        if ( tag.c_str() != NULL && strcmp(tag.c_str(), "" ) )
          {
          //---A string of characters with leading or trailing spaces (20H) being non-significant. 
          this->SetDecayCorrection( tag.c_str() );
          }
        else
          {
          this->SetDecayCorrection( "no value found" );
          }

        //---
        //--- StudyDate
        this->ClearStudyDate();
        tag.clear();
        tag = f->GetEntryValue (0x0008,0x0021);
        if ( tag.c_str() != NULL && strcmp (tag.c_str(), "" ) )
          {
          //--- YYYYMMDD
          yearstr.clear();
          daystr.clear();
          monthstr.clear();
          len = tag.length();
          if ( len >= 4 )
            {
            yearstr = tag.substr(0, 4);
            this->Year = atoi(yearstr.c_str() );
            }
          else
            {
            yearstr = "????";
            this->Year = 0;
            }
          if ( len >= 6 )
            {
            monthstr = tag.substr(4, 2);
            this->Month = atoi ( monthstr.c_str() );
            }
          else
            {
            monthstr = "??";
            this->Month = 0;
            }
          if ( len >= 8 )
            {
            daystr = tag.substr (6, 2);
            this->Day = atoi ( daystr.c_str() );
            }
          else
            {
            daystr = "??";
            this->Day = 0;
            }
          tag.clear();
          tag = yearstr.c_str();
          tag += "/";
          tag += monthstr.c_str();
          tag += "/";
          tag += daystr.c_str();
          this->SetStudyDate ( tag.c_str() );
          }
        else
          {
          this->SetStudyDate ( "no value found" );
          }

        //---
        //--- PatientName
        tag.clear();
        tag = f->GetEntryValue (0x0010,0x0010);
        if ( tag.c_str() != NULL && strcmp (tag.c_str(), "" ) )
          {
          this->SetPatientName ( tag.c_str() );
          }
        else
          {
          this->SetPatientName ( "no value found");
          }

        //---
        //--- DecayFactor
        tag.clear();
        tag = f->GetEntryValue (0x0054,0x1321);
        if ( tag.c_str() != NULL && strcmp(tag.c_str(), "" ) )
          {
          //--- have to parse this out. what we have is
          //---A string of characters representing either a fixed point number or a
          //--- floating point number. A fixed point number shall contain only the
          //---characters 0-9 with an optional leading "+" or "-" and an optional "."
          //---to mark the decimal point. A floating point number shall be conveyed
          //---as defined in ANSI X3.9, with an "E" or "e" to indicate the start of the
          //---exponent. Decimal Strings may be padded with leading or trailing spaces.
          //---Embedded spaces are not allowed. or maybe atof does it already...
          this->SetDecayFactor(  tag.c_str()  );
          }
        else
          {
          this->SetDecayFactor( "no value found" );
          }

    
        //---
        //--- FrameReferenceTime
        tag.clear();
        tag = f->GetEntryValue (0x0054,0x1300);
        if ( tag.c_str() != NULL && strcmp(tag.c_str(), "" ) )
          {
          //--- The time that the pixel values in the image
          //--- occurred. Frame Reference Time is the
          //--- offset, in msec, from the Series reference
          //--- time.
          this->SetFrameReferenceTime( tag.c_str() );
          }
        else
          {
          this->SetFrameReferenceTime( "no value found" );
          }

  
        //---
        //--- SeriesTime
        tag.clear();
        tag = f->GetEntryValue (0x0008,0x0031);
        if ( tag.c_str() != NULL && strcmp(tag.c_str(), "" ) )
          {
          hourstr.clear();
          minutestr.clear();
          secondstr.clear();
          len = tag.length();
          if ( len >= 2 )
            {
            hourstr = tag.substr(0, 2);
            }
          else
            {
            hourstr = "00";
            }
          if ( len >= 4 )
            {
            minutestr = tag.substr(2, 2);
            }
          else
            {
            minutestr = "00";
            }
          if ( len >= 6 )
            {
            secondstr = tag.substr(4);
            }
          else
            {
            secondstr = "00";
            }
          tag.clear();
          tag = hourstr.c_str();
          tag += ":";
          tag += minutestr.c_str();
          tag += ":";
          tag += secondstr.c_str();
          this->SetSeriesTime( tag.c_str() );
          }
        else
          {
          this->SetSeriesTime( "no value found");
          }


        //---
        //--- PatientWeight
        tag.clear();
        tag = f->GetEntryValue (0x0010,0x1030);
        if ( tag.c_str() != NULL && strcmp(tag.c_str(), "" ) )
          {
          //--- Expect same format as RadionuclideHalfLife
          this->SetPatientWeight( atof ( tag.c_str() ) );
          this->SetWeightUnits ( "kg" );
          }
        else
          {
          this->SetPatientWeight( 0.0 );
          this->SetWeightUnits ( "" );
          }


        //---
        //--- CalibrationFactor
        tag.clear();
        tag = f->GetEntryValue (0x7053,0x1009);
        if ( tag.c_str() != NULL && strcmp(tag.c_str(), "" ) )
          {
          //--- converts counts to Bq/cc. If Units = BQML then CalibrationFactor =1 
          //--- I think we expect the same format as RadiopharmaceuticalStartTime
          this->SetCalibrationFactor(  tag.c_str() );
          }
        else
          {
          this->SetCalibrationFactor( "no value found" );
          }


        //---
        //--- PhilipsSUVFactor
        tag.clear();
        tag = f->GetEntryValue (0x7053,0x1000);
        if ( tag.c_str() != NULL && strcmp(tag.c_str(), "" ) )
          {
          //--- I think we expect the same format as RadiopharmaceuticalStartTime
          this->SetPhilipsSUVFactor(  tag.c_str() );
          }
        else
          {
          this->SetPhilipsSUVFactor( "no value found" );
          }
        }
      }
    //END TEST
    delete f;


    // check.... did we get all params we need for computation?
    if ( (parsingDICOM) &&
         (this->GetInjectedDose() != 0.0) &&
         (this->GetPatientWeight() != 0.0) &&
         (this->GetSeriesTime() != NULL) &&
         (this->GetRadiopharmaceuticalStartTime() != NULL) &&
         (this->GetRadionuclideHalfLife() != NULL) )
      {
      return 1;
      }
    else
      {
      return 0;
      }
}

/** Print Self function */
template <class TImageType>
void PETDicomImageReader<TImageType>
::PrintSelf( std::ostream& os, itk::Indent indent ) const
{
  Superclass::PrintSelf( os, indent );
}

} // end namespace igstA

#endif
