#ifndef YIELDMPICOMMBUFFER_H_
#define YIELDMPICOMMBUFFER_H_

#include <mpi.h>

class YieldMPICommBuffer : public CommBuffer
{
public:
	YieldMPICommBuffer(int capacity, MPI::Intracomm c);
	virtual ~YieldMPICommBuffer();
	
	int pack(int* data, int count);
	int pack(char* data, int count);
	
	int unpack(int* data, int count);
	int unpack(char* data, int count);
	
	void setSize(int newSize) { size = newSize; }
	
	int   getSize()          { return size; }
	int   getCapacity()		 { return capacity; }
	void* getBufferPointer() { return buffer; }
	
	int growTo(int newSize) {
		if (buffer == NULL) {
			buffer = malloc(newSize);
		} else {
			buffer = realloc(buffer, newSize);
		}
		capacity = newSize;
		
		return newSize;
	}

private:
	void*          buffer;
	unsigned int   unpacked;
	unsigned int   size;
	unsigned int   capacity;
	MPI::Intracomm comm;
};

#endif /*MPICOMMBUFFER_H_*/
