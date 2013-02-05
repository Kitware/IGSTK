/*=========================================================================

  Program:   Image Guided Surgery Software Toolkit
  Module:    igstkTestMain.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) ISC  Insight Software Consortium.  All rights reserved.
  See IGSTKCopyright.txt or http://www.igstk.org/copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// This file is used to create TestDriver executables
// These executables are able to register a function pointer to a string name
// in a lookup table.   By including this file, it creates a main function
// that calls RegisterTests() then looks up the function pointer for the test
// specified on the command line.
#include "itkWin32Header.h"
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include "itkNumericTraits.h"
#include "itkMultiThreader.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionConstIterator.h"
#include "itkSubtractImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkTestingComparisonImageFilter.h"
#include "itkImageRegion.h"
#include "itksys/SystemTools.hxx"

const unsigned int IGSTK_TEST_DIMENSION_MAX = 2;

typedef int (*MainFuncPointer)(int , char* [] );
std::map<std::string, MainFuncPointer> StringToTestFunctionMap;

#define REGISTER_TEST(test) \
extern int test(int, char* [] ); \
StringToTestFunctionMap[#test] = test

int RegressionTestImage (const char *, const char *, int,
                         double, unsigned int, unsigned int);

std::map<std::string,int> RegressionTestBaselines (char *);

void RegisterTests();
void PrintAvailableTests()
{
  std::cout << "Available tests:\n";
  std::map<std::string, MainFuncPointer>::iterator
                                          j = StringToTestFunctionMap.begin();
  int i = 0;
  while(j != StringToTestFunctionMap.end())
    {
    std::cout << i << ". " << j->first << "\n";
    ++i;
    ++j;
    }
}

int main(int ac, char* av[] )
{
  char *baselineFilename = NULL;
  char *testFilename = NULL;

  double toleranceIntensity = 2.0;
  unsigned int toleranceRadius = 1;
  unsigned int toleranceNumberOfPixels = 0;

  std::cout << "AC = " << ac << std::endl;
  RegisterTests();
  std::string testToRun;

  if(ac < 2)
    {
    PrintAvailableTests();
    std::cout << "To run a test, enter the test number: ";
    int testNum = 0;
    std::cin >> testNum;
    std::map<std::string, MainFuncPointer>::iterator
                                          j = StringToTestFunctionMap.begin();
    int i = 0;
    while(j != StringToTestFunctionMap.end() && i < testNum)
      {
      ++i;
      ++j;
      }
    if(j == StringToTestFunctionMap.end())
      {
      std::cerr << testNum << " is an invalid test number\n";
      return -1;
      }
    testToRun = j->first;
    }
  else
    {
    if (strcmp(av[1], "--with-threads") == 0)
      {
      int numThreads = atoi(av[2]);
      itk::MultiThreader::SetGlobalDefaultNumberOfThreads(numThreads);
      av += 2;
      ac -= 2;
      }
    else if (strcmp(av[1], "--without-threads") == 0)
      {
      itk::MultiThreader::SetGlobalDefaultNumberOfThreads(1);
      av += 1;
      ac -= 1;
      }
    else if (strcmp(av[1], "--compare") == 0)
      {
      baselineFilename = av[2];
      testFilename = av[3];
      av += 3;
      ac -= 3;
      if (strcmp(av[1], "--toleranceIntensity") == 0)
        {
        toleranceIntensity = atof( av[2] );
        av += 2;
        ac -= 2;
        }
      if (strcmp(av[1], "--toleranceRadius") == 0)
        {
        toleranceRadius = atoi( av[2] );
        av += 2;
        ac -= 2;
        }
      if (strcmp(av[1], "--toleranceNumberOfPixels") == 0)
        {
        toleranceNumberOfPixels = atoi( av[2] );
        av += 2;
        ac -= 2;
        }
      }
    testToRun = av[1];
    }
  std::map<std::string, MainFuncPointer>::iterator
                                 j = StringToTestFunctionMap.find(testToRun);
  if(j != StringToTestFunctionMap.end())
    {
    MainFuncPointer f = j->second;
    int result;
    try
      {
      // Invoke the test's "main" function.
      result = (*f)(ac-1, av+1);

      // Make a list of possible baselines
      if (baselineFilename && testFilename)
        {
        std::map<std::string,int> baselines =
                                    RegressionTestBaselines(baselineFilename);
        std::map<std::string,int>::iterator baseline = baselines.begin();
        std::string bestBaseline;
        int bestBaselineStatus = itk::NumericTraits<int>::max();
        while (baseline != baselines.end())
          {
          baseline->second = RegressionTestImage(testFilename,
                                                 (baseline->first).c_str(),
                                                 0,
                                                 toleranceIntensity,
                                                 toleranceRadius,
                                                 toleranceNumberOfPixels);
          if (baseline->second < bestBaselineStatus)
            {
            bestBaseline = baseline->first;
            bestBaselineStatus = baseline->second;
            }
          if (baseline->second == 0)
            {
            break;
            }
          ++baseline;
          }
        // if the best we can do still has errors, generate the error images
        if (bestBaselineStatus)
          {
          RegressionTestImage(testFilename,
                              bestBaseline.c_str(),
                              1,
                              toleranceIntensity,
                              toleranceRadius,
                              toleranceNumberOfPixels);
          }

        // output the matching baseline
        std::cout << "<DartMeasurement name=\"BaselineImageName\" \
                     type=\"text/string\">";
        std::cout << itksys::SystemTools::GetFilenameName(bestBaseline);
        std::cout << "</DartMeasurement>" << std::endl;

        result += bestBaselineStatus;
        }
      }
    catch(const itk::ExceptionObject& e)
      {
      std::cerr << "ITK test driver caught an ITK exception:\n";
      std::cerr << e.GetFile() << ":" << e.GetLine() << ":\n"
                << e.GetDescription() << "\n";
      result = -1;
      }
    catch(const std::exception& e)
      {
      std::cerr << "ITK test driver caught an exception:\n";
      std::cerr << e.what() << "\n";
      result = -1;
      }
    catch(...)
      {
      std::cerr << "ITK test driver caught an unknown exception!!!\n";
      result = -1;
      }
    return result;
    }
  PrintAvailableTests();
  std::cerr << "Failed: " << testToRun << ": No test registered with name "
            << testToRun << "\n";
  return -1;
}

// Regression Testing Code

int RegressionTestImage( const char *testImageFilename,
                         const char *baselineImageFilename,
                         int reportErrors,
                         double toleranceIntensity,
                         unsigned int toleranceRadius,
                         unsigned int toleranceNumberofPixels)
{
  // Use the factory mechanism to read the test and baseline files
  // and convert them to double
  typedef itk::Image<double,IGSTK_TEST_DIMENSION_MAX>         ImageType;
  typedef itk::Image<unsigned char,IGSTK_TEST_DIMENSION_MAX>  OutputType;
  typedef itk::Image<unsigned char,2>                         DiffOutputType;
  typedef itk::ImageFileReader<ImageType>                     ReaderType;

  // Read the baseline file
  ReaderType::Pointer baselineReader = ReaderType::New();
  baselineReader->SetFileName(baselineImageFilename);
  try
    {
    baselineReader->UpdateLargestPossibleRegion();
    }
  catch (itk::ExceptionObject& e)
    {
    std::cerr << "Exception detected while reading "
              << baselineImageFilename << " : "  << e.GetDescription();
    return 1000;
    }

  // Read the file generated by the test
  ReaderType::Pointer testReader = ReaderType::New();
  testReader->SetFileName(testImageFilename);
  try
    {
    testReader->UpdateLargestPossibleRegion();
    }
  catch (itk::ExceptionObject& e)
    {
    std::cerr << "Exception detected while reading "
            << testImageFilename << " : "  << e.GetDescription() << std::endl;
    return 1000;
    }

  // The sizes of the baseline and test image must match
  ImageType::SizeType baselineSize;
  baselineSize =
         baselineReader->GetOutput()->GetLargestPossibleRegion().GetSize();
  ImageType::SizeType testSize;
  testSize = testReader->GetOutput()->GetLargestPossibleRegion().GetSize();

  if (baselineSize != testSize)
    {
    std::cerr << "The size of the Baseline image and Test image do not match!"
              << std::endl;
    std::cerr << "Baseline image: " << baselineImageFilename
              << " has size " << baselineSize << std::endl;
    std::cerr << "Test image:     " << testImageFilename
              << " has size " << testSize << std::endl;
    return 1;
    }

  // Now compare the two images
  typedef itk::Testing::ComparisonImageFilter<ImageType,ImageType> DiffType;
  DiffType::Pointer diff = DiffType::New();
  diff->SetValidInput(baselineReader->GetOutput());
  diff->SetTestInput(testReader->GetOutput());
  diff->SetDifferenceThreshold( toleranceIntensity );
  diff->SetToleranceRadius( toleranceRadius );
  diff->UpdateLargestPossibleRegion();

  // Analyze the difference image
  //
  // if the number of non-zero pixels in the difference image is
  // less than a threshold, then the baseline and test output
  // generated image are marked to be different and
  // a difference image is generated.
  //

  typedef DiffType::OutputImageType  OutputImageType;
  typedef OutputImageType::PixelType PixelType;
  OutputImageType::Pointer outputImage = OutputImageType::New();
  outputImage = diff->GetOutput();

  typedef itk::ImageRegionConstIterator< OutputImageType > ConstIteratorType;
  ConstIteratorType inputIt( outputImage,
                                    outputImage->GetLargestPossibleRegion());

  PixelType ZeroValue  = itk::NumericTraits< PixelType >::Zero;

  unsigned int count = 0;
  for ( inputIt.GoToBegin(); !inputIt.IsAtEnd(); ++inputIt )
    {
    if ( inputIt.Get() != ZeroValue )
      {
      count++;
      }
    }

  int status=0;

  status = count > toleranceNumberofPixels ? 1 : 0;

  // if there are discrepencies, create an diff image
  if (status && reportErrors)
    {
    typedef itk::RescaleIntensityImageFilter<ImageType,OutputType> RescaleType;
    typedef itk::ExtractImageFilter<OutputType,DiffOutputType>     ExtractType;
    typedef itk::ImageFileWriter<DiffOutputType>                   WriterType;
    typedef itk::ImageRegion<IGSTK_TEST_DIMENSION_MAX>             RegionType;

    OutputType::IndexType index; index.Fill(0);
    OutputType::SizeType size; size.Fill(0);

    RescaleType::Pointer rescale = RescaleType::New();
    rescale->SetOutputMinimum(
                         itk::NumericTraits<unsigned char>::NonpositiveMin());
    rescale->SetOutputMaximum(itk::NumericTraits<unsigned char>::max());
    rescale->SetInput(diff->GetOutput());
    rescale->UpdateLargestPossibleRegion();

    RegionType region;
    region.SetIndex(index);

    size = rescale->GetOutput()->GetLargestPossibleRegion().GetSize();
    for (unsigned int i = 2; i < IGSTK_TEST_DIMENSION_MAX; i++)
      {
      size[i] = 0;
      }
    region.SetSize(size);

    ExtractType::Pointer extract = ExtractType::New();
    extract->SetInput(rescale->GetOutput());
    extract->SetExtractionRegion(region);

    WriterType::Pointer writer = WriterType::New();
    writer->SetInput(extract->GetOutput());

    std::cout << "<DartMeasurement name=\"ImageError\" \
                 type=\"numeric/double\">";
    std::cout << status;
    std::cout <<  "</DartMeasurement>" << std::endl;

    std::ostringstream diffName;
    diffName << testImageFilename << ".diff.png";
    try
      {
      rescale->SetInput(diff->GetOutput());
      rescale->Update();
      }
    catch (...)
      {
      std::cerr << "Error during rescale of " << diffName.str() << std::endl;
      }
    writer->SetFileName(diffName.str().c_str());
    try
      {
      writer->Update();
      }
    catch (...)
      {
      std::cerr << "Error during write of " << diffName.str() << std::endl;
      }

    std::cout << "<DartMeasurementFile name=\"DifferenceImage\" \
                 type=\"image/png\">";
    std::cout << diffName.str();
    std::cout << "</DartMeasurementFile>" << std::endl;

    std::ostringstream baseName;
    baseName << testImageFilename << ".base.png";
    try
      {
      rescale->SetInput(baselineReader->GetOutput());
      rescale->Update();
      }
    catch (...)
      {
      std::cerr << "Error during rescale of " << baseName.str() << std::endl;
      }
    try
      {
      writer->SetFileName(baseName.str().c_str());
      writer->Update();
      }
    catch (...)
      {
      std::cerr << "Error during write of " << baseName.str() << std::endl;
      }

    std::cout << "<DartMeasurementFile name=\"BaselineImage\" \
                 type=\"image/png\">";
    std::cout << baseName.str();
    std::cout << "</DartMeasurementFile>" << std::endl;

    std::ostringstream testName;
    testName << testImageFilename << ".test.png";
    try
      {
      rescale->SetInput(testReader->GetOutput());
      rescale->Update();
      }
    catch (...)
      {
      std::cerr << "Error during rescale of " << testName.str()
                << std::endl;
      }
    try
      {
      writer->SetFileName(testName.str().c_str());
      writer->Update();
      }
    catch (...)
      {
      std::cerr << "Error during write of " << testName.str() << std::endl;
      }

    std::cout << "<DartMeasurementFile name=\"TestImage\" type=\"image/png\">";
    std::cout << testName.str();
    std::cout << "</DartMeasurementFile>" << std::endl;
    }
  return (status != 0) ? 1 : 0;
}

//
// Generate all of the possible baselines
// The possible baselines are generated fromn the baselineFilename
// using the following algorithm:
// 1) strip the suffix
// 2) append a digit .x
// 3) append the original suffix.
// It the file exists, increment x and continue
//
std::map<std::string,int> RegressionTestBaselines (char *baselineFilename)
{
  std::map<std::string,int> baselines;
  baselines[std::string(baselineFilename)] = 0;

  std::string originalBaseline(baselineFilename);

  int x = 0;
  std::string::size_type suffixPos = originalBaseline.rfind(".");
  std::string suffix;
  if (suffixPos != std::string::npos)
    {
    suffix = originalBaseline.substr(suffixPos,originalBaseline.length());
    originalBaseline.erase(suffixPos,originalBaseline.length());
    }
  while (++x)
    {
    std::ostringstream filename;
    filename << originalBaseline << "." << x << suffix;
    std::ifstream filestream(filename.str().c_str());
    if (!filestream)
      {
      break;
      }
    baselines[filename.str()] = 0;
    filestream.close();
    }
  return baselines;
}
