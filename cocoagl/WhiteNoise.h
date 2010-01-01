//
//  WhiteNoise.h
//  triangle
//
//  Created by jian zhang on 1/2/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "TestPiece.h"

@interface WhiteNoise : TestPiece {
	GLuint texname;
	int poolw, poolh;
}

- (id) init;
- (NSString *) name;
- (void)draw;
- (void)initGL;

@end
