#include "zFnEXR.h"

ZFnEXR::ZFnEXR(void)
{
}

ZFnEXR::~ZFnEXR(void)
{
}

void ZFnEXR::checkFile(const char* filename, int* width, int* height)
{
	InputFile file(filename);
	Box2i dw = file.header().dataWindow(); 
	*width  = dw.max.x - dw.min.x + 1;
	*height = dw.max.y - dw.min.y + 1;
}

void ZFnEXR::readR(const char* filename, int width, int height, float* data)
{
	InputFile file(filename); 
	Box2i dw = file.header().dataWindow();
	
	int size = (width)*(height);
	
	half *rPixels = new half[size];
	
	FrameBuffer frameBuffer; 
	frameBuffer.insert ("R",                                  // name 
						Slice (HALF,                          // type 
							   (char *) rPixels, 
							   sizeof (*rPixels) * 1,    // xStride 
							   sizeof (*rPixels) * (width),// yStride 
							   1, 1,                          // x/y sampling 
							   0.0));                         // fillValue 
							   
	file.setFrameBuffer (frameBuffer); 
	file.readPixels (dw.min.y, dw.max.y); 
	
	for(int i=0; i<size; i++) data[i] = rPixels[i];
	
	delete[] rPixels;
}

void ZFnEXR::save(float* data, const char* filename, int width, int height)
{
	Array2D<Rgba> px (height, width);

	 for (int y = 0; y < height; ++y)
    {
		int invy = height-1-y;
	for (int x = 0; x < width; ++x)
	{
	    Rgba &p = px[y][x];
	    p.r = data[(invy*width+x)*4];
	    p.g = data[(invy*width+x)*4+1];
	    p.b = data[(invy*width+x)*4+2];
	    p.a = data[(invy*width+x)*4+3];
	}
    }

	RgbaOutputFile file (filename, width, height, WRITE_RGBA);
    file.setFrameBuffer (&px[0][0], 1, width);
    file.writePixels (height);
}

void ZFnEXR::saveCameraNZ(float* data, M44f mat, float fov, const char* filename, int width, int height)
{
	Header header (width, height); 
	header.insert ("fov", DoubleAttribute (fov)); 
	header.insert ("cameraTransform", M44fAttribute (mat));
	header.channels().insert ("R", Channel (FLOAT));
	
	OutputFile file (filename, header); 
	FrameBuffer frameBuffer;

	frameBuffer.insert ("R", 
						Slice (FLOAT, 
							   (char *) data, 
							   sizeof (*data) * 1, 
							   sizeof (*data) * width)); 
	file.setFrameBuffer (frameBuffer);              
	file.writePixels (height);
}

void ZFnEXR::readCameraNZ(const char* filename, int width, int height, float* data, M44f& mat, float& fov)
{
	InputFile file(filename); 
	
	const DoubleAttribute *fovattr = file.header().findTypedAttribute <DoubleAttribute> ("fov");
	fov = fovattr->value();
	
	const M44fAttribute *matattr = file.header().findTypedAttribute <M44fAttribute> ("cameraTransform");
	mat = matattr->value();
	
	Box2i dw = file.header().dataWindow();
	
	int size = (width)*(height);
	
	FrameBuffer frameBuffer; 
	frameBuffer.insert ("R",                                  
						Slice (FLOAT,                         
							   (char *) data, 
							   sizeof (*data) * 1,    
							   sizeof (*data) * (width),
							   1, 1,                          
							   0.0));                        
							   
	file.setFrameBuffer (frameBuffer); 
	file.readPixels (dw.min.y, dw.max.y); 
}