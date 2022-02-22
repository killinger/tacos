#pragma once
#ifdef  _PROFILE
#include "defs.h"
#include <intrin.h>
#include "subsystems.h"
#include <map>
#include <unordered_map>
#include <vector>
#include <algorithm>
#pragma intrinsic(__rdtsc)

#define PROFILE() auto_profiler PROFILER##__COUNTER__(__FUNCTION__)
#define PROFILE_ROOT() root_profiler ROOT_PROFILER##__COUNTER__(__FUNCTION__)
#define PROFILE_GROUP(Group) group_profiler GROUP_PROFILER##__COUNTER__(Group)

#define PROFILE_GROUP_STACK_SIZE 20
#define PROFILE_HISTORY_SIZE 30
#ifndef _DEBUG
#define PROFILE_FRAMES_TO_UPDATE 60
#else
#define PROFILE_FRAMES_TO_UPDATE 20
#endif // !_DEBUG


struct profile_node;
typedef std::unordered_map<const char*, profile_node> node_map;
typedef std::vector<profile_node*> node_list;

struct profile_node
{
	profile_node() :
		m_AccumulatedTime(0),
		m_CallCount(0),
		m_PeakAverage(0.0),
		m_Name(""),
		m_HistoryCursor(0),
		m_AddedToList(false) {	memset(m_History, 0.0, sizeof(m_History)); }
	uint64			m_AccumulatedTime;
	uint64			m_CallCount;
	double			m_AverageTime;
	double			m_PeakAverage;
	double			m_AverageCalls;
	double			m_History[PROFILE_HISTORY_SIZE];
	uint32			m_HistoryCursor;
	const char*		m_Name;
	bool			m_AddedToList;
};

class profile_system
{
public:
	profile_system()
	{
		m_FrameTime = 0.0;
		m_FrameCount = 1;
	}
	profile_node	m_Root;
	node_map		m_Nodes;
	node_list		m_NodeList;
	uint32			m_FrameCount;
	double			m_FrameTime;
	double			m_Frequency;

	void Update()
	{
		if (m_FrameCount % PROFILE_FRAMES_TO_UPDATE == 0)
		{
			m_Root.m_AverageTime = (m_Root.m_AccumulatedTime / (double)PROFILE_FRAMES_TO_UPDATE) / m_Frequency;
			m_Root.m_AverageCalls = m_Root.m_AverageCalls / (double)PROFILE_FRAMES_TO_UPDATE;
			m_Root.m_AccumulatedTime = 0;
			m_Root.m_CallCount = 0;
			for (auto& Node : m_Nodes)
			{
				Node.second.m_AverageTime = ((double)Node.second.m_AccumulatedTime / (double)PROFILE_FRAMES_TO_UPDATE) / m_Frequency;
				Node.second.m_AverageCalls = (double)Node.second.m_CallCount / (double)PROFILE_FRAMES_TO_UPDATE;
				Node.second.m_AccumulatedTime = 0;
				Node.second.m_CallCount = 0;
				Node.second.m_History[Node.second.m_HistoryCursor] = Node.second.m_AverageTime / m_Root.m_AverageTime;
				++Node.second.m_HistoryCursor %= PROFILE_HISTORY_SIZE;
				if (!Node.second.m_AddedToList)
				{
					m_NodeList.push_back(&Node.second);
					Node.second.m_AddedToList = true;
				}
			}
			SortListByName();
		}
		m_FrameCount++;
	}

	void SortListByName()
	{
		std::sort(m_NodeList.begin(), m_NodeList.end(), NameComparison);
	}

	static bool NameComparison(profile_node* A, profile_node* B)
	{
		return std::strcmp(A->m_Name, B->m_Name) < 0;
	}
	
	void StoreSample(const char* Name, uint64 ElapsedTime)
	{
		profile_node* Node = &m_Nodes[Name];
		Node->m_AccumulatedTime += ElapsedTime;
		Node->m_Name = Name;
		Node->m_CallCount++;
	}

	void StoreRootSample(const char* Name, uint64 ElapsedTime)
	{
		m_Root.m_AccumulatedTime += ElapsedTime;
		m_Root.m_Name = Name;
		m_Root.m_CallCount++;
	}

private:
};

//struct auto_profiler
//{
//	auto_profiler(const char* Function)
//	{
//		m_StartCounter = __rdtsc();
//		m_Function = Function;
//	}
//	~auto_profiler()
//	{
//		m_EndCounter = __rdtsc();
//		uint64 ElapsedTime = m_EndCounter - m_StartCounter;
//		ProfileSystem->StoreSample(m_Function, ElapsedTime);
//	}
//	uint64			m_StartCounter;
//	uint64			m_EndCounter;
//	const char*		m_Function;
//};

struct auto_profiler
{
	auto_profiler(const char* Function)
	{
		QueryPerformanceCounter(&m_StartCounter);
		m_Function = Function;
	}
	~auto_profiler()
	{
		QueryPerformanceCounter(&m_EndCounter);
		uint64 ElapsedTime = uint64(m_EndCounter.QuadPart - m_StartCounter.QuadPart);
		ProfileSystem->StoreSample(m_Function, ElapsedTime);
	}
	LARGE_INTEGER			m_StartCounter;
	LARGE_INTEGER			m_EndCounter;
	const char*				m_Function;
};

struct root_profiler
{
	root_profiler(const char* Function)
	{
		QueryPerformanceCounter(&m_StartCounter);
		m_Function = Function;
	}
	~root_profiler()
	{
		QueryPerformanceCounter(&m_EndCounter);
		uint64 ElapsedTime = uint64(m_EndCounter.QuadPart - m_StartCounter.QuadPart);
		ProfileSystem->StoreRootSample(m_Function, ElapsedTime);
		ProfileSystem->Update();
	}
	LARGE_INTEGER			m_StartCounter;
	LARGE_INTEGER			m_EndCounter;
	const char*				m_Function;
};

#else
#define PROFILE(Index)
#endif //  _PROFILE
