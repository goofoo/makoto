#include <RslPlugin.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "CPRTData.h"

static RixMutex *s_stringLock = NULL;

void destructGlobals(RixContext *ctx, void *buffer)
{
    delete (CPRTData *)buffer; 
    if (s_stringLock) delete s_stringLock;
}

extern "C" {
	
	RSLEXPORT int shadowed_simple(RslContext* rslContext, int argc, const RslArg* argv[])
	{
		RslPointIter resArg(argv[0]);
		RslStringIter inString(argv[1]);
		RslFloatIter inExpr(argv[2]);
		RslPointIter inSide(argv[3]);
		RslPointIter inUp(argv[4]);
		RslFloatIter coeff0Arg(argv[5]);
		RslFloatIter coeff1Arg(argv[6]);
		RslFloatIter coeff2Arg(argv[7]);
		RslFloatIter coeff3Arg(argv[8]);
		RslFloatIter coeff4Arg(argv[9]);
		RslFloatIter coeff5Arg(argv[10]);
		RslFloatIter coeff6Arg(argv[11]);
		RslFloatIter coeff7Arg(argv[12]);
		RslFloatIter coeff8Arg(argv[13]);
		RslFloatIter coeff9Arg(argv[14]);
		RslFloatIter coeff10Arg(argv[15]);
		RslFloatIter coeff11Arg(argv[16]);
		RslFloatIter coeff12Arg(argv[17]);
		RslFloatIter coeff13Arg(argv[18]);
		RslFloatIter coeff14Arg(argv[19]);
		RslFloatIter coeff15Arg(argv[20]);
		
		int inLen = strlen(*inString);
		char *hdrname = new char[inLen+1]; 

		strcpy(hdrname, *inString);
		hdrname[inLen] = '\0';
		
		RixStorage* globalStorage = rslContext->GetGlobalStorage();

		globalStorage->Lock();
		
		CPRTData *tempString = (CPRTData *)globalStorage->Get("tempString");
		if (!tempString) {
			tempString = new CPRTData;
			RixThreadUtils *lockFactory = (RixThreadUtils *)rslContext->GetRixInterface(k_RixThreadUtils);
			s_stringLock = lockFactory->NewMutex();
			globalStorage->Set("tempString", (void *)tempString, destructGlobals);
		}
		
		if(tempString->isNull()) {
			XYZ side((*inSide)[0], (*inSide)[1], (*inSide)[2]);
			XYZ up((*inUp)[0], (*inUp)[1], (*inUp)[2]);
			XYZ front = side^up;
			float expr = (*inExpr);
			s_stringLock->Lock();
			tempString->initialize(hdrname, expr, side, up, front);
			 s_stringLock->Unlock();
		}
		
		globalStorage->Unlock();
		
		XYZ coeff[SH_N_BASES];
		int numVals = argv[0]->NumValues();
		for(int i=0; i<numVals; ++i) {
			
			for(int i =0; i<SH_N_BASES; i++) {
				coeff[0].x = coeff[0].y = coeff[0].z = (*coeff0Arg);
				coeff[1].x = coeff[1].y = coeff[1].z = (*coeff1Arg);
				coeff[2].x = coeff[2].y = coeff[2].z = (*coeff2Arg);
				coeff[3].x = coeff[3].y = coeff[3].z = (*coeff3Arg);
				coeff[4].x = coeff[4].y = coeff[4].z = (*coeff4Arg);
				coeff[5].x = coeff[5].y = coeff[5].z = (*coeff5Arg);
				coeff[6].x = coeff[6].y = coeff[6].z = (*coeff6Arg);
				coeff[7].x = coeff[7].y = coeff[7].z = (*coeff7Arg);
				coeff[8].x = coeff[8].y = coeff[8].z = (*coeff8Arg);
				coeff[9].x = coeff[9].y = coeff[9].z = (*coeff9Arg);
				coeff[10].x = coeff[10].y = coeff[10].z = (*coeff10Arg);
				coeff[11].x = coeff[11].y = coeff[11].z = (*coeff11Arg);
				coeff[12].x = coeff[12].y = coeff[12].z = (*coeff12Arg);
				coeff[13].x = coeff[13].y = coeff[13].z = (*coeff13Arg);
				coeff[14].x = coeff[14].y = coeff[14].z = (*coeff14Arg);
				coeff[15].x = coeff[15].y = coeff[15].z = (*coeff15Arg);
			}
			
			XYZ C;
			
			C = tempString->f_sh->computeSHLighting(coeff);
			
			(*resArg)[0] = C.x;
			(*resArg)[1] = C.y;
			(*resArg)[2] = C.z;
			
			++resArg;
			++coeff0Arg; ++coeff1Arg; ++coeff2Arg; ++coeff3Arg;
			++coeff4Arg; ++coeff5Arg; ++coeff6Arg; ++coeff7Arg;
			++coeff8Arg; ++coeff9Arg; ++coeff10Arg; ++coeff11Arg;
			++coeff12Arg; ++coeff13Arg; ++coeff14Arg; ++coeff15Arg;
		}
		return 0;
	}
	
	RSLEXPORT int shadowed_disp(RslContext* rslContext, int argc, const RslArg* argv[])
	{
		RslPointIter resArg(argv[0]);
		RslStringIter inString(argv[1]);
		RslFloatIter inExpr(argv[2]);
		RslPointIter inSide(argv[3]);
		RslPointIter inUp(argv[4]);
		RslPointIter inNorArg(argv[5]);
		RslFloatIter coeff0Arg(argv[6]);
		RslFloatIter coeff1Arg(argv[7]);
		RslFloatIter coeff2Arg(argv[8]);
		RslFloatIter coeff3Arg(argv[9]);
		RslFloatIter coeff4Arg(argv[10]);
		RslFloatIter coeff5Arg(argv[11]);
		RslFloatIter coeff6Arg(argv[12]);
		RslFloatIter coeff7Arg(argv[13]);
		RslFloatIter coeff8Arg(argv[14]);
		RslFloatIter coeff9Arg(argv[15]);
		RslFloatIter coeff10Arg(argv[16]);
		RslFloatIter coeff11Arg(argv[17]);
		RslFloatIter coeff12Arg(argv[18]);
		RslFloatIter coeff13Arg(argv[19]);
		RslFloatIter coeff14Arg(argv[20]);
		RslFloatIter coeff15Arg(argv[21]);
		
		int inLen = strlen(*inString);
		char *hdrname = new char[inLen+1]; 

		strcpy(hdrname, *inString);
		hdrname[inLen] = '\0';
		
		RixStorage* globalStorage = rslContext->GetGlobalStorage();

		globalStorage->Lock();
		
		CPRTData *tempString = (CPRTData *)globalStorage->Get("tempString");
		if (!tempString) {
			tempString = new CPRTData;
			RixThreadUtils *lockFactory = (RixThreadUtils *)rslContext->GetRixInterface(k_RixThreadUtils);
			s_stringLock = lockFactory->NewMutex();
			globalStorage->Set("tempString", (void *)tempString, destructGlobals);
		}
		
		if(tempString->isNull()) {
			XYZ side((*inSide)[0], (*inSide)[1], (*inSide)[2]);
			XYZ up((*inUp)[0], (*inUp)[1], (*inUp)[2]);
			XYZ front = side^up;
			float expr = (*inExpr);
			s_stringLock->Lock();
			tempString->initialize(hdrname, expr, side, up, front);
			 s_stringLock->Unlock();
		}
		
		globalStorage->Unlock();

		XYZ coeff[SH_N_BASES];
		XYZ nor;
		int numVals = argv[0]->NumValues();
		for(int i=0; i<numVals; ++i) {
			
			for(int i =0; i<SH_N_BASES; i++) {
				coeff[0].x = coeff[0].y = coeff[0].z = (*coeff0Arg);
				coeff[1].x = coeff[1].y = coeff[1].z = (*coeff1Arg);
				coeff[2].x = coeff[2].y = coeff[2].z = (*coeff2Arg);
				coeff[3].x = coeff[3].y = coeff[3].z = (*coeff3Arg);
				coeff[4].x = coeff[4].y = coeff[4].z = (*coeff4Arg);
				coeff[5].x = coeff[5].y = coeff[5].z = (*coeff5Arg);
				coeff[6].x = coeff[6].y = coeff[6].z = (*coeff6Arg);
				coeff[7].x = coeff[7].y = coeff[7].z = (*coeff7Arg);
				coeff[8].x = coeff[8].y = coeff[8].z = (*coeff8Arg);
				coeff[9].x = coeff[9].y = coeff[9].z = (*coeff9Arg);
				coeff[10].x = coeff[10].y = coeff[10].z = (*coeff10Arg);
				coeff[11].x = coeff[11].y = coeff[11].z = (*coeff11Arg);
				coeff[12].x = coeff[12].y = coeff[12].z = (*coeff12Arg);
				coeff[13].x = coeff[13].y = coeff[13].z = (*coeff13Arg);
				coeff[14].x = coeff[14].y = coeff[14].z = (*coeff14Arg);
				coeff[15].x = coeff[15].y = coeff[15].z = (*coeff15Arg);
			}
			
			nor.x =(*inNorArg)[0];
			nor.y =(*inNorArg)[1];
			nor.z =(*inNorArg)[2];
			
			XYZ C;
			
			s_stringLock->Lock();
			C = tempString->f_sh->computeSHLighting(coeff, nor);
			s_stringLock->Unlock();
			
			(*resArg)[0] = C.x;
			(*resArg)[1] = C.y;
			(*resArg)[2] = C.z;
			
			++resArg;
			++inNorArg;
			++coeff0Arg; ++coeff1Arg; ++coeff2Arg; ++coeff3Arg;
			++coeff4Arg; ++coeff5Arg; ++coeff6Arg; ++coeff7Arg;
			++coeff8Arg; ++coeff9Arg; ++coeff10Arg; ++coeff11Arg;
			++coeff12Arg; ++coeff13Arg; ++coeff14Arg; ++coeff15Arg;
		}
		return 0;
	}
	
	RSLEXPORT int occ_disp(RslContext* rslContext, int argc, const RslArg* argv[])
	{
		RslFloatIter resArg(argv[0]);
		RslPointIter inNorArg(argv[1]);
		RslFloatIter coeff0Arg(argv[2]);
		RslFloatIter coeff1Arg(argv[3]);
		RslFloatIter coeff2Arg(argv[4]);
		RslFloatIter coeff3Arg(argv[5]);
		RslFloatIter coeff4Arg(argv[6]);
		RslFloatIter coeff5Arg(argv[7]);
		RslFloatIter coeff6Arg(argv[8]);
		RslFloatIter coeff7Arg(argv[9]);
		RslFloatIter coeff8Arg(argv[10]);
		RslFloatIter coeff9Arg(argv[11]);
		RslFloatIter coeff10Arg(argv[12]);
		RslFloatIter coeff11Arg(argv[13]);
		RslFloatIter coeff12Arg(argv[14]);
		RslFloatIter coeff13Arg(argv[15]);
		RslFloatIter coeff14Arg(argv[16]);
		RslFloatIter coeff15Arg(argv[17]);
		
		RixStorage* globalStorage = rslContext->GetGlobalStorage();

		globalStorage->Lock();
		
		CPRTData *tempString = (CPRTData *)globalStorage->Get("tempString");
		if (!tempString) {
			tempString = new CPRTData;
			RixThreadUtils *lockFactory = (RixThreadUtils *)rslContext->GetRixInterface(k_RixThreadUtils);
			s_stringLock = lockFactory->NewMutex();
			globalStorage->Set("tempString", (void *)tempString, destructGlobals);
		}

		globalStorage->Unlock();

		XYZ coeff[SH_N_BASES];
		XYZ nor;
		int numVals = argv[0]->NumValues();
		for(int i=0; i<numVals; ++i) {
			
			for(int i =0; i<SH_N_BASES; i++) {
				coeff[0].x = coeff[0].y = coeff[0].z = (*coeff0Arg);
				coeff[1].x = coeff[1].y = coeff[1].z = (*coeff1Arg);
				coeff[2].x = coeff[2].y = coeff[2].z = (*coeff2Arg);
				coeff[3].x = coeff[3].y = coeff[3].z = (*coeff3Arg);
				coeff[4].x = coeff[4].y = coeff[4].z = (*coeff4Arg);
				coeff[5].x = coeff[5].y = coeff[5].z = (*coeff5Arg);
				coeff[6].x = coeff[6].y = coeff[6].z = (*coeff6Arg);
				coeff[7].x = coeff[7].y = coeff[7].z = (*coeff7Arg);
				coeff[8].x = coeff[8].y = coeff[8].z = (*coeff8Arg);
				coeff[9].x = coeff[9].y = coeff[9].z = (*coeff9Arg);
				coeff[10].x = coeff[10].y = coeff[10].z = (*coeff10Arg);
				coeff[11].x = coeff[11].y = coeff[11].z = (*coeff11Arg);
				coeff[12].x = coeff[12].y = coeff[12].z = (*coeff12Arg);
				coeff[13].x = coeff[13].y = coeff[13].z = (*coeff13Arg);
				coeff[14].x = coeff[14].y = coeff[14].z = (*coeff14Arg);
				coeff[15].x = coeff[15].y = coeff[15].z = (*coeff15Arg);
			}
			
			nor.x =(*inNorArg)[0];
			nor.y =(*inNorArg)[1];
			nor.z =(*inNorArg)[2];
			
			
			float occ =1.f;
			
			s_stringLock->Lock();
			occ= tempString->f_sh->computeSrcSHLighting(coeff, nor);
			s_stringLock->Unlock();
			 
			(*resArg) = occ/3.1415927;
			
			++resArg;
			++inNorArg;
			++coeff0Arg; ++coeff1Arg; ++coeff2Arg; ++coeff3Arg;
			++coeff4Arg; ++coeff5Arg; ++coeff6Arg; ++coeff7Arg;
			++coeff8Arg; ++coeff9Arg; ++coeff10Arg; ++coeff11Arg;
			++coeff12Arg; ++coeff13Arg; ++coeff14Arg; ++coeff15Arg;
		}
		return 0;
	}
	
	RSLEXPORT int shadowed_triple(RslContext* rslContext, int argc, const RslArg* argv[])
	{
		RslPointIter resArg(argv[0]);
		RslStringIter inString(argv[1]);
		RslFloatIter inExpr(argv[2]);
		RslPointIter inSide(argv[3]);
		RslPointIter inUp(argv[4]);
		RslPointIter coeff0Arg(argv[5]);
		RslPointIter coeff1Arg(argv[6]);
		RslPointIter coeff2Arg(argv[7]);
		RslPointIter coeff3Arg(argv[8]);
		RslPointIter coeff4Arg(argv[9]);
		RslPointIter coeff5Arg(argv[10]);
		RslPointIter coeff6Arg(argv[11]);
		RslPointIter coeff7Arg(argv[12]);
		RslPointIter coeff8Arg(argv[13]);
		RslPointIter coeff9Arg(argv[14]);
		RslPointIter coeff10Arg(argv[15]);
		RslPointIter coeff11Arg(argv[16]);
		RslPointIter coeff12Arg(argv[17]);
		RslPointIter coeff13Arg(argv[18]);
		RslPointIter coeff14Arg(argv[19]);
		RslPointIter coeff15Arg(argv[20]);
		
		int inLen = strlen(*inString);
		char *hdrname = new char[inLen+1]; 

		strcpy(hdrname, *inString);
		hdrname[inLen] = '\0';
		
		RixStorage* globalStorage = rslContext->GetGlobalStorage();

		globalStorage->Lock();
		
		CPRTData *tempString = (CPRTData *)globalStorage->Get("tempString");
		if (!tempString) {
			tempString = new CPRTData;
			RixThreadUtils *lockFactory = (RixThreadUtils *)rslContext->GetRixInterface(k_RixThreadUtils);
			s_stringLock = lockFactory->NewMutex();
			globalStorage->Set("tempString", (void *)tempString, destructGlobals);
		}
		
		if(tempString->isNull()) {
			XYZ side((*inSide)[0], (*inSide)[1], (*inSide)[2]);
			XYZ up((*inUp)[0], (*inUp)[1], (*inUp)[2]);
			XYZ front = side^up;
			float expr = (*inExpr);
			s_stringLock->Lock();
			tempString->initialize(hdrname, expr, side, up, front);
			 s_stringLock->Unlock();
		}
		
		globalStorage->Unlock();
		
		XYZ coeff[SH_N_BASES];
		int numVals = argv[0]->NumValues();
		for(int i=0; i<numVals; ++i) {
			
			for(int i =0; i<SH_N_BASES; i++) {
				coeff[0].x = (*coeff0Arg)[0];
				coeff[0].y = (*coeff0Arg)[1];
				coeff[0].z = (*coeff0Arg)[2];
				coeff[1].x = (*coeff1Arg)[0];
				coeff[1].y = (*coeff1Arg)[1];
				coeff[1].z = (*coeff1Arg)[2];
				coeff[2].x = (*coeff2Arg)[0];
				coeff[2].y = (*coeff2Arg)[1];
				coeff[2].z = (*coeff2Arg)[2];
				coeff[3].x = (*coeff3Arg)[0];
				coeff[3].y = (*coeff3Arg)[1];
				coeff[3].z = (*coeff3Arg)[2];
				coeff[4].x = (*coeff4Arg)[0];
				coeff[4].y = (*coeff4Arg)[1];
				coeff[4].z = (*coeff4Arg)[2];
				coeff[5].x = (*coeff5Arg)[0];
				coeff[5].y = (*coeff5Arg)[1];
				coeff[5].z = (*coeff5Arg)[2];
				coeff[6].x = (*coeff6Arg)[0];
				coeff[6].y = (*coeff6Arg)[1];
				coeff[6].z = (*coeff6Arg)[2];
				coeff[7].x = (*coeff7Arg)[0];
				coeff[7].y = (*coeff7Arg)[1];
				coeff[7].z = (*coeff7Arg)[2];
				coeff[8].x = (*coeff8Arg)[0];
				coeff[8].y = (*coeff8Arg)[1];
				coeff[8].z = (*coeff8Arg)[2];
				coeff[9].x = (*coeff9Arg)[0];
				coeff[9].y = (*coeff9Arg)[1];
				coeff[9].z = (*coeff9Arg)[2];
				coeff[10].x = (*coeff10Arg)[0];
				coeff[10].y = (*coeff10Arg)[1];
				coeff[10].z = (*coeff10Arg)[2];
				coeff[11].x = (*coeff11Arg)[0];
				coeff[11].y = (*coeff11Arg)[1];
				coeff[11].z = (*coeff11Arg)[2];
				coeff[12].x = (*coeff12Arg)[0];
				coeff[12].y = (*coeff12Arg)[1];
				coeff[12].z = (*coeff12Arg)[2];
				coeff[13].x = (*coeff13Arg)[0];
				coeff[13].y = (*coeff13Arg)[1];
				coeff[13].z = (*coeff13Arg)[2];
				coeff[14].x = (*coeff14Arg)[0];
				coeff[14].y = (*coeff14Arg)[1];
				coeff[14].z = (*coeff14Arg)[2];
				coeff[15].x = (*coeff15Arg)[0];
				coeff[15].y = (*coeff15Arg)[1];
				coeff[15].z = (*coeff15Arg)[2];
			}
			
			XYZ C;
			
			C = tempString->f_sh->computeSHLighting(coeff);
			
			(*resArg)[0] = C.x;
			(*resArg)[1] = C.y;
			(*resArg)[2] = C.z;
			
			++resArg;
			++coeff0Arg; ++coeff1Arg; ++coeff2Arg; ++coeff3Arg;
			++coeff4Arg; ++coeff5Arg; ++coeff6Arg; ++coeff7Arg;
			++coeff8Arg; ++coeff9Arg; ++coeff10Arg; ++coeff11Arg;
			++coeff12Arg; ++coeff13Arg; ++coeff14Arg; ++coeff15Arg;
		}
		return 0;
	}
	
	static RslFunction shfunc[] = {
		{"color SHLighting(string, float, vector, vector, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float)", shadowed_simple, NULL, NULL},
		{"color SHLighting(string, float, vector, vector, normal, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float)", shadowed_disp, NULL, NULL},
		{"float SHLighting(normal, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float)", occ_disp, NULL, NULL},
		{"color SHLighting(string, float, vector, vector, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color, color)", shadowed_triple, NULL, NULL},
		NULL
	};
	
	RSLEXPORT RslFunctionTable RslPublicFunctions = shfunc;
	
};
