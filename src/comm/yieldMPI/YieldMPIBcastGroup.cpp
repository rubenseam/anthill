#include "YieldMPIBcastGroup.h"
#include "CommLayer.h"




/*int YieldMPIBcastGroup::bcast(void* data, int size) {

	// check parameters
	if (data == NULL) {
		return COMM_ERR_BAD_PARAMETER;
	}
	if (size <= 0) {
		return COMM_ERR_BAD_PARAMETER;
	}
	
	// in MPI a bcast must be recieved with bcast too
	//interComm.Bcast(data, size, MPI::BYTE, interComm.Get_rank());
	for (int i=0; i<remoteGroupSize; i++) {
		interComm.Send(data, size, MPI::BYTE, i, groupTag);
	}
	
	return size;
}
*/

/// \todo: perhaps mpi_comm_create do the entire job
/*BcastGroup* YieldMPIBcastGroup::subGroup(int* participants, int subGroupSize, int newGroupTag) {

	// check parameters
	if (participants == NULL) {
		return NULL;
	}
	if (subGroupSize > remoteGroupSize) {
		return NULL;
	}
	if (subGroupSize <= 0) {
		return NULL;
	}
	if (newGroupTag <= 0) {
		return NULL;
	}
	if (newGroupTag == groupTag) {
		return NULL;
	}
	
	for (int i=0; i<subGroupSize; i++) {
		if ( (participants[i]<0) || (participants[i]>remoteGroupSize) ) {
			return NULL;
		}
	}
	
	// get the remote group
	MPI::Group remoteGroup = interComm.Get_remote_group();
	
	// build a communicator with only the ranks we want
	MPI::Group     newSubGroup = remoteGroup.Incl(subGroupSize, participants);
	MPI::Intracomm newSubComm  = interComm.Create(newSubGroup); 
	
	// create a new intercommunicator
	MPI::Intercomm newInterComm = localGroup.Create_intercomm(0, newSubComm, 0, newGroupTag);
	
	YieldMPIBcastGroup* newGroup = new YieldMPIBcastGroup(subGroupSize, newInterComm, newGroupTag);
	newGroup->setLocalGroup(localGroup);
	
	return newGroup;
}*/

int YieldMPIBcastGroup::setLocalGroup(int* participants, int groupSize) {
	// check parameters
	if (participants == NULL) {
		return COMM_ERR_BAD_PARAMETER;
	}
	if (groupSize > interComm.Get_size()) {
		return COMM_ERR_BAD_PARAMETER;
	}
	if (groupSize <= 0) {
		return COMM_ERR_BAD_PARAMETER;
	}
	
	for (int i=0; i<groupSize; i++) {
		if ((participants[i]<0) || (participants[i] > localGroupSize)) {
			return COMM_ERR_NOT_IN_GROUP;
		}
	}
	
	// get the local group
	MPI::Intracomm intracomm   = interComm;
	MPI::Group allLocalProcess = intracomm.Get_group();
	
	// build a communicator with only the ranks we want
	MPI::Group newLocalGroup = allLocalProcess.Incl(localGroupSize, participants);
	localGroup               = intracomm.Create(newLocalGroup); 
	
	return localGroupSize;
}

int YieldMPIBcastGroup::setLocalGroup(MPI::Intracomm& newLocalGroup) {
		localGroup     = newLocalGroup;
		localGroupSize = localGroup.Get_size();
		return localGroupSize;
}

