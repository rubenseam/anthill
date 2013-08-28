/*
 * BufferedInputPort.h
 *
 *  Created on: Nov 18, 2011
 *      Author: coutinho
 */

#ifndef BUFFEREDINPUTPORT_H_
#define BUFFEREDINPUTPORT_H_


#include <unistd.h>
#include "DataBuffer.h"


class BufferedInputPort {
public:
	typedef void IdleWorker();

private:
	const static useconds_t defaultSleepTime = 10000;

	InputPortHandler port;
	DataBuffer portBuffer;

	IdleWorker* idleWorkerCallback;
	useconds_t waitBeforeCallingIdleWorker;

public:
	BufferedInputPort(InputPortHandler iph)
	: port(iph)
	{}

	virtual ~BufferedInputPort() {};

	void registerIdleWorker(IdleWorker* worker, useconds_t wait = defaultSleepTime) {
		idleWorkerCallback = worker;
		waitBeforeCallingIdleWorker = wait;
	}

	int recv(DataBuffer* buf);

private:
	BufferedInputPort()
	{};

};

#endif /* BUFFEREDINPUTPORT_H_ */
