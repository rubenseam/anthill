/*
 * BufferedInputPort.cpp
 *
 *  Created on: Nov 18, 2011
 *      Author: coutinho
 */


#include "BufferedInputPort.h"


int BufferedInputPort::recv(DataBuffer* buf) {

	// Unpack message to user buffer
	int unpackStatus = portBuffer.unpack(buf);
	if (END_OF_BUFFER == unpackStatus) {
		int recvStatus = 0;

		// Receive new aggregated message
		if (idleWorkerCallback != NULL) {
			useconds_t timeWaiting = 0;

			recvStatus = portBuffer.tryRecv(port);

			while (recvStatus == 0) {
				usleep(defaultSleepTime)
						;
				timeWaiting += defaultSleepTime;
				if (timeWaiting >= waitBeforeCallingIdleWorker) {
					idleWorkerCallback();
				}

				recvStatus = portBuffer.tryRecv(port);
			}
		} else {
			recvStatus = portBuffer.recv(port);
		}
		if (EOW == recvStatus) {
			return EOW;
		}


		// Unpack message to user buffer
		return portBuffer.unpack(buf);
	}

	return unpackStatus;
}
