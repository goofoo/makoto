#ifndef _Z_GLOBAL_H
#define _Z_GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <time.h>

class zGlobal
{
public:
		zGlobal() {}
		~zGlobal() {}
		
		static void divideByFirstSpace(std::string& ab2a, std::string& b);
		static void trimByFirstSpace(std::string& res);
		static void getTime(std::string& log);
		static void cutByFirstDot(std::string& res);
		static void cutByLastDot(std::string& res);
		static void cutByLastSlash(std::string& res);
		static void changeFrameNumber(std::string& res, int frame);
		static int safeConvertToInt(const double a);
		static int getFrameNumber(std::string& name);
		static int compareFilenameExtension(std::string& name, const char* other);
		static void setFrameNumberAndExtension(std::string& name, int frame, const char* extension);
		static int getFrameNumber(const std::string& name);
		static void changeFilenameExtension(std::string& name, const char* ext);
		static void validateFilePath(std::string& name);
		static void replacefilename(std::string& res, std::string& toreplace);
		static void cutfilepath(std::string& res);
		static void saveFloatFile(const char* filename, const int nfloat, const float* data);
		static void changeFrameNumberFistDot4Digit(std::string& res, int frame);
};
#endif
//:~