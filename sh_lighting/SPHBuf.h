#ifndef SPHBUF_H
#define SPHBUF_H

#include <vector>
#include <hash_map>
#include <hash_set>
using namespace std;

struct SPHBufPix
{
	SPHBufPix() {d= 10e8;}
	XYZ pos, nor;
	float d, facing, k;
	char is_frontFace;
	int id;
};

typedef std::pair<short, short> SPHBufCoord;

struct eqcoord
{
	size_t operator()(const SPHBufCoord& key) const
    {
        if (key.first < key.second)
            return key.first + key.second * 2;

        return key.first * 2 + key.second;
    }
  bool operator()(const SPHBufCoord& key1, const SPHBufCoord& key2) const
  {
    unsigned short key10, key11, key20, key21;
		
 key10 = key1.first;
            key11 = key1.second;
	      key20 = key2.first;
            key21 = key2.second;
	    
        if (key11 == key21)
            return key10 != key20;

        return key11 != key21;
  }
  
  enum
    {
        bucket_size = 4,
        min_buckets = 8
    };
};

typedef stdext::hash_map<SPHBufCoord, SPHBufPix, eqcoord> SPHBuf;

#endif