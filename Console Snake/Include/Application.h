#pragma once
#ifndef SNAKE_APPLICATION_HEADER_
#define SNAKE_APPLICATION_HEADER_

class Application
{
public:
	Application(int argc, char* argv[]);
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

public:
	int run();

};

#endif // SNAKE_APPLICATION_HEADER_