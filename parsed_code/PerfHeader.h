#ifndef PERF_H
#define PERF_H
#include <windows.h>
#define BEGIN(name) Push(&perfStack, ClockCount())

#define END(name) perfData[name].clockCount += (ClockCount() - Pop(&perfStack)), perfData[name].callCount++

typedef struct PerfStack
{
int size;
long long clocks[100];
} PerfStack;
PerfStack perfStack;
void Push(PerfStack* stack, long long clock)
{
stack->clocks[stack->size] = clock;
stack->size++;
}
long long Pop(PerfStack* stack)
{
stack->size--;
return stack->clocks[stack->size];
}
long long ClockCount()
{
LARGE_INTEGER clock;
QueryPerformanceCounter(&clock);
return clock.QuadPart;
}
typedef enum PerfCountName
{
	name,
	nameMainPerf,
	UpdatePerf,
	RenderPerf,
	InputResetPref,
	XInputPerf,
	WindowUpdatePerf,
	BufferFillPerf,
	DisplayPerf,
} PerfCountName;

char* perfNames[] = {
	"name",
	"nameMainPerf",
	"UpdatePerf",
	"RenderPerf",
	"InputResetPref",
	"XInputPerf",
	"WindowUpdatePerf",
	"BufferFillPerf",
	"DisplayPerf",
};

long long clockFrequency = 3410107;

typedef struct PerfData
{
long long clockCount;
int callCount;
} PerfData;
PerfData perfData[9];
typedef struct PerformanceMetric
{
int size;
char** names;
PerfData* data;
} PerformanceMetric;
void FlushPerfMetric()
{
for (int i = 0; i < 9; i++)
{
perfData[i].clockCount = 0;
perfData[i].callCount = 0;
}
}PerformanceMetric GetPerfMetric()
{
PerformanceMetric metric = {9, perfNames, perfData};
return metric;
}
#endif
