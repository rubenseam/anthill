#ifndef YIELDMPICOMM_H_
#define YIELDMPICOMM_H_

#include <mpi.h>
#include <assert.h>
#include "CommLayer.h"
#include "BcastGroup.h"
#include "YieldMPICommBuffer.h"

#define YIELD_TIME 1000 ///< Time that current process will sleep when waiting for messages


/** MPI implementation of CommLayer that forcefully yield cpu control when waiting for messages.
 *  	\todo Use MPI::Init_thread() and become thread-safe.
 */ 
class YieldMPIComm : public CommLayer
{
public:
	YieldMPIComm(int& processArgc, char**& processArgv);
	virtual ~YieldMPIComm();
	
	int spwan(const string command, const char** argv, const int nprocess, const char** where, 
		int& spawned, int* errors=NULL);
	
	int send(int to, void* data, int size, int tag);
	int sendBuffer(int to, CommBuffer* buf, int tag);
	int recv(int from, void* data, int size, int tag);
	int recvBuffer(int from, CommBuffer* buf, int tag);
	int probe(int& from, int& tag);
	int poll(int& from, int& tag);
	
	int getNumProcess() { return nProcess; }
	int getMyRank()     { return allProcess.Get_rank(); }
	
	string getHost() {
		int   nameLen  = -1;
		char* hostName = (char*) malloc(MPI_MAX_PROCESSOR_NAME+1);
		hostName[MPI_MAX_PROCESSOR_NAME] = '\0';

		MPI::Get_processor_name(hostName, nameLen);
		string ret = hostName;

		free(hostName);
		return ret;
	}
	
	CommBuffer* createBuffer(int capacity) {
		YieldMPICommBuffer *mpiBuf = new YieldMPICommBuffer(capacity, allProcess);
		CommBuffer    *buf    = mpiBuf; 
		return buf;
	}
	
private:
	static int            numberOfClassInstances;
	static MPI::Intracomm allProcess;
	int			          nProcess;
};


#endif /*YIELDMPICOMM_H_*/
