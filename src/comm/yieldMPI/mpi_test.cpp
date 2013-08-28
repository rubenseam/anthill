#include <iostream>
#include "YieldMPIComm.h"
#include <assert.h>


using namespace std;

int main (int argc, char* argv[]) {
	YieldMPIComm* network = new YieldMPIComm(argc, argv);
	int nProcess     = network->getNumProcess();

	if (1 == nProcess) {
		// I am the parent
		char processorName[100];
		int lixo;
		MPI::Get_processor_name(processorName, lixo);
				
		int nproces = network->getNumProcess();
		int rank    = network->getMyRank();
		
		cout << "parent " << rank << " of " << nproces << " running on " << processorName << endl;

			
		// spawn children
		const char* argv[2] = {"param", NULL};
		int errorCodes[4]   = {-1, -1, -1, -1};
		int spawned         = -1;
		 
		int status  = network->spwan("mpi_test", argv, 4, spawned, errorCodes);
		int newRank = network->getMyRank();
		
		cout << "parent: spawned " << spawned << " process, newRank: " << newRank << ", status = " << status << endl;
		
		// send something to them
		int message[2] = {1, 2};
		for (int i=1; i<=4; i++) {
			status = network->send(i, message, sizeof(message), 1);
			assert(status == (2*sizeof(int)));
		}
	} else {
		char processorName[100];
		int lixo;
		MPI::Get_processor_name(processorName, lixo);
		
		int nproces = network->getNumProcess();
		int rank    = network->getMyRank();
		
		cout << "child " << rank << " of " << nproces << " running on " << processorName << endl;
		
		// recieve something from parent
		int source  = -1;
		int size    = network->poll(source, 1);
		int source2 = -1; 
		int size2   = network->probe(source2, COMM_ANY_TAG);
		assert(source == source2);
		assert(size == size2);
		assert(source==0);
		assert(size== (2*sizeof(int)) );
		
		int message[2] = {0,0};
		int status = network->recv(COMM_ANY_SOURCE, message, size, 1);
		assert(status == (2*sizeof(int)));
		
		// 1 and 2 send messages to 3 and 4
		if (rank <= 2) {
			int msg[] = {rank, 3};
			int status = network->send(3, msg, 2*sizeof(int), 2);
			assert(2*sizeof(int) == status);
			
			msg[1] = 4;
			status = network->send(4, msg, 2*sizeof(int), 2);
			assert(2*sizeof(int) == status);			
		} else {
			int msg[] = {-1, -1};
			int status = network->recv(1, msg, 2*sizeof(int), COMM_ANY_TAG);
			assert(2*sizeof(int) == status);
			assert(1 == msg[0]);
			assert(rank == msg[1]);
			
			status = network->recv(2, msg, 2*sizeof(int), 2);
			assert(2*sizeof(int) == status);
			assert(2 == msg[0]);
			assert(rank == msg[1]);
		}
		
		// 3 and 4 send messages to 1 and 2
	}
	
	delete network;
	return 0;	
}

