#ifndef COMMLAYER_H_
#define COMMLAYER_H_

#include <string>
#include "CommBuffer.h"
#include "BcastGroup.h"


#define COMM_ANY_SOURCE -1
#define COMM_ANY_TAG    -1


#define COMM_ERR_BAD_PARAMETER      (-2)  ///< coincides with PvmBadArameter
#define COMM_ERR_NOT_IN_GROUP       (-20) ///< coincides with PvmNotInGroup
#define COMM_INTERNAL_ERR           (-34) ///< Comunication api internal error
#define COMM_ERR_TAG_ALREADY_IN_USE (-35) ///< Tag already used
#define COMM_ERR_SPAWNING_PORCESS   (-36) ///< A error ocurred spwaning process


using namespace std;


class CommLayer
{
public:
	// GCC complains if dtor isn't defined and dosen't accept a abstract one, 
	// so we define a empty dtor
	virtual ~CommLayer() {};
	
	virtual int spwan(const string command, const char** argv, const int nprocess, const char** where, 
		int& spawned, int* errorCodes=NULL) = 0;
	
	/// Create a buffer with a given initial capacity.
	virtual CommBuffer* createBuffer(int capacity) = 0;
	
	///	Send size bytes.
	///		\param to   Id of process that data will be sent.
	///		\param data Pointer to the data that will be sent. 
	///     \patam size Amount of bytes that will be sent.
	///		\param tag  Each tag is a channel. Tags must be bigger than 0.
	///		\return a negative value (errorcode) on error or bytes sent in success
	virtual int send(int to, void* data, int size, int tag) = 0;
	
	///	Send a buffer.
	///		\param to   Id of process that data will be sent.
	///		\param buf  Buffer with data that will be sent. 
	///		\param tag  Each tag is a channel. Tags must be bigger than 0.
	///		\return a negative value (errorcode) on error or bytes sent in success
	virtual int sendBuffer(int to, CommBuffer* buf, int tag) = 0;
	
	/// Recieve data.
	///		\param from Could be the id of specific process or COMM_ANY_SOURCE to recieve from any process.
	///		\param data Buffer where recieved data will be stored.
	///		\param size Capacity of the buffer. If buffer is smaller than the message, mesage will be truncated.
	///		\param tag  Could be a specific tag (tags must be bigger than 0) or COMM_ANY_TAG.
	///		\return a negative value (errorcode) on error or bytes stored in the buffer if successful.
	virtual int recv(int from, void* data, int size, int tag) = 0;
	
	/// Recieve a message and puts it in a buffer.
	///		\param from Could be the id of specific process or COMM_ANY_SOURCE to recieve from any process.
	///		\param buf  Buffer where recieved data will be stored.
	///		\param tag  Could be a specific tag (tags must be bigger than 0) or COMM_ANY_TAG.
	///		\return a negative value (errorcode) on error or bytes stored in the buffer if successful.
	virtual int recvBuffer(int from, CommBuffer* buf, int tag) = 0;
	
	/// Tells if there is a message ready to be recieved.
	/// 	\param from recieves the process id of message origin 
	///		\param tag  You could specify a tag to be probed or pass COMM_ANY_TAG to probe any tag. 
	/// 	\return a negative value (errorcode), 0 if there isn't any message or 1the size of message if there's one
	virtual int probe(int& from, int& tag) = 0;
	
	/// Waits until there is a message ready to be recieved.
	/// 	\param from recieves the process id of message origin 
	///		\param tag  You could specify a tag to be probed or pass COMM_ANY_TAG to probe any tag. 
	/// 	\return a negative value (errorcode), 0 if there isn't any message or 1the size of message if there's one
	virtual int poll(int& from, int& tag) = 0;

	/// Get the number of process in application.
	virtual int getNumProcess() = 0;
	
	/// Get my processor rank
	virtual int getMyRank() = 0;
	
	/// Get my host
	virtual string getHost() = 0;
};

#endif /*COMMLAYER_H_*/
