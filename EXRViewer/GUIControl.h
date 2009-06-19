#import <Cocoa/Cocoa.h>
#import "myView.h"

@interface GUIControl : NSObject {
    IBOutlet NSWindow *MyWindow;
    IBOutlet myView *ViewImage;
	IBOutlet NSSlider *expDial;
	IBOutlet NSTextField *shoexp;
}
- (IBAction)openFile:(id)sender;
- (IBAction)setExposure:(id)sender;
@end
