#include <stdlib.h>
#include <stdio.h>
#include "pdcfile.h"



void swapInt( char * theInt )
{
	char swap[4];

	swap[0] = theInt[0];
	swap[1] = theInt[1];
	swap[2] = theInt[2];
	swap[3] = theInt[3];

	theInt[0] = swap[3];
	theInt[1] = swap[2];
	theInt[2] = swap[1];
	theInt[3] = swap[0];
}

void swapDouble(char * theDouble )
{
	char swap;
	for (int i=0; i<4; i++ )
	{
		swap = theDouble[7-i];
		theDouble[7-i] = theDouble[i];
		theDouble[i] = swap;
	}
}

pdcFile::pdcFile():data(0),m_num_of_ptc(0)
{
}

pdcFile::~pdcFile(void)
{
	if(data) delete[] data;
}


int pdcFile::load(const char* filename)
{
	FILE *fp = fopen( filename, "rb");

	if( fp==NULL ) return 0;
	
	int filesize;
	fseek( fp, 0, SEEK_END );
	filesize = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	
	data = new char[filesize];
	int stat = fread(data,filesize,1,fp);
	fclose(fp);
	
	pheader = (pdcHeader*) data;
	swapInt((char*)&pheader->byteOrder);
	
	if(pheader->byteOrder !=0)
	{
		swapInt( (char*) &pheader->formatVersion );
		swapInt( (char*) &pheader->extra1 );
		swapInt( (char*) &pheader->extra2 );
		swapInt( (char*) &pheader->numParticles );
		swapInt( (char*) &pheader->numAttributes );
	}
	
	m_num_of_ptc = pheader->numParticles;
	//MGlobal::displayInfo(MString("nptc ")+num_ptc);
	if(pheader->numParticles<1) return 0;
	
	
	
	return stat;
}

int pdcFile::readPositionAndVelocity(MVectorArray& positions, MVectorArray& velocities)
{
	positions.clear();
	velocities.clear();
	positions.setLength(m_num_of_ptc);
	velocities.setLength(m_num_of_ptc);
	char * record = (char*) pheader + sizeof(struct pdcHeader);
	int attribNameLen;
	
	for (int i=0; i<pheader->numAttributes; i++ )
	{
		if(pheader->byteOrder!=0) swapInt( record );
		attribNameLen = (int) *record;
		
		record += 4;
		
		char* attribName = new char[attribNameLen + 1];
		for (int j=0; j<attribNameLen; j++ )
		{
			attribName[j] = record[j];
		}
		attribName[attribNameLen] = '\0';  

		record += attribNameLen;
		
		if(pheader->byteOrder!=0) swapInt( record );
		int attribType = (int) *record;
		//printf( "  attribType = %d  ", attribType );
		record += 4; //Move record pointer
		
		//MGlobal::displayInfo(MString("type ") + attribType);
		
			switch ( attribType )
			{
				case 0: // Integer
	
						record += 4; //Move record pointer to next element
						
					break;
	
				case 1: // Integer Array
	
						record += 4*pheader->numParticles; //Move record pointer to next element
						
					break;
	
				case 2: // Double
	
						record += 8; //Move record pointer to next element
						
					break;
	
				case 3: // Double Array
					record += 8*pheader->numParticles; //Move record pointer to next element
					break;
	
				case 4: // Vector
						record += 8*3; //Move record pointer to next element
	
					break;
	
				case 5: // Vector Array
					if(MString(attribName) == MString("position"))
					{
						for(int j=0; j<pheader->numParticles; j++)
						{
							for(int k=0; k<3; k++)
							{		
								if(pheader->byteOrder!=0) swapDouble( record );

								memcpy(&positions[j][k], record, sizeof(double));

								record += 8; //Move record pointer to next element
							}
						}
					}
					else if(MString(attribName) == MString("velocity"))
					{
						for(int j=0; j<pheader->numParticles; j++)
						{
							for(int k=0; k<3; k++)
							{
								
								if(pheader->byteOrder!=0) swapDouble( record );

								memcpy(&velocities[j][k], record, sizeof(double));

								record += 8; //Move record pointer to next element
							}
						}
					}
					else record += 8*3*pheader->numParticles;
					break;
	
				default:  // Unknown
					//printf( " Unkown Type\n"); 
					//printf( "If ghostFrames is the last attirbute showing Unknown Type\n"); 
					//printf( "All contents have been displayed correctly\n" );
					//printf( "\"ghostFrames\" are usually at the end of the file and don't seem to contain anything\n" );
					//exit( 1 );
					break;
			}
		delete[] attribName;
	}

	return 0;
}

int pdcFile::readPositions(MVectorArray& positions, MVectorArray& velocities, MVectorArray& ups, MVectorArray& fronts, MDoubleArray& offsets, MDoubleArray& amplitudes, MDoubleArray& bends, MDoubleArray& scales, MDoubleArray& masses)
{
	positions.clear();
	velocities.clear();
	ups.clear();
	fronts.clear();
	offsets.clear();
	amplitudes.clear();
	bends.clear();
	scales.clear();
	masses.clear();
	for(int i=0; i<m_num_of_ptc;i++)
	{
		positions.append(MVector(0,0,0));
		velocities.append(MVector(0,0,0));
		ups.append(MVector(0,1,0));
		fronts.append(MVector(0,0,1));
		offsets.append(0.0);
		amplitudes.append(0.0);
		bends.append(0.0);
		scales.append(1.0);
		masses.append(1.0);
	}
	
	char * record = (char*) pheader + sizeof(struct pdcHeader);
	int attribNameLen;
	
	for (int i=0; i<pheader->numAttributes; i++ )
	{
		if(pheader->byteOrder!=0) swapInt( record );
		attribNameLen = (int) *record;
		
		record += 4;
		
		char* attribName = new char[attribNameLen + 1];
		for (int j=0; j<attribNameLen; j++ )
		{
			attribName[j] = record[j];
		}
		attribName[attribNameLen] = '\0';  
		
		

		//MGlobal::displayInfo(MString("name ") + attribName);

		
		
		record += attribNameLen;
		
		if(pheader->byteOrder!=0) swapInt( record );
		int attribType = (int) *record;
		//printf( "  attribType = %d  ", attribType );
		record += 4; //Move record pointer
		
		//MGlobal::displayInfo(MString("type ") + attribType);
		
			switch ( attribType )
			{
				case 0: // Integer
	
						record += 4; //Move record pointer to next element
						
					break;
	
				case 1: // Integer Array
	
						record += 4*pheader->numParticles; //Move record pointer to next element
						
					break;
	
				case 2: // Double
	
						record += 8; //Move record pointer to next element
						
					break;
	
				case 3: // Double Array
					if(MString(attribName) == MString("flapOffset"))
					{
						for(int j=0; j<pheader->numParticles; j++)
						{
							if(pheader->byteOrder!=0) swapDouble( record );
							memcpy(&offsets[j], record, sizeof(double));
							record += 8; //Move record pointer to next element
						}
					}
					else if(MString(attribName) == MString("flapAmplitude"))
					{
						for(int j=0; j<pheader->numParticles; j++)
						{
							if(pheader->byteOrder!=0) swapDouble( record );
							memcpy(&amplitudes[j], record, sizeof(double));
							record += 8; //Move record pointer to next element
						}
					}
					else if(MString(attribName) == MString("bend"))
					{
						for(int j=0; j<pheader->numParticles; j++)
						{
							if(pheader->byteOrder!=0) swapDouble( record );
							memcpy(&bends[j], record, sizeof(double));
							record += 8; //Move record pointer to next element
						}
					}
					else if(MString(attribName) == MString("scalePP"))
					{
						for(int j=0; j<pheader->numParticles; j++)
						{
							if(pheader->byteOrder!=0) swapDouble( record );
							memcpy(&scales[j], record, sizeof(double));
							//MGlobal::displayInfo(MString("frq: ")+scales[j]);
							record += 8; //Move record pointer to next element
						}
					}
					else record += 8*pheader->numParticles; //Move record pointer to next element
					break;
	
				case 4: // Vector
						record += 8*3; //Move record pointer to next element
	
					break;
	
				case 5: // Vector Array
					if(MString(attribName) == MString("position"))
					{
						for(int j=0; j<pheader->numParticles; j++)
						{
							for(int k=0; k<3; k++)
							{		
								if(pheader->byteOrder!=0) swapDouble( record );

								memcpy(&positions[j][k], record, sizeof(double));

								record += 8; //Move record pointer to next element
							}
						}
					}
					else if(MString(attribName) == MString("velocity"))
					{
						for(int j=0; j<pheader->numParticles; j++)
						{
							for(int k=0; k<3; k++)
							{
								
								if(pheader->byteOrder!=0) swapDouble( record );

								memcpy(&velocities[j][k], record, sizeof(double));

								record += 8; //Move record pointer to next element
							}
						}
					}
					else if(MString(attribName) == MString("upVector"))
					{
						for(int j=0; j<pheader->numParticles; j++)
						{
							for(int k=0; k<3; k++)
							{
								if(pheader->byteOrder!=0) swapDouble( record );
								memcpy(&ups[j][k], record, sizeof(double));
								
								//
								//
								record += 8; //Move record pointer to next element
							}
						}
					}
					else if(MString(attribName) == MString("view"))
					{
						for(int j=0; j<pheader->numParticles; j++)
						{
							for(int k=0; k<3; k++)
							{
								if(pheader->byteOrder!=0) swapDouble( record );
								memcpy(&fronts[j][k], record, sizeof(double));
								
								
								
								record += 8; //Move record pointer to next element
							}
						}
					}
					else record += 8*3*pheader->numParticles;
					break;
	
				default:  // Unknown
					//printf( " Unkown Type\n"); 
					//printf( "If ghostFrames is the last attirbute showing Unknown Type\n"); 
					//printf( "All contents have been displayed correctly\n" );
					//printf( "\"ghostFrames\" are usually at the end of the file and don't seem to contain anything\n" );
					//exit( 1 );
					break;
			}
		delete[] attribName;
	}
	
	
	return 0;
}