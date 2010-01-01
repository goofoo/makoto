/*
 *  image.h
 *  triangle
 *
 *  Created by jian zhang on 1/1/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
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

void checkEXRDim(const char* filename, int* width, int* height);
void readEXRRED(const char* filename, int width, int height, float* data);

#ifdef __cplusplus
}
#endif
