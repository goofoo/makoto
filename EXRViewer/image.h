/*
 *  image.h
 *  EXRViewer
 *
 *  Created by jian zhang on 6/19/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef IMAGE_H
#define IMAGE_H

#ifdef __cplusplus

#include <ImfRgbaFile.h>
#include <ImfChannelList.h>
#include <ImfHeader.h>
#include <ImfOutputFile.h>
#include <ImfInputFile.h>
#include <ImfStringAttribute.h>
#include <ImfMatrixAttribute.h>
#include <ImfArray.h>

using namespace Imf;
using namespace Imath;

#endif

#ifdef __cplusplus
extern "C" {
#endif

void loadEXR(const char* filename, int* width, int* height);
void readEXR(const char* filename, int width, int height, float* data);

#ifdef __cplusplus
}
#endif

#endif