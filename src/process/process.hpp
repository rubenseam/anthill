/// @file
/// Process definition, supporting communication schemas.
/// Classes inheriting this interface may act either
/// as managers or workers in the Anthill parallel processing model.
/// 
/// @author Bruno Rocha Coutinho <coutinho@dcc.ufmg.br> 
/// @author Rubens Emilio Alves Moreira <rubens@dcc.ufmg.br> 
/// 
/// @version x.y
# include <unordered_map>
# include "message.hpp"
# include "../comm/comm.hpp"


class Process {

protected:

	// Constructor
	Process() {
	}

	// Destructor
	~Process() {
	}

private:

	std::unordered_map<int, Port>;

};
