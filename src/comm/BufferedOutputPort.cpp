/*
 * BufferedOutputPort.cpp
 *
 *  Created on: Nov 18, 2011
 *      Author: coutinho
 */

#include "BufferedOutputPort.h"


const int BufferedOutputPort::sendTreshold;


BufferedOutputPort::BufferedOutputPort(OutputPortHandler oph, sendPolicy_t pol)
: port(oph)
, policy(pol)
, recieverInstances(dsGetNumReaders(port))
, portBuffers( (pol==labeledStream)? recieverInstances : 1 ) // labeled stream has one buffer per instance, other have one buffer to all
, labelStreamGetLabel(NULL)
, labelStreamHashFunc(NULL)
, bytesSent(0)
, nMessages(0)
{
}

int BufferedOutputPort::labeledStreamGetDestination(DataBuffer* buf) {
	int destination = 0;

	if (NULL == labelStreamGetLabel) {
		// cannot send data through label stream without registering functions first
		return -1;
	}

	int* intBuff = (int*) buf->getBuffer();
	int* labelFuncBuf = intBuff-1; // message aggregation will put a int before the first message and label stream take it into account

	// get label
	static char label[MAX_LBL_LENGTH];
	labelStreamGetLabel(labelFuncBuf, buf->getSize(), label);

	// get hashcode from label
	destination = labelStreamHashFunc(label, recieverInstances);
	destination = destination % recieverInstances;

	return destination;
}

int BufferedOutputPort::send(DataBuffer* buf) {
	int toBuffer = 0;

	if (port < 0) return errorPortClosed;

	// choose buffer
	switch (policy) {
	case labeledStream:
		toBuffer = labeledStreamGetDestination(buf);
		break;
	default:
		toBuffer = 0;
		break;
	}

	// put data in port buffer
	portBuffers[toBuffer].pack(buf);

	if (portBuffers[toBuffer].getSize() > sendTreshold) {
		// buffer with enough data to be sent
		sendBufferData( &(portBuffers[toBuffer]) );
	}

	return 0;
}

int BufferedOutputPort::flush() {
	if (port < 0) return errorPortClosed;

	for (unsigned int i=0; i<portBuffers.size(); ++i) {
		sendBufferData( &(portBuffers[i]) );
	}

	return 0;
}

int BufferedOutputPort::close() {
	if (port < 0) return errorPortClosed;

	// flush buffers
	int status = flush();

	// close port
	status = dsCloseOutputPort(port);

	// free resources
	portBuffers.clear();

	port = -1;
	return status;
}

int BufferedOutputPort::sendBufferData(DataBuffer* buf) {
	if (port < 0) return errorPortClosed;
	int status = 0;

	if (buf->getSize() > 0) {
		nMessages++;
		bytesSent += buf->getSize();

		status = buf->send(port);
		buf->clear();
	}

	return status;
}
