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

	glInited = 0;
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

- (NSArray *)getFloatAttr
{
	return float_attr_array;
}

- (void)initShaders
{
	const char *vertex_string   = [vert_source cString];
	const char *fragment_string   = [frag_source cString];
	
GLuint vertex_shader   = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_string, 0);
	glCompileShader(vertex_shader);

	GLuint fragment_shader   = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_string, 0);
	glCompileShader(fragment_shader);
	
	program = glCreateProgram();
	
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	
	glLinkProgram(program);
	
	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	
	if(linked) NSLog(@"shader compiled");
	else {
		NSLog(vert_source);
		NSLog(frag_source);
	}
}

- (void)updateUniformFloat
{
NSUInteger i;
	for (i =0; i< [float_attr_array count]; i++) { 
	FloatAttr *attr = [float_attr_array objectAtIndex:i];
	glUniform1f(glGetUniformLocation(program, [attr.modelName cString]), attr.val);
	}
}
@end
