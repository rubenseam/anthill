#include <iostream>
#include <unistd.h>
#include <mpi.h>
#include "YieldMPIComm.h"

using namespace std;

// Static class fields initialization
int            YieldMPIComm::numberOfClassInstances = 0;
MPI::Intracomm YieldMPIComm::allProcess             = MPI_COMM_NULL;


YieldMPIComm::YieldMPIComm(int& processArgc, char**& processArgv) {
	
	// the first instance must init MPI and allProcess communicator
	if (0 == numberOfClassInstances) {
		MPI::Init(processArgc, processArgv);

		// if this process was spawned, get the parent intercommunicator
		MPI::Intercomm parent = MPI::Comm::Get_parent();
		
		// if this process was spawned
		if (MPI::COMM_NULL != parent) {
			// create a intracommunicator that include the parent
			allProcess = parent.Merge(true);
		} else {
			// if this process wasn't spawned
			allProcess = MPI::COMM_WORLD;
		}
	}
	numberOfClassInstances++;
	
	nProcess = allProcess.Get_size();
}

YieldMPIComm::~YieldMPIComm() {
	numberOfClassInstances--;
	
	// the last instance must finalize MPI
	if (0 == numberOfClassInstances) {
		// only destroy allProcess if it is a new communicator 
		if (allProcess != MPI::COMM_WORLD) {
			allProcess.Free();
		}
		MPI::Finalize();
	}
}

int YieldMPIComm::spwan(const string command, const char** argv, const int nprocess, const char** where, int& spawned, int* errors) {
	MPI::Info spawnInfo = MPI::INFO_NULL;
	int toSpawn = -1;

	if (where == NULL) {
		// spawnInfo already is MPI::INFO_NULL			
		toSpawn = nprocess;
	} else {
		map<string, int> machines;
		
		for (int i=0; i<nprocess; i++) {
			machines[where[i]]++;	
	 	}
		
		int numMachines = machines.size();
		
		// hosts string will be like "host1,host2,host3"
		map<string, int>::iterator it = machines.begin();
		string hosts                  = it->first;                
		int maxInstancesPerMachine    = it->second;
		
		for (it++; it != machines.end(); it++) {
			// generate the hosts string
			hosts.append(",");
			hosts.append(it->first);
			
			// find the machine that has most instances 
			if (it->second > maxInstancesPerMachine) {
				maxInstancesPerMachine = it->second;
			}				
		}
			
#ifdef DEBUG
		cout << "spawn(): Command= " << command << endl;
		cout << "hosts: " << hosts << " instances/machine: " << maxInstancesPerMachine << endl;
#endif
			
		spawnInfo = MPI::Info::Create();
		spawnInfo.Set("host", hosts.c_str());
		toSpawn = numMachines * maxInstancesPerMachine;
	}

	MPI::Intercomm childrenIntercomm;
	if (errors == NULL) {
		// we don't need errorCodes
		childrenIntercomm = allProcess.Spawn(command.c_str(), argv,
			toSpawn, spawnInfo, 0);	
	} else {
		int* errorCodes = (int*) malloc(sizeof(int)*nprocess);
		
		childrenIntercomm = allProcess.Spawn(command.c_str(), argv,
			toSpawn, spawnInfo, 0, errorCodes);
		
		// fill errors array
		for (int i=0; i<nprocess; i++) {
			if (errorCodes[i] == MPI::SUCCESS) {
				errors[i] = 0;
			} else {
				errors[i] = errorCodes[i];
			}
		}

		free(errorCodes);	
	}

	// free spawnInfo
	if (spawnInfo != MPI::INFO_NULL) {
		spawnInfo.Free();
	}

	// get how many process were sucessful spwned
	spawned = childrenIntercomm.Get_remote_size();
#ifdef DEBUG
	int localSize = childrenIntercomm.Get_size();
	cout << "local group size: " << localSize << ", remote: " << spawned << endl;
#endif
	
	// return NULL if we didin't spawned any process 
	if (0 == spawned) return COMM_ERR_SPAWNING_PORCESS;

	// ugly hack	
	sleep(1);
	// create a intracommunicator that include all children
	allProcess = childrenIntercomm.Merge(false);
	
	// update number of process 
	nProcess   = allProcess.Get_size();
	
#ifdef DEBUG
	cout << "joint group size: " << nProcess << endl;
#endif
	 
	return 0;
}
	
/*BcastGroup* YieldMPIComm::getParent() {
	MPI::Intercomm parentIntercomm = MPI::Comm::Get_parent();
	
	// return NULL if current process dosen't has a parent
	if (MPI::COMM_NULL == parentIntercomm) return NULL;
	
	int parentGroupSize       = parentIntercomm.Get_remote_size();
	YieldMPIBcastGroup* parentGroup = new YieldMPIBcastGroup(parentGroupSize, parentIntercomm, 1);
	return parentGroup;
}
	
BcastGroup* YieldMPIComm::getBrothers() {
	// create the group using MPI::COMM_WORLD
	YieldMPIBcastGroup* allChildren = new YieldMPIBcastGroup(MPI::COMM_WORLD.Get_size(), MPI::COMM_WORLD, 2);
	allChildren->setLocalGroup(MPI::COMM_WORLD);
	
	return allChildren;
}*/

int YieldMPIComm::send(int to, void* data, int size, int tag) {

	// check parameters
	if (data == NULL) {
		return COMM_ERR_BAD_PARAMETER;
	}
	if (size <= 0) {
		return COMM_ERR_BAD_PARAMETER;
	}
	
	// check destiny
	if ( (to<0) || (to>nProcess) ) {
		return COMM_ERR_NOT_IN_GROUP;
	}
	
	allProcess.Send(data, size, MPI::BYTE, to, tag);
	
	return size;
}

int YieldMPIComm::sendBuffer(int to, CommBuffer* buf, int tag) {
	
	// check parameters
	if (buf == NULL) {
		return COMM_ERR_BAD_PARAMETER;
	}
	
	// check destiny
	if ( (to<0) || (to>nProcess) ) {
		return COMM_ERR_NOT_IN_GROUP;
	}

	YieldMPICommBuffer* mpiBuf = (YieldMPICommBuffer*) buf;
	int   size = mpiBuf->getSize();
	void* data = mpiBuf->getBufferPointer();
	
	allProcess.Send(data, size, MPI::BYTE, to, tag);

	return size;	
}

int YieldMPIComm::recv(int from, void* data, int size, int tag) {

	// check parameters
	if (data == NULL) {
		return COMM_ERR_BAD_PARAMETER;
	}
	if (size <= 0) {
		return COMM_ERR_BAD_PARAMETER;
	}

	// check origin
	int mpiSource = -1;
	if (from == COMM_ANY_SOURCE) {
		mpiSource = MPI::ANY_SOURCE;
	} else {
		if ( (from < 0) || (from > nProcess) ) {
			return COMM_ERR_NOT_IN_GROUP;
		}
		
		mpiSource = from;
	}
	
	// check tag
	int mpiTag = -1;
	if (tag == COMM_ANY_TAG) {
		mpiTag = MPI::ANY_TAG;
	} else {
		if (tag < 0) {
			return COMM_ERR_BAD_PARAMETER;
		}
		
		mpiTag = tag;
	}
	
	MPI::Status opStatus;
	bool hasMessage = allProcess.Iprobe(mpiSource, mpiTag, opStatus);
	while (!hasMessage) {
		usleep(YIELD_TIME);
		hasMessage = allProcess.Iprobe(mpiSource, mpiTag, opStatus);	
	}
	
	allProcess.Recv(data, size, MPI::BYTE, mpiSource, mpiTag, opStatus);
	
	// check status
	int error = opStatus.Get_error();
	/// \todo check errors, we don't handle message truncation!!!
	
	return size;
}

int YieldMPIComm::recvBuffer(int from, CommBuffer* buf, int tag) {
	
	// check parameters
	if (buf == NULL) {
		return COMM_ERR_BAD_PARAMETER;
	}
	
	// check origin
	int mpiSource = -1;
	if (from == COMM_ANY_SOURCE) {
		mpiSource = MPI::ANY_SOURCE;
	} else {
		if ( (from < 0) || (from > nProcess) ) {
			return COMM_ERR_NOT_IN_GROUP;
		}
		
		mpiSource = from;
	}
	
	// check tag
	int mpiTag = -1;
	if (tag == COMM_ANY_TAG) {
		mpiTag = MPI::ANY_TAG;
	} else {
		if (tag < 0) {
			return COMM_ERR_BAD_PARAMETER;
		}
		
		mpiTag = tag;
	}
	
	// get message size
	MPI::Status opStatus;
	bool hasMessage = allProcess.Iprobe(mpiSource, mpiTag, opStatus);
	if (!hasMessage) {
		// give a second try
		hasMessage = allProcess.Iprobe(mpiSource, mpiTag, opStatus);
		while (!hasMessage) {
			usleep(YIELD_TIME);
			hasMessage = allProcess.Iprobe(mpiSource, mpiTag, opStatus);
		}	
	}
	int msgSize = opStatus.Get_count(MPI::BYTE);
	
	// grow buffer if necessary
	YieldMPICommBuffer* mpiBuf = (YieldMPICommBuffer*) buf;
	int    bufferCapacity = mpiBuf->getCapacity();
	if (msgSize > bufferCapacity) {
		mpiBuf->growTo(msgSize);
	}
	
	// recieve data
	void* bufferPtr = mpiBuf->getBufferPointer();
	allProcess.Recv(bufferPtr, msgSize, MPI::BYTE, mpiSource, mpiTag, opStatus);
	
	// check status
	int error = opStatus.Get_error();
	/// \todo check errors
	
	mpiBuf->setSize(msgSize);
	return msgSize;
}

int YieldMPIComm::probe(int& from, int& tag) {
	
	// check tag
	int mpiTag = -1;
	if (tag == COMM_ANY_TAG) {
		mpiTag = MPI::ANY_TAG;
	} else {
		if (tag < 0) {
			return COMM_ERR_BAD_PARAMETER;
		}
		
		mpiTag = tag;
	}
	
	MPI::Status status;
	bool hasMessage = allProcess.Iprobe(MPI::ANY_SOURCE, mpiTag, status);
	if (!hasMessage) {
		// give a second try
		hasMessage = allProcess.Iprobe(MPI::ANY_SOURCE, mpiTag, status);
		if (!hasMessage) return 0;
	}
	
	int msgSize = status.Get_elements(MPI::BYTE);
	if (from == COMM_ANY_SOURCE) {
		from = status.Get_source();
	}
	if (tag == COMM_ANY_TAG) {
		tag = status.Get_tag();
	}
	
	return msgSize;
}

int YieldMPIComm::poll(int& from, int& tag) {
	
	// check tag
	int mpiTag = -1;
	if (tag == COMM_ANY_TAG) {
		mpiTag = MPI::ANY_TAG;
	} else {
		if (tag < 0) {
			return COMM_ERR_BAD_PARAMETER;
		}
		
		mpiTag = tag;
	}
	
	MPI::Status status;
	bool hasMessage = allProcess.Iprobe(MPI::ANY_SOURCE, mpiTag, status);
	while (!hasMessage) {
		usleep(YIELD_TIME);
		hasMessage = allProcess.Iprobe(MPI::ANY_SOURCE, mpiTag, status);	
	}
	
	int msgSize = status.Get_elements(MPI::BYTE);
	if (from == COMM_ANY_SOURCE) {
		from = status.Get_source();
	}
	if (tag == COMM_ANY_TAG) {
		tag = status.Get_tag();
	}
	
	return msgSize;
}
