//
//  UIController.h
//  triangle
//
//  Created by jian zhang on 1/2/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "myGLView.h"

@interface UIController : NSObject {
	IBOutlet NSTableView *table_view;
	IBOutlet myGLView  *opengl_view;
	IBOutlet NSArrayController *attrib_array;
	NSArray *pieces;
}

- (id) init;
- (int) numberOfRowsInTableView: (NSTableView *) aTableView;
- (id) tableView: (NSTableView *) aTableView
       objectValueForTableColumn: (NSTableColumn *) aTableColumn                                                                 
       row: (int) rowIndex;
- (void) tableViewSelectionDidChange: (NSNotification *) aNotification;

@end
