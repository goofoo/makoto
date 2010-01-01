//
//  TestPiece.h
//  triangle
//
//  Created by jian zhang on 1/2/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import <OpenGL/gl.h>
#import <OpenGL/glext.h>
#import <OpenGL/glu.h>
#import <OpenGL/OpenGL.h>

@interface TestPiece : NSObject {
	BOOL glInited;
}

- (id) init;
- (NSString *) name;
- (void) draw;
- (void)initGL;
- (BOOL)isGLInited;
@end
