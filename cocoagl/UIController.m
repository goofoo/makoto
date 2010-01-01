//
//  UIController.m
//  triangle
//
//  Created by jian zhang on 1/2/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "UIController.h"
#import "WhiteNoise.h"
#import "FFTImage.h"

@implementation UIController

- (id) init
{
	[super init];
	
	pieces = [NSArray arrayWithObjects:
	[[WhiteNoise alloc] init],
	[[FFTImage alloc] init],
	nil];
	
	[pieces retain];
	
	return self;
}

- (int) numberOfRowsInTableView: (NSTableView *) aTableView
{
	return [pieces count];
}

- (id) tableView: (NSTableView *) aTableView
       objectValueForTableColumn: (NSTableColumn *) aTableColumn                                                                 
       row: (int) rowIndex
{
	return [[pieces objectAtIndex: rowIndex]name];
}

- (void) tableViewSelectionDidChange: (NSNotification *) aNotification
{
	unsigned int rowindex;
	TestPiece  *piece;
	
   
   rowindex = [table_view selectedRow];
   
   if ((rowindex < 0) || (rowindex >= [pieces count])) return;
   
	piece = [pieces objectAtIndex: rowindex];
	
	[opengl_view setPiece: piece];
}

@end
