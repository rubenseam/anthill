#ifndef YIELDMPICOMMGROUP_H_
#define YIELDMPICOMMGROUP_H_

#include <mpi.h>
#include "BcastGroup.h"


class YieldMPIBcastGroup : public BcastGroup
{
public:
	YieldMPIBcastGroup(const int newRemoteGroupSize, MPI::Intercomm newInterComm, const int newTag)
	:	localGroupSize(-1),
		remoteGroupSize(newRemoteGroupSize),
		localGroup(MPI::COMM_NULL),
		interComm(newInterComm),
		groupTag(newTag)
	{}
	
	virtual ~YieldMPIBcastGroup() {
		localGroup.~Intracomm();
		interComm.~Intercomm();
	}
	
	//CommGroup* subGroup(int* participants, int subGroupSize, int newGroupTag);
	//int bcast(void* data, int size);
	
	int setLocalGroup(int* participants, int localGroupSize);
	int setLocalGroup(MPI::Intracomm& newLocalGroup);
	
private:
	int            localGroupSize;
	int            remoteGroupSize;
	MPI::Intracomm localGroup;
	MPI::Intercomm interComm;
	int            groupTag;
};

#endif /*YIELDMPICOMMGROUP_H_*/
