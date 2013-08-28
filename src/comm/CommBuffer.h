#ifndef COMMBUFFER_H_
#define COMMBUFFER_H_

class CommBuffer
{
public:
	// dtor
	virtual ~CommBuffer() {};
	
	virtual int pack(int* data, int count)=0;
	virtual int pack(char* data, int count)=0;
	
	virtual int unpack(int* data, int count)=0;
	virtual int unpack(char* data, int count)=0;
	
	virtual int getSize()=0;
};

#endif /*COMMBUFFER_H_*/
