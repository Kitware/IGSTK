/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkCylinderSpatialObject.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "itkCylinderSpatialObject.h" 

namespace itk 
{ 

/** Constructor */
CylinderSpatialObject::CylinderSpatialObject()
{
  this->SetTypeName("CylinderSpatialObject");
  this->SetDimension(3);
  m_Radius = 1.0;
  m_Height = 1.0;
} 

/** Destructor */
CylinderSpatialObject ::~CylinderSpatialObject()  
{
  
}

/** Test whether a point is inside or outside the object 
 *  For computational speed purposes, it is faster if the method does not
 *  check the name of the class and the current depth */ 
bool CylinderSpatialObject
::IsInside( const PointType & point) const
{
  if(!this->GetIndexToWorldTransform()->GetInverse(const_cast<TransformType *>(this->GetInternalInverseTransform())))
    {
    return false;
    }
/*
  PointType transformedPoint = this->GetInternalInverseTransform()->TransformPoint(point);  
  double r = 0;
  for(unsigned int i=0;i<3;i++)
    {
    if(m_Radius[i]!=0.0)
      {
      r += (transformedPoint[i]*transformedPoint[i])/(m_Radius[i]*m_Radius[i]);
      }
    else if(transformedPoint[i]>0.0)  // Degenerate Cylinder
      {
      r = 2; // Keeps function from returning true here 
     break;
      }
    }
  
  if(r<1)
    {
    return true;
    }*/
  return false;
}



/** Test if the given point is inside the Cylinder */
bool CylinderSpatialObject 
::IsInside( const PointType & point, unsigned int depth, char * name ) const 
{
  itkDebugMacro( "Checking the point [" << point << "] is inside the Cylinder" );
    
  if(name == NULL)
    {
    if(IsInside(point))
      {
      return true;
      }
    }
  else if(strstr(typeid(Self).name(), name))
    {
    if(IsInside(point))
      {
      return true;
      }
    }

  return Superclass::IsInside(point, depth, name);
} 

/** Compute the bounds of the Cylinder */
bool CylinderSpatialObject
::ComputeLocalBoundingBox() const
{ 
  itkDebugMacro( "Computing Cylinder bounding box" );

/*  if( this->GetBoundingBoxChildrenName().empty() 
      || strstr(typeid(Self).name(), this->GetBoundingBoxChildrenName().c_str()) )
    {
    PointType pnt;
    PointType pnt2;
    for(unsigned int i=0; i<;i++) 
      {   
      if(m_Radius[i]>0)
        {
        pnt[i]=-m_Radius[i];
        pnt2[i]=m_Radius[i];
        }
      else
        {
        pnt[i]=m_Radius[i];
        pnt2[i]=-m_Radius[i];
        }
      } 
     
      pnt = this->GetIndexToWorldTransform()->TransformPoint(pnt);
      pnt2 = this->GetIndexToWorldTransform()->TransformPoint(pnt2);
  */       
//      const_cast<BoundingBoxType *>(this->GetBounds())->SetMinimum(pnt);
//      const_cast<BoundingBoxType *>(this->GetBounds())->SetMaximum(pnt2);
 //   }
  return true;
} 


/** Returns if the Cylinder os evaluable at one point */
bool CylinderSpatialObject
::IsEvaluableAt( const PointType & point, unsigned int depth, char * name ) const
{
  itkDebugMacro( "Checking if the Cylinder is evaluable at " << point );
  return IsInside(point, depth, name);
}

/** Returns the value at one point */
bool CylinderSpatialObject
::ValueAt( const PointType & point, double & value, unsigned int depth,
           char * name ) const
{
  itkDebugMacro( "Getting the value of the Cylinder at " << point );
  if( IsInside(point, 0, name) )
    {
    value = 1;
    return true;
    }
  else
    {
    if( Superclass::IsEvaluableAt(point, depth, name) )
      {
      Superclass::ValueAt(point, value, depth, name);
      return true;
      }
    else
      {
      value = 0;
      return false;
      }
    }
  return false;
}

/** Print Self function */
void CylinderSpatialObject
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os, indent);
  os << "Radius: " << m_Radius << std::endl;
  os << "Height: " << m_Height << std::endl;
}

} // end namespace itk

