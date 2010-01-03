#import <Cocoa/Cocoa.h>
#import "TestPiece.h"
 
@interface myGLView : NSOpenGLView
{
	double lastFrameReferenceTime;
	NSTimer *timer;

	float dir;
	
	float knoise, zcam, freq, lacunarity, dimension;
	int nslice;
	
	TestPiece *piece;
	
	bool leftMouseIsDown;
	bool rightMouseIsDown;
	NSPoint lastMousePoint;
	
	float eyex, eyey, eyez;
}

+ (NSOpenGLPixelFormat*) basicPixelFormat;

- (void) updateProjection;
- (void) updateModelView;
- (void) resizeGL;

- (void) drawRect:(NSRect)rect;

- (void) prepareOpenGL;
- (id) initWithFrame: (NSRect) frameRect;
 
- (TestPiece *)piece;
- (void)setPiece:(TestPiece *) apiece;

@end