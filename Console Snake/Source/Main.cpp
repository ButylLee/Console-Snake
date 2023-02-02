#include "Modules.h"
#include "Application.h"
#include <cstdlib>

int main(int argc, char* argv[]) try
{
	ModuleManager manager;
	Application app(argc, argv);
	return app.run();
}
catch (...)
{
	// has automatically cleaned up
	exit(EXIT_FAILURE);
}