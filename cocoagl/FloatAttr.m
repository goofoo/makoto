//
//  FloatAttr.m
//  cocoagl
//
//  Created by jian zhang on 1/3/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "FloatAttr.h"


@implementation FloatAttr

@synthesize modelName;
@synthesize val;
@synthesize min;
@synthesize max;

- (id)init
{
	[super init];
	
	modelName = @"untitled";
	
	val = 0.5f;
	min = 0.f;
	max = 1.f;
	
	return self;
}

@end
