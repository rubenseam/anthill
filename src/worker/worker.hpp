/// @file
/// Worker process definition, with virtual functions to be
/// overridden by actual implementations. The user defined actions
/// will be executed based on events -- be it either a message
/// or a signal.
///
/// Worker processes must:
/// - define the virtual function `void Process()´;
/// - instantiate `anthill::packer` structures to send data;
/// - instantiate `anthill::unpacker` structures to receive data;
/// - call `anthill::terminate()` to finish the execution.
/// 
/// @author Bruno Rocha Coutinho <coutinho@dcc.ufmg.br> 
/// @author Rubens Emilio Alves Moreira <rubens@dcc.ufmg.br> 
/// 
/// @version x.y
# include <iostream>
# include "../process/process.hpp"


class Worker : public Process {

public:

	// Constructor
	Worker() {
	}

	// Destructor
	~Worker() {
	}

private:

	int id;

};
