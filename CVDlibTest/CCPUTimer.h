#if !defined(__CCPUTIMER_H__)
#define __CCPUTIMER_H__

/////////////////////////////////////////////////////////////////////////////
//		CCPUTIMER
//			PerformanceTimerを用いた高精度タイマークラス
//
//							Coded by Toru Kurihara(bond@alab.t.u-tokyo.ac.jp)
//		
//		Ver.1.00 2012.12.20  骨格完成
//
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>

class CCPUTimer 
{
private:
	LARGE_INTEGER m_StartTime;
	LARGE_INTEGER m_CPUFreq;

public:
	CCPUTimer(){
		QueryPerformanceFrequency(&m_CPUFreq);		
	};
	~CCPUTimer(){};
	void Start(){									//Timer start
		QueryPerformanceCounter(&m_StartTime);
	};
	unsigned long GetElapsedTime(){				//Get ElapsedTime in [us]
		LARGE_INTEGER time;	
		QueryPerformanceCounter(&time);
		return (time.QuadPart-m_StartTime.QuadPart)*1000000/m_CPUFreq.QuadPart;
	};
	unsigned long GetElapsedTimeReset(){				//Get ElapsedTime in [us]
		LARGE_INTEGER time;
		LARGE_INTEGER stime;
		QueryPerformanceCounter(&time);
		stime = m_StartTime;
		m_StartTime=time;
		return (time.QuadPart-stime.QuadPart)*1000000/m_CPUFreq.QuadPart;
	};
};

#endif
