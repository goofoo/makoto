// Class for dealing with HDR textures

#include "hdrtexture.h"
#include "string.h"

#define RGBE_RLE_32BIT 0
/* offsets to red, green, and blue components in a data (float) pixel */
#define RGBE_DATA_RED    0
#define RGBE_DATA_GREEN  1
#define RGBE_DATA_BLUE   2
/* number of floats per pixel */
#define RGBE_DATA_SIZE   3

void rgbe2float(float *red, float *green, float *blue, unsigned char rgbe[4])
{
  float f;

  if (rgbe[3]!=0) 
  {   /*nonzero pixel*/
    f = ldexp(1.0f,rgbe[3]-(int)(128+8));
    *red = rgbe[0] * f;
    *green = rgbe[1] * f;
    *blue = rgbe[2] * f;
  }
  else
    *red = *green = *blue = 0.0;
}

HDRtexture::HDRtexture(const char *filename)
	: m_valid(false), m_data(NULL), m_exp(0.0), m_exposure(1.0)
{
	load(filename);
	m_space.setIdentity();
//  printf("Loaded HDR image: \"%s\"\nImage resolution: %d X %d\n", filename, m_width, m_height);
  
}

void HDRtexture::load(const char *filename)
{
	if(m_data) delete[] m_data;
		
	FILE *fp = fopen(filename, "rb");
  if (!fp) {
	 printf("Failed to load HDR image: %s\n", filename);
    return;
  }
  
  char sbuf[128];
  float tempf;
  
  fgets(sbuf,sizeof(sbuf),fp);
  
  if(strcmp(sbuf,"#?RADIANCE\n")!=0)
  {
	printf("%s is not an HDR Image\n", filename);
	fclose(fp);
	return;
  }
  
  while(strcmp(sbuf,"\n")!=0)
  {
	if(strcmp(sbuf,"FORMAT=32-bit_rle_rgbe\n") == 0)
		m_format = RGBE_RLE_32BIT;
	else if(sscanf(sbuf,"EXPOSURE=%g",&tempf) == 1)
	{/*m_exposure = tempf;*/}
	fgets(sbuf,sizeof(sbuf),fp);
  }
  
  fgets(sbuf,sizeof(sbuf),fp);
  //printf(sbuf);
  if (sscanf(sbuf,"-Y %d +X %d",&m_height,&m_width) == 2)
	m_flip_vertical = 1;
else if(sscanf(sbuf,"+Y %d +X %d",&m_height,&m_width) == 2)
	m_flip_vertical = 0;
	
	//if(m_width>=1024) m_width = 1024;
	//else m_width = 512;
  
  	m_height = m_width/2;

	unsigned char* data = new unsigned char[m_width*m_height*4];
	
	for(int j = 0; j<m_height; j++)
	{
		for(int i=0; i<m_width; i++)
		{
			data[(j*m_width+i)*4]=255;
			data[(j*m_width+i)*4+1]=255;
			data[(j*m_width+i)*4+2]=255;
			data[(j*m_width+i)*4+3]=255;
		}
	}
	
	
	
	unsigned char rgbe[4];
	unsigned char buf[2];
	int count;
	
	int line = m_height;
	
	while(line>0)
	{
		if(fread(rgbe,sizeof(rgbe),1,fp) < 1)
		{
			delete[] data;
			printf("Failed to read pixels\n");
			return;
		}
	
		//printf("%i %i %i %i\n", rgbe[0], rgbe[1], rgbe[2], rgbe[3]);
			
		unsigned char* scanline_buf = new unsigned char[m_width*4];
		unsigned char* ptr = &scanline_buf[0];
		unsigned char* ptr_end;
			for(int i=0;i<4;i++) 
			{
				ptr_end = &scanline_buf[(i+1)*m_width];
				while(ptr<ptr_end)
				{
					fread(buf,2,1,fp);
					
					if(buf[0]>128)
					{
						count = buf[0]-128;
						while(count-- > 0)
							*ptr++ = buf[1];
					}
					else
					{
						count = buf[0];
						*ptr++ = buf[1];
						if (--count > 0) 
						{
							if (fread(ptr,sizeof(*ptr)*count,1,fp) < 1) 
							{
								return;
							}
							ptr += count;
						}
					}
				}
			}
			
			for(int i=0;i<m_width;i++)
			{
				data[((line-1)*m_width+i)*4] = scanline_buf[i];
				data[((line-1)*m_width+i)*4+1] = scanline_buf[i+m_width];
				data[((line-1)*m_width+i)*4+2] = scanline_buf[i+2*m_width];
				data[((line-1)*m_width+i)*4+3] = scanline_buf[i+3*m_width];
			}
		
		line--;
	}
	
  fclose(fp);
  
  //for(int i=0; i<m_width*m_height; i=i+1024)
  m_data = new float[m_width*m_height*4];
  //unsigned char rgbe[4];
  
  for(int j=0; j<m_height; j++)
  {
	for(int i=0; i<m_width; i++)
	{
		rgbe[0] = data[(j*m_width+i)*4];
		rgbe[1] = data[(j*m_width+i)*4+1];
		rgbe[2] = data[(j*m_width+i)*4+2];
		rgbe[3] = data[(j*m_width+i)*4+3];
		
		rgbe2float(&m_data[(j*m_width+i)*4], &m_data[(j*m_width+i)*4+1], &m_data[(j*m_width+i)*4+2], rgbe);
		
		m_data[(j*m_width+i)*4+3] = 1.0f;
	}
  }
  //clean up
	delete[] data;
  
  

  m_valid = true;
}

HDRtexture::~HDRtexture()
{
  delete [] m_data;
  //delete [] m_hilodata;
}

void HDRtexture::getRGBByVector(const double vx, const double vy, const double vz, float& r, float& g, float& b) const
{
	XYZ v(vx, vy, vz);
	m_space.transform(v);
	v.normalize();
	double theta, phi;
	xyz2sph(v, theta, phi);
	getRGBAt(theta, phi, r, g, b);
}
//:~
