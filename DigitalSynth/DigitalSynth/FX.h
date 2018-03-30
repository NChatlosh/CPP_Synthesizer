#pragma once
#include <SFML\Audio.hpp>
#include <SFML\System.hpp>
#include <SFML\Audio\Sound.hpp>
#include <SFML\Window.hpp>
#include <vector>
#include <functional>
#include <thread>
#include "filt.h"
#include "Generator.h"
#include <string>

using namespace sf;

class FX
{
public:
	FX();
	~FX();
	 void AmpEnvelope(std::vector<Int16>* signal);
	 void FilterSignal(std::vector<Int16>* signal);
	 double LFO(double freq, int index);
	 void LoadLFO();
	 void RouteSignal(std::vector<Int16>* signal);
	 float GetAttack();
	 float GetRelease();
	 void SetAttack(bool up);
	 void SetRelease(bool up);
	 void ToggleFilter();
	 void ToggleLFO();
	 void ChangeFilterType();
	 void ChangeCutoff(bool up);
	 bool GetLFOon();
	 bool GetFilterOn();
	 double GetCutoff();
	 double GetLFOFreq();
	 void ChangeLFOFreq(bool up);
	 std::string GetFiltType();
private:
	float Attack;
	float Release;
	Generator lfo;
	std::vector<Int16> LFOSamples;
	filterType FT;
	double cutoff;
	bool LFOIsOn;
	bool FilterOn;
	double LFOFreq;
};

FX::FX()
{
	std::thread t(&FX::LoadLFO, this);
	LFOFreq = 4.0;
	LFOIsOn = false;
	FilterOn = false;
	cutoff = 800.0;
	Attack = 0.1;
	Release = 1.0;
	FT = LPF;
	t.join();
}

FX::~FX()
{
}

inline void FX::AmpEnvelope(std::vector<Int16>* signal)
{
	float a = Attack;
	float r = Release;
	double e = 0.9999 / 22050;
	for (int i = 0; i < 22050; i++)
	{
		(*signal)[i] *= a;
		if ((a + e) >= 1)
		{
			a = 1;
		}
		else
		{
			a += e;
		}
	}
	for (int i = 22050; i < 44100; i++)
	{
		if (r <= 0)
		{
			r = 0;
		}
		(*signal)[i] *= r;
		if ((r - e) <= 0)
		{
			r = 0;
		}
		else
		{
			r -= e;
		}
	}
}

inline void FX::FilterSignal(std::vector<Int16>* signal)
{
	signal->reserve(88200);
	for (int i = 0; i < 44100; i++)
	{
		if (LFOIsOn)
		{
			Filter filt(FT, 4, 44100.0, LFO(cutoff, i));
			signal->push_back(filt.do_sample((*signal)[i]) * 10);
		}
		else
		{
			Filter filt(FT, 4, 44100.0, cutoff);
			signal->push_back(filt.do_sample((*signal)[i]) * 10);
		}
	}
		signal->erase(signal->begin(), signal->begin()+44100);

}

inline double FX::LFO(double freq, int index)
{
	double value = freq + LFOSamples[index];
	return value;
}

inline void FX::LoadLFO()
{
	LFOSamples.clear();
	LFOSamples.reserve(44100);
	for (int i = 0; i < 44100; i++)
	{
		LFOSamples.push_back(lfo.SineWave(i, LFOFreq, 0.099));
	}
}

inline void FX::RouteSignal(std::vector<Int16>* signal)
{
	if (FilterOn)
	{
		FilterSignal(signal);
	}
	AmpEnvelope(signal);
}

inline float FX::GetAttack()
{
	return Attack;
}

inline float FX::GetRelease()
{
	return Release;
}

inline void FX::SetAttack(bool up)
{
	if (up)
	{
		if ((Attack + 0.1) >= 1.0)
		{
			Attack = 1.0;
		}
		else
		{
			Attack += 0.1;
		}
	}
	else
	{
		if ((Attack - 0.1) <= 0.1)
		{
			Attack = 0.1;
		}
		else
		{
			Attack -= 0.1;
		}
	}
}

inline void FX::SetRelease(bool up)
{
	if (up)
	{
		if ((Release + 0.1) >= 1.0)
		{
			Release = 1.0;
		}
		else
		{
			Release += 0.1;
		}
	}
	else
	{
		if ((Release - 0.1) <= 0.1)
		{
			Release = 0.1;
		}
		else
		{
			Release -= 0.1;
		}
	}
}

inline void FX::ToggleFilter()
{
	FilterOn = !FilterOn;
}

inline void FX::ToggleLFO()
{
	LFOIsOn = !LFOIsOn;
}

inline void FX::ChangeFilterType()
{
	if (FT == LPF)
	{
		FT = HPF;
	}
	else
	{
		FT = LPF;
	}
}

inline void FX::ChangeCutoff(bool up)
{
	if (up)
	{
		cutoff += 10;
	}
	else
	{
		cutoff -= 10;
		if (cutoff <= 0)
			cutoff = 0;
	}
}

inline bool FX::GetLFOon()
{
	return LFOIsOn;
}

inline bool FX::GetFilterOn()
{
	return FilterOn;
}

inline double FX::GetCutoff()
{
	return cutoff;
}

inline double FX::GetLFOFreq()
{
	return LFOFreq;
}

inline void FX::ChangeLFOFreq(bool up)
{
	if (up)
	{
		LFOFreq += 0.5;
		if (LFOFreq >= 10)
		{
			LFOFreq = 10;
		}
	}
	else
	{
		LFOFreq -= 0.5;
		if (LFOFreq <= 0.5)
		{
			LFOFreq = 0.5;
		}
	}
	std::thread t(&FX::LoadLFO, this);
	t.join();
}

inline std::string FX::GetFiltType()
{
	if (FT == LPF)
	{
		return "LPF";
	}
	else
	{
		return "HPF";
	}
}



