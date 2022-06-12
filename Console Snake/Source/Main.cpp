#include "Modules.h"
#include "Application.h"

int main(int argc, char* argv[])
{
	ModuleManager manager;
	Application app(argc, argv);
	return app.run();
}