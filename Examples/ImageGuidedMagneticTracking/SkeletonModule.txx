
#include "SkeletonModule.h"

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif

template <class TInputImage, class TOutputImage>
SkeletonModule<TInputImage, TOutputImage>
::SkeletonModule()
{
  m_pInputImage = NULL;
  m_pConnectedBuffer = NULL;
  m_ProgressCallbackFunc = NULL;

  m_pImportFilter = ImportFilterType::New();
  
  m_ClusterRadius = 7.0;
}

template <class TInputImage, class TOutputImage>
SkeletonModule<TInputImage, TOutputImage>
::~SkeletonModule()
{
  
}

template <class TInputImage, class TOutputImage>
void 
SkeletonModule<TInputImage, TOutputImage>
::SetInput(const TInputImage* pInputImage)
{
  m_pInputImage = pInputImage;
}

template <class TInputImage, class TOutputImage>
TOutputImage* 
SkeletonModule<TInputImage, TOutputImage>
::GetOutput()
{
  return m_pImportFilter->GetOutput();
}

template <class TInputImage, class TOutputImage>
void 
SkeletonModule<TInputImage, TOutputImage>
::SetClusterRadius(double radius)
{
  m_ClusterRadius = radius;;
}

template <class TInputImage, class TOutputImage>
void 
SkeletonModule<TInputImage, TOutputImage>
::SetProgressCallback(void(* func)(void*, double), void* cdata)
{
  m_ProgressCallbackFunc = func;
  m_CallData = cdata;
}

template <class TInputImage, class TOutputImage>
void 
SkeletonModule<TInputImage, TOutputImage>
::MorphologySkeleton(signed char* buffer, int x, int y)
{
	int i, j, idx, p, ext[4];
	int kicknum;
	int xy = x * y;
	
	ext[0] = 10000;
  ext[1] = 0;
  ext[2] = 10000;
  ext[3] = 0;
  
  for (j = 1; j < y - 1; j++)
  {
	  for (i = 1; i < x - 1; i++)
	  {
	    idx = j * x + i;
	    if (buffer[idx] == 1)
	    {
	      if (i < ext[0])
	      {
	        ext[0] = i;
	      }
	      if (i > ext[1])
	      {
	        ext[1] = i;
	      }
	      if (j < ext[2])
	      {
	        ext[2] = j;
	      }
	      if (j > ext[3])
	      {
	        ext[3] = j;
	      }
	    }
	  }
	}
	
	
	signed char* pTemp = new signed char[xy];
	signed char* pKeep = new signed char[xy];
	signed char* pDest = buffer; 
	
	memcpy(pTemp, buffer, xy);
	memset(pKeep, 0, xy);
	memset(pDest, 0, xy);

	kicknum = 1;
	while (kicknum > 0)
	{
		kicknum = 0;
		memset(pKeep, 0, xy);
		for (j = ext[2]; j <= ext[3]; j++)
		{
			for (i = ext[0]; i <= ext[1]; i++)
			{
				p = 0;
				idx = j * x + i;
				if (pTemp[idx] > 0)
				{
					if (pKeep[idx])
					{
						pDest[idx] = 1;
					}
					else
					{
						if (pTemp[idx - x] > 0)
							p += 1;
						if (pTemp[idx - 1] > 0)
							p += 2;
						if (pTemp[idx + x] > 0)
							p += 4;
						if (pTemp[idx + 1] > 0)
							p += 8;
						switch (p)
						{
						case 0:
							if (pTemp[idx - x - 1] > 0
								|| pTemp[idx - x + 1] > 0
								|| pTemp[idx + x - 1] > 0
								|| pTemp[idx + x + 1] > 0)
							{
								pDest[idx] = 1;
							}
							break;
						case 1:
							pDest[idx] = 1;
							pKeep[idx - x] = 1;
							break;
						case 2:
							pDest[idx] = 1;
							pKeep[idx - 1] = 1;
							break;
						case 4:
							pDest[idx] = 1;
							pKeep[idx + x] = 1;
							break;
						case 8:
							pDest[idx] = 1;
							pKeep[idx + 1] = 1;
							break;
						case 1 + 2:
							pDest[idx] = 0;
							if (pTemp[idx - x - x] == 0)
							{
								pKeep[idx - x] = 1;
							}
							if (pTemp[idx - 2] == 0)
							{
								pKeep[idx - 1] = 1;
							}
							kicknum++;
							break;
						case 2 + 4:
							pDest[idx] = 0;
							if (pTemp[idx + x + x] == 0)
							{
								pKeep[idx + x] = 1;
							}
							if (pTemp[idx - 2] == 0)
							{
								pKeep[idx - 1] = 1;
							}
							kicknum++;
							break;
						case 4 + 8:
							pDest[idx] = 0;
							if (pTemp[idx + x + x] == 0)
							{
								pKeep[idx + x] = 1;
							}
							if (pTemp[idx + 2] == 0)
							{
								pKeep[idx + 1] = 1;
							}
							kicknum++;
							break;
						case 8 + 1:
							pDest[idx] = 0;
							if (pTemp[idx - x - x] == 0)
							{
								pKeep[idx - x] = 1;
							}
							if (pTemp[idx + 2] == 0)
							{
								pKeep[idx + 1] = 1;
							}
							kicknum++;
							break; 
						case 1 + 4:
						case 2 + 8:
							pDest[idx] = 1;
							break;
						case 1 + 2 + 4:
							pDest[idx] = 0;
							pKeep[idx - 1] = 1;
							kicknum++;
							break;
						case 2 + 4 + 8:
							pDest[idx] = 0;
							pKeep[idx + 1] = 1;
							kicknum++;
							break;
						case 4 + 8 + 1:
							pDest[idx] = 0;
							pKeep[idx + 1] = 1;
							kicknum++;
							break;
						case 8 + 1 + 2:
							pDest[idx] = 0;
							pKeep[idx - x] = 1;
							kicknum++;
							break;
						case 1 + 2 + 4 + 8:
							pDest[idx] = 1;
							break;
						}
					}
				}
			}
		}
		memcpy(pTemp, pDest, xy);
	}

	delete pTemp;	
}

template <class TInputImage, class TOutputImage>
void 
SkeletonModule<TInputImage, TOutputImage>
::DeutschSkeleton(signed char* buffer, int x, int y)
{
  unsigned int i, j, l, idx, a, b;
  int off[9], kicknum, ext[4];
  bool kick;
    
  off[0] = 1;
  off[1] = 1 - x;
  off[2] = - x;
  off[3] = - 1 - x;
  off[4] = - 1;
  off[5] = - 1 + x;
  off[6] = x;
  off[7] = 1 + x;
  off[8] = off[0];
  
  ext[0] = 10000;
  ext[1] = 0;
  ext[2] = 10000;
  ext[3] = 0;
  
  for (j = 1; j < y - 1; j++)
  {
	  for (i = 1; i < x - 1; i++)
	  {
	    idx = j * x + i;
	    if (buffer[idx] == 1)
	    {
	      if (i < ext[0])
	      {
	        ext[0] = i;
	      }
	      if (i > ext[1])
	      {
	        ext[1] = i;
	      }
	      if (j < ext[2])
	      {
	        ext[2] = j;
	      }
	      if (j > ext[3])
	      {
	        ext[3] = j;
	      }
	    }
	  }
	}
    
  kicknum = 1;
	while (kicknum > 0)
  {
	  kicknum = 0;
	
	  for (j = ext[2]; j <= ext[3]; j++)
  	{
	    for (i = ext[0]; i <= ext[1]; i++)
	    {
	      kick = false;
	      a = b = 0;
  	    idx = j * x + i;
		
  	  	if (buffer[idx]	== 1)
	  	  {
		  	  for	(l = 0;	l	<	8; l++)
	  		  {
		    		a	+= abs(buffer[idx	+	off[l	+	1]]	-	buffer[idx + off[l]]); 
  			    b	+= buffer[idx	+	off[l]];
    			}
		    	if (a	== 0 &&	a	== 2 &&	a	== 4 
				    && b !=	1
      			&& (buffer[idx + off[0]] & buffer[idx	+	off[2]]	&	buffer[idx + off[4]] ==	0) 
			        || (buffer[idx + off[0]] & buffer[idx	+	off[2]]	&	buffer[idx + off[6]] ==	0))
    			{
		    		if (a	== 4)
				    {
    					if ((buffer[idx + off[0]] & buffer[idx	+	off[6]]	== 1)
		        		&& (buffer[idx	+	off[1]]	|	buffer[idx + off[5]] ==	1)
						    && buffer[idx	+	off[2]]	== 0
    						&& buffer[idx	+	off[3]]	== 0
		    				&& buffer[idx	+	off[4]]	== 0
				      	&& buffer[idx	+	off[7]]	== 0)
    					{
		    				kick = true;
				    	}
      				else if	((buffer[idx	+	off[0]]	&	buffer[idx + off[2]] ==	1)
			        	&& (buffer[idx	+	off[3]]	|	buffer[idx + off[7]] ==	1)
							  && buffer[idx	+	off[1]]	== 0
    						&& buffer[idx	+	off[4]]	== 0
		    				&& buffer[idx	+	off[5]]	== 0
				      	&& buffer[idx	+	off[6]]	== 0)
    					{
		    				kick = true;
				    	}
      			}
			    	else
					  {
    					kick = true;
		      	}
    			}
		    	if (kick)
    			{
		      	buffer[idx]	=	0;
					  kicknum++;
    			}
		    }
  		}
	  }
	
	  for (j = 1; j < y - 1; j++)
  	{
	    for (i = 1; i < x - 1; i++)
	    {
	      kick = false;
	      a = b = 0;
  	    idx = j * x + i;
	      if (buffer[idx] == 1)
	      {
	        for (l = 0; l < 8; l++)
      		{
		        a += abs(buffer[idx + off[l + 1]] - buffer[idx + off[l]]); 
      		  b += buffer[idx + off[l]];
		      }
      		if (a == 0 && a == 2 && a == 4 
		        && b != 1
      		  && (buffer[idx + off[2]] & buffer[idx + off[4]] & buffer[idx + off[6]] == 0) 
		          || (buffer[idx + off[4]] & buffer[idx + off[6]] & buffer[idx + off[0]] == 0))
        		{
        		  if (a == 4)
        		  {
        		    if ((buffer[idx + off[4]] & buffer[idx + off[2]] == 1)
        		      && (buffer[idx + off[5]] | buffer[idx + off[1]] == 1)
		              && buffer[idx + off[0]] == 0
        		      && buffer[idx + off[3]] == 0
		              && buffer[idx + off[6]] == 0
        		      && buffer[idx + off[7]] == 0)
		            {
        		      kick = true;
		            }
        		    else if ((buffer[idx + off[6]] & buffer[idx + off[4]] == 1)
		              && (buffer[idx + off[7]] | buffer[idx + off[3]] == 1)
        		      && buffer[idx + off[0]] == 0
		              && buffer[idx + off[5]] == 0
        		      && buffer[idx + off[2]] == 0
        		      && buffer[idx + off[1]] == 0)
		            {
        		      kick = true;
		            }		      
        		  }
		        else
      		  {
		          kick = true;
      		  }
		      }
      		if (kick)
		      {
      		  buffer[idx] = 0;
		        kicknum++;
      		}
		    }
      }
  	}
	}  
}



template <class TInputImage, class TOutputImage>
void
SkeletonModule<TInputImage, TOutputImage>
::ExtractSkeleton()
{
  if (!m_pInputImage)
  {
    return;
  }

  unsigned int i, j, k, xy, num;
  double f;
 
  signed char* p1;
  signed short* p2;

  InputRegionType region;
  InputSizeType size;
  InputIndexType index;
  InputPixelType pixel;
  InputSpacingType spacing;
  InputOriginType origin;

  region = m_pInputImage->GetLargestPossibleRegion();
  size = region.GetSize();
  index = region.GetIndex();
  xy = size[0] * size[1];
  num = size[2] * xy;
  
  spacing = m_pInputImage->GetSpacing();
  origin = m_pInputImage->GetOrigin();

  double ori[3], spa[3];
  for (i = 0; i < 3; i++)
  {
	ori[i] = origin[i];
	spa[i] = spacing[i];
  }  

  m_pImportFilter->SetRegion(region);
  m_pImportFilter->SetSpacing(spa);
  m_pImportFilter->SetOrigin(ori);
  
  m_pConnectedBuffer = new signed short[num];
  
  signed char* buffer = new signed char[xy];
  memset(buffer, 0, xy * sizeof(char));

  for (k = 0; k < size[2]; k++)
  {
	  p1 = buffer;
  	index[2] = k;
	  for (j = 0; j < size[1]; j++)
  	{
	    index[1] = j;
	    for (i = 0; i < size[0]; i++, p1++)
  	  {
	      index[0] = i;
	      pixel = m_pInputImage->GetPixel(index);
	      *p1 = (pixel > 0) ? 1: 0;	     
  	  }
	  }
  
//    this->DeutschSkeleton(buffer, size[0], size[1]);
    this->MorphologySkeleton(buffer, size[0], size[1]);
    
    this->Cluster2DSkeleton(buffer, size[0], size[1]);
    
    p1 = buffer;
	  p2 = m_pConnectedBuffer + k * xy;
  	for (j = 0; j < size[1]; j++)
	  {
	    for (i = 0; i < size[0]; i++, p1++, p2++)
  	  {
	      *p2 = *p1 * 100;
	    }
  	}
  	
  	if (m_ProgressCallbackFunc)
    {
      f = (double)(k + 1) / size[2];
      m_ProgressCallbackFunc(m_CallData, f);
    }
  }
  
  int k1 = 0, k2 = 0;
  p2 = m_pConnectedBuffer;

  for (k = 0; k < size[2]; k++)
  {
    for (j = 0; j < size[1]; j++)
    {
      for (i = 0; i < size[0]; i++, p2++)
      {
        if (*p2 > 0)
        {
          k1++;          
          printf("%d: x=%d y=%d z=%d\n", k1, i, j, k);
        }
        else
        {
          k2++;            
        }
      }
    }
  }
  
  m_pImportFilter->SetImportPointer(m_pConnectedBuffer, num, true);

  printf("%d center points got!\n", k1);
  
  delete buffer;

}

template <class TInputImage, class TOutputImage>
void
SkeletonModule<TInputImage, TOutputImage>
::Cluster2DSkeleton(signed char* buffer, int x, int y)
{
  int i, j, k, num, idx;
  double radius = m_ClusterRadius * m_ClusterRadius, dx, dy;
  double centerpos[1000][2];
  int centernum[1000];
  bool clustered;
  
  num = 0;
  for (j = 0; j < y; j++)
  {
    for (i = 0; i < x; i++)
    {
      idx = j * x + i;
      if (buffer[idx] > 0)
      {
        clustered = false;
        for (k = 0; k < num; k++)
        {
          dx = centerpos[k][0] - i;
          dy = centerpos[k][1] - j;
          if (dx * dx + dy * dy < radius)
          {
            centerpos[k][0] = (centerpos[k][0] * centernum[k] + i) / (centernum[k] + 1);
            centerpos[k][1] = (centerpos[k][1] * centernum[k] + j) / (centernum[k] + 1);
            centernum[k]++;
            clustered = true; 
          }          
        }
        if (!clustered)
        {
          centerpos[num][0] = i;
          centerpos[num][1] = j;
          centernum[num] = 1;
          num++;
        }
      }
    }
  }
  
  if (num > 0)
  {
    memset(buffer, 0, x * y * sizeof(signed char));
    for (k = 0; k < num; k++)
    {
      idx = (int)(centerpos[k][1] + 0.5) * x + (int)(centerpos[k][0] + 0.5);
      buffer[idx] = 1;
    }
  }
    
}

template <class TInputImage, class TOutputImage>
void
SkeletonModule<TInputImage, TOutputImage>
::TestSkeleton()
{
 if (!m_pInputImage)
  {
    return;
  }

  unsigned int i, j, k, l, xy, idx;  

  InputRegionType region;
  InputSizeType size;
  InputIndexType index;
  InputPixelType pixel;
  InputSpacingType spacing;
  InputOriginType origin;

  region = m_pInputImage->GetLargestPossibleRegion();
  size = region.GetSize();
  index = region.GetIndex();
  xy = size[0] * size[1];
 
  m_pImportFilter->SetRegion(region);

  spacing = m_pInputImage->GetSpacing();
  origin = m_pInputImage->GetOrigin();
  
  double ori[3], spa[3];
  for (i = 0; i < 3; i++)
  {
	ori[i] = origin[i];
	spa[i] = spacing[i];
  }  

  m_pImportFilter->SetSpacing(spa);
  m_pImportFilter->SetOrigin(ori);
  
  unsigned int num = size[0] * size[1] * size[2];
  signed short* buffer = new signed short[num];
  signed short* p1;
  
  p1 = buffer;
  for (k = 0; k < size[2]; k++)
  {
	for (j = 0; j < size[1]; j++)
	{
	  for (i = 0; i < size[0]; i++)
	  {
		*p1 = i + j + k;
		p1++;
	  }
	}
  }
  
  m_pImportFilter->SetImportPointer(buffer, num, true);
	
}

