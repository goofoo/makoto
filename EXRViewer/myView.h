#import <Cocoa/Cocoa.h>

@interface myView : NSView/* Specify a superclass (eg: NSObject or NSView) */ {

	NSBitmapImageRep *myImageCache;
	unsigned char *myDataPtr;
	float *data;
	int myRowBytes;
	int width, height;
	float exposure;
}

- (id)initWithFrame:(NSRect) frameRect;
- (void) drawRect: (NSRect) bounds ;
- (void) loadImage: (NSString *) filename;
- (int) getWidth;
- (int) getHeight;
- (void) setExposure: (float) val;
@end
