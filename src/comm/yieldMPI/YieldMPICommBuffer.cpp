#include <stdlib.h>
#include "CommLayer.h"
#include "YieldMPICommBuffer.h"

YieldMPICommBuffer::YieldMPICommBuffer(int cap, MPI::Intracomm c)
:	unpacked(0),
	size(0),
	capacity(cap),
	comm(c)
{
	if (capacity > 0) {
		buffer = malloc(capacity);
	} else {
		buffer = NULL;
	} 
}

YieldMPICommBuffer::~YieldMPICommBuffer()
{
	if (buffer != NULL) free(buffer);
}

int YieldMPICommBuffer::pack(int* data, int count) {
	if (count <= 0)   return COMM_ERR_BAD_PARAMETER;
	if (data == NULL) return COMM_ERR_BAD_PARAMETER;
	
	// grow buffer if necessary
	if (size+(count*sizeof(*data)) > capacity) {
		int newSize = size + (count * sizeof(*data));
		
		this->growTo(newSize);
	}
	
	// use the C function, cause in c++, we hava to create a Datatype object to do this operation
	MPI_Comm c_comm = comm; 
	MPI_Pack(data, count, MPI_INT, buffer, capacity, (int*) &size, c_comm);
	
	return count;
}

int YieldMPICommBuffer::pack(char* data, int count) {
	if (count <= 0)   return COMM_ERR_BAD_PARAMETER;
	if (data == NULL) return COMM_ERR_BAD_PARAMETER;
	
	// grow buffer if necessary
	if (size+(count*sizeof(*data)) > capacity) {
		int newSize = size + (count * sizeof(*data));
		
		this->growTo(newSize);
	}
	
	// use the C function, cause in c++, we hava to create a Datatype object to do this operation
	MPI_Comm c_comm = comm; 
	MPI_Pack(data, count, MPI_BYTE, buffer, capacity, (int*) &size, c_comm);
	
	return count;
}
	
int YieldMPICommBuffer::unpack(int* data, int count) {
	if (count <= 0)   return COMM_ERR_BAD_PARAMETER;
	if (data == NULL) return COMM_ERR_BAD_PARAMETER;
	
	// use the C function, cause in c++, we hava to create a Datatype object to do this operation
	MPI_Comm c_comm = comm; 
	MPI_Unpack(buffer, capacity, (int*) &unpacked, data, count, MPI_INT, c_comm);
	
	/// \todo check if buffer ends
	
	return count;
}

int YieldMPICommBuffer::unpack(char* data, int count) {
	if (count <= 0)   return COMM_ERR_BAD_PARAMETER;
	if (data == NULL) return COMM_ERR_BAD_PARAMETER;
	
	// use the C function, cause in c++, we hava to create a Datatype object to do this operation
	MPI_Comm c_comm = comm; 
	MPI_Unpack(buffer, capacity, (int*) &unpacked, data, count, MPI_BYTE, c_comm);
	
	/// \todo check if buffer ends
	
	return count;
}
