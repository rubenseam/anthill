/// @file
/// Set of operations designed to manage streams and filters,
/// based on Anthill parallel processing model.
/// The manager must:
/// - read up the configuration file -- config.xml;
/// - set both the communication layout and process placement;
/// - broadcast global configurations and parameters;
/// - start the execution of loaded filters;
/// - TODO: treat user signals -- termination, global status, ...;
/// - wait for the termination of entire process.
///
/// @author Bruno Rocha Coutinho <coutinho@dcc.ufmg.br>
/// @author Rubens Emilio Alves Moreira <rubens@dcc.ufmg.br>
///
/// @version x.y
# include <xmlparser.hpp>
# include "../process/process.hpp"


class Manager : public Process {

public:

	// Constructor
	Manager() {
	}

	// Destructor
	~Manager() {
	}

private:

	int num_workers;

};
