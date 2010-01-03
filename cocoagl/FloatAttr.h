//
//  FloatAttr.h
//  cocoagl
//
//  Created by jian zhang on 1/3/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface FloatAttr : NSObject {
	float val, min, max;
	NSString *modelName;
}

@property(copy, readwrite) NSString *modelName;
@property(readwrite) float val;
@property(readwrite) float min;
@property(readwrite) float max;

- (id)init;

@end
