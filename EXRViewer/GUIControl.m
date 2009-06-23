#import "GUIControl.h"

#define MAX_WIDTH 800
#define MAX_HEIGHT 600

@implementation GUIControl

- (IBAction)openFile:(id)sender
{
	// "Standard" open file panel
	NSArray *fileTypes = [NSArray arrayWithObjects:@"exr", nil];
	
	int i;
	// Create the File Open Panel class.
	NSOpenPanel* oPanel = [NSOpenPanel openPanel];
		
	[oPanel setCanChooseDirectories:NO];
	[oPanel setCanChooseFiles:YES];
	[oPanel setCanCreateDirectories:YES];
	[oPanel setAllowsMultipleSelection:NO];
	[oPanel setAlphaValue:0.95];
	[oPanel setTitle:@"Select a file to open"];
	
	// Display the dialog.  If the OK button was pressed,
	// process the files.
	if ( [oPanel runModalForDirectory:nil file:nil types:fileTypes]
							== NSOKButton )
	{
		// Get an array containing the full filenames of all
		// files and directories selected.
		NSArray* files = [oPanel filenames];
		
		// Loop through all the files and process them.
		for( i = 0; i < [files count]; i++ )
		{
			NSString* fileName = [files objectAtIndex:i];
			NSLog(fileName);
			
			[ViewImage loadImage: fileName];

			NSRect frame = [MyWindow frame];
			frame.size.width = [ViewImage getWidth];
			frame.size.height = [ViewImage getHeight];
			
			float whratio = frame.size.width;
			whratio /= frame.size.height;
			
			if (frame.size.width >= MAX_WIDTH) {
				frame.size.width = MAX_WIDTH;
				frame.size.height = MAX_WIDTH/whratio;
			}
			if (frame.size.height >= MAX_HEIGHT) {
				frame.size.height = MAX_HEIGHT;
				frame.size.width = MAX_HEIGHT*whratio;
			}
			
			frame.size.width += 30;
			frame.size.height += 150;
			
			[MyWindow setFrame:frame display:YES
					animate:YES];
						
		}
	}
}

- (IBAction)setExposure:(id)sender {

    float exp = [expDial floatValue];
	
	[ViewImage setExposure: pow(2, exp)];
	[ViewImage setNeedsDisplay:YES];
	
	char foo[32];
	sprintf(foo, "%f", exp);
	NSString *bar; 

	bar = [NSString stringWithUTF8String:foo]; 
	[shoexp setStringValue: bar];
}
@end
