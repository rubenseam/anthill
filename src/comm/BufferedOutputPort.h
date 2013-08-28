/*
 * BufferedOutputPort.h
 *
 *  Created on: Nov 18, 2011
 *      Author: coutinho
 */

#ifndef BUFFEREDOUTPUTPORT_H_
#define BUFFEREDOUTPUTPORT_H_


#include <vector>
#include "FilterDev.h"
#include "DataBuffer.h"


class BufferedOutputPort {
public:
	typedef enum {roundRobin, random, broadCast, labeledStream} sendPolicy_t;
	typedef int hashFunc_t(char *label, int image);
	typedef void labelFunc_t(void *msg, int size, char *label);

private:
	OutputPortHandler port;
	sendPolicy_t policy;

	unsigned int recieverInstances;
	std::vector<DataBuffer> portBuffers;

	labelFunc_t* labelStreamGetLabel;
	hashFunc_t* labelStreamHashFunc;

	unsigned long bytesSent;
	unsigned long nMessages;

public:
	static const int sendTreshold = 8900;

	static const int errorPortClosed = -2;

	BufferedOutputPort(OutputPortHandler oph, sendPolicy_t pol);
	virtual ~BufferedOutputPort() {
		if (port >= 0) {
			// port is still open
			close();
		}
	};

	void registerLabelStreamFuncs(labelFunc_t* getLabelFunc, hashFunc_t* hashFunc) {
		labelStreamGetLabel = getLabelFunc;
		labelStreamHashFunc = hashFunc;
	}

	void getStats(unsigned long* messages, unsigned long* bytes) {
		*messages = nMessages;
		*bytes    = bytesSent;
	}

	int labeledStreamGetDestination(DataBuffer* buf);

	int send(DataBuffer* buf);
	int flush();
	int close();

private:
	BufferedOutputPort()
	: policy(roundRobin)
	, recieverInstances(0)
	, labelStreamGetLabel(NULL)
	, labelStreamHashFunc(NULL)
	, bytesSent(0)
	, nMessages(0)
	{};

	int sendBufferData(DataBuffer* buf);
};

#endif /* BUFFEREDOUTPUTPORT_H_ */
