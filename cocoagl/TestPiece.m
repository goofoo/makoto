//
//  TestPiece.m
//  triangle
//
//  Created by jian zhang on 1/2/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "TestPiece.h"

@implementation TestPiece

- (id) init
{
	[super init];
	
	return self;
}

- (NSString *) name
{
	return @"Unnamed Test";
}

- (void) draw
{

}

- (void)initGL
{
	glInited = 1;
}

- (BOOL)isGLInited
{
	return glInited;
}
@end
