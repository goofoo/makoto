//
//  FFTImage.h
//  triangle
//
//  Created by jian zhang on 1/2/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "TestPiece.h"

@interface FFTImage : TestPiece {
	GLuint teximg;
	GLuint fftimg;
}
- (id) init;
- (NSString *) name;
- (void) draw;
- (void)initGL;
@end
