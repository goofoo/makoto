#import "myView.h"

#import "image.h"

@implementation myView

- (void) loadImage: (NSString *) filename
{
	loadEXR([filename UTF8String], &width, &height);
	if(data) free(data);
	data = (float *)malloc(width*height*4*sizeof(float));
	
	readEXR([filename UTF8String], width, height, data);
}

- (id)initWithFrame:(NSRect)frameRect
{
	exposure = 1.f;
	self = [super initWithFrame:frameRect];
	return self;
}

-(void) drawRect: (NSRect) bounds
{
	NSRect bds = [self bounds];
	
	myRowBytes = 4 * bds.size.width;
	
	if(myDataPtr) free(myDataPtr);
	if(myImageCache) [myImageCache release];
	
	myDataPtr = (unsigned char *)malloc(myRowBytes * bds.size.height);
	myImageCache = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:&myDataPtr
			pixelsWide:bds.size.width
			pixelsHigh:bds.size.height
			bitsPerSample:8
			samplesPerPixel:4
			hasAlpha:YES
			isPlanar:NO
			colorSpaceName:NSCalibratedRGBColorSpace
			bitmapFormat:0
			bytesPerRow:4*bds.size.width
			bitsPerPixel:32];
			
	if(data) {
		int i, j;
		float x, y, cap;
		
		for (j=0; j<bds.size.height; j++) {
		
			y = (float)j/bds.size.height;
			
			for (i=0; i<bds.size.width; i++) {
				
				x = (float)i/bds.size.width;
				
				int drawIndex = j*bds.size.width + i;
				int imgx = x*width;
				int imgy = y*height;
				int imgIndex = imgy*width + imgx;
				
				cap = data[imgIndex*4]*exposure;
				if(cap > 1) cap = 1;
				myDataPtr[drawIndex*4] = cap*255;
				
				cap = data[imgIndex*4+1]*exposure;
				if(cap > 1) cap = 1;
				myDataPtr[drawIndex*4+1] = cap*255;
				
				cap = data[imgIndex*4+2]*exposure;
				if(cap > 1) cap = 1;
				myDataPtr[drawIndex*4+2] = cap*255;
				myDataPtr[drawIndex*4+3] = 255;
			}
		}
	
	}
	
	[myImageCache draw];
}

- (int) getWidth
{
	return width;
}

- (int) getHeight
{
	return height;
}

- (void) setExposure: (float) val
{
	exposure = val;

	[self drawRect:[self bounds]];
}

@end
