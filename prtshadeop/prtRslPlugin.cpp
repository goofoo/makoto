#include <RslPlugin.h>
#include <stdlib.h>

void initFoo(RixContext *ctx)
{
	
}

extern "C" {
	RSLEXPORT int ffoo(RslContext* rslContext, int argc, const RslArg* argv[])
	{
		RslFloatIter retArg(argv[0]);
		RslPointIter pointArg(argv[1]);
		
		float noi = float(rand()%131)/131.f;
		
		int numVals = argv[0]->NumValues();
		for(int i=0; i<numVals; ++i) {
			
			*retArg = noi;
			++retArg;++pointArg;
		}
		return 0;
	}
	
	RSLEXPORT int cfoo(RslContext* rslContext, int argc, const RslArg* argv[])
	{
		RslPointIter retArg(argv[0]);
		RslPointIter pointArg(argv[1]);
		
		float noix = float(rand()%431)/431.f;
		float noiy = float(rand()%301)/301.f;
		float noiz = float(rand()%271)/271.f;
		
		int numVals = argv[0]->NumValues();
		for(int i=0; i<numVals; ++i) {
			
			(*retArg)[0] = noix;
			(*retArg)[1] = noiy;
			(*retArg)[2] = noiz;
			++retArg;++pointArg;
		}
		return 0;
	}
	
	static RslFunction foofunc[] = {
		{"float foopic(point)", ffoo, NULL, NULL},
		{"color foopic(point)", cfoo, NULL, NULL},
		NULL
	};
	
	RSLEXPORT RslFunctionTable RslPublicFunctions(foofunc, initFoo);
	
};
