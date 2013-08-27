/// @file
/// Message structure used by Anthill processes to exchange data.
/// The encoding/decoding of C++ data structures is performed using
/// the MsgPack library, available at http://msgpack.org/
/// 
/// @author Bruno Rocha Coutinho <coutinho@dcc.ufmg.br> 
/// @author Rubens Emilio Alves Moreira <rubens@dcc.ufmg.br> 
/// 
/// @version x.y
# include <vector>
# include <msgpack.hpp>


class Message {

public:

	// Constructor
	Message() {
	}

	// Destructor
	~Message() {
	}

private:

	std::vector<char> buffer;

};
