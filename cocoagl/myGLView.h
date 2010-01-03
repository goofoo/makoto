#import <Cocoa/Cocoa.h>
#import "TestPiece.h"
 
@interface myGLView : NSOpenGLView
{
	double lastFrameReferenceTime;
	NSTimer *timer;

	float dir;
	IBOutlet NSSlider *knoiseDial;
	IBOutlet NSSlider *zcamDial;
	IBOutlet NSSlider *nsliceDial;
	IBOutlet NSSlider *freqDial;
	IBOutlet NSSlider *lacunarityDial;
	IBOutlet NSSlider *dimensionDial;
	float knoise, zcam, freq, lacunarity, dimension;
	int nslice;
	
	TestPiece *piece;
	
	bool leftMouseIsDown;
	bool rightMouseIsDown;
	NSPoint lastMousePoint;
	
	float eyex, eyey, eyez;
}

@property(copy, readwrite) TestPiece *piece;

+ (NSOpenGLPixelFormat*) basicPixelFormat;

- (void) updateProjection;
- (void) updateModelView;
- (void) resizeGL;

- (void) drawRect:(NSRect)rect;

- (void) prepareOpenGL;
- (id) initWithFrame: (NSRect) frameRect;
- (IBAction)setExposure:(id)sender;
- (IBAction)setFreq:(id)sender;
- (IBAction)setLacunarty:(id)sender;
- (IBAction)setDimension:(id)sender;

- (void)setPiece:(TestPiece *) apiece;
@end