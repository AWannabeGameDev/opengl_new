#ifndef PROFILER_H
#define PROFILER_H

class Profiler
{
private :
	float startTime;
	const char* message;
	float& accumulator;

public :
	Profiler(const char* message, float& accumulator);
	~Profiler();
};

#endif