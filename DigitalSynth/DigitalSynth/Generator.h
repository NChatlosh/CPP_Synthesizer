#pragma once
#include <SFML\Audio.hpp>
#include <SFML\System.hpp>
#include <SFML\Audio\Sound.hpp>
#include <math.h>
#include <SFML\Window.hpp>
#include <vector>
#include <functional>
#include <string>
#include <algorithm>
#include <thread>

#define TWOPI 6.283185307 

using namespace sf;

const enum Wave {Sine, Square, FmSine, FmSquare};
const float OctaveMult[] = {0.5, 1.0, 2.0, 4.0};

class Generator 
{
	public:
		short SineWave(double time, double freq, double amp);
		short SquareWave(double time, double freq, double amp);
		void PlayNote(double freq);
		Generator(int id);
		void ChangeWave();
		void UpOctave();
		void DownOctave();
		int GetVolume();
		int GetOctave();
		std::string GetWave();	
		float GetAmp();
		void SetAmp(float newAmp);
		float GetModAmp();
		void SetModAmp(float newAmp);
		bool GetIsOn();
		void ToggleIsOn();
		int GetID();
		short GetSamples(int i);
		void DumpSamples();
	private:
		std::vector<Int16>* samples;
		bool IsOn;
		float ModAmp;
		float Amp;
		int volume;
		Wave wave;
		int octave;
		int ID;
};

Generator::Generator(int id = 0)
{
	//set default values
	ID = id;
	IsOn = true;
	ModAmp = 0.5;
	Amp = 0.9;
	wave = Sine;
	volume = 100;
	octave = 1;
}

void Generator::DumpSamples()
{
	samples->clear();
	delete samples;
}

short Generator::GetSamples(int i)
{
	if (IsOn)
	{
		return (*samples)[i];
	}
	else
	{
		return 0;
	}
}

int Generator::GetVolume()
{
	return volume;
}

int Generator::GetOctave()
{
	return octave;
}

std::string Generator::GetWave()
{
	std::string w = "";
	switch (wave)
	{
	case Sine:
		w = "Sine";
		break;
	case Square:
		w = "Square";
		break;
	case FmSine:
		w = "FM-Sine";
		break;
	case FmSquare:
		w = "FM-Square";
		break;
	default:
		break;
	}
	return w;
}


int Generator::GetID()
{
	return ID;
}

bool Generator::GetIsOn()
{
	return IsOn;
}

void Generator::ToggleIsOn()
{
	IsOn = !IsOn;
}

float Generator::GetModAmp()
{
	return ModAmp;
}

void Generator::SetModAmp(float newAmp)
{
	ModAmp = newAmp;
}

float Generator::GetAmp()
{
	return Amp;
}
void Generator::SetAmp(float newAmp)
{
	Amp = newAmp;
}

void Generator::UpOctave()
{
	if (octave != 3)
	{
		octave++;
	}	
}

void Generator::DownOctave()
{
	if (octave != 0 )
	{
		octave--;
	}
}

//Got this code to generate sine wave from Phillip Bohun on youtube
//this is the link to the video: https://www.youtube.com/watch?v=5xd9BMxoXqo
short Generator::SineWave(double time, double freq, double amp)
{
	short result; //16-bit int
	double tpc = 44100 / freq; // ticks per cycle, 44100hz is the sampling rate(cyles per second)
	double cycles = time / tpc; // number of cycles of wave period
	double radians = TWOPI * cycles; // radians to be put into sine function since math::sine computes with radian values, not degrees
	short amplitude = 32767 * amp; // set real amplitude, 32767 is the highest value output form sine function, so amp is relative: .5 passed into function produces half the amplitude
	result = amplitude * sin(radians);//multiply real amplitude times the result of sine of the radians value and cast that value into a 16-bit int to get one sample of sine wave
	return result; //return the single sample
}

void Generator::ChangeWave()
{
	switch (wave)
	{
	case Sine:
		wave = Square;
		break;
	case Square:
		wave = FmSine;
		break;
	case FmSine:
		wave = FmSquare;
		break;
	case FmSquare:
		wave = Sine;
		break;
	default:
		wave = Sine;
		break;
	}
}

//Got this code to generate square wave from Phillip Bohun on youtube from another video in his series
//this is the link to the video: https://www.youtube.com/watch?v=Y4qiT3E3RE4
short Generator::SquareWave(double time, double freq, double amp)
{
	short result = 0; //16-bit int
	int tpc = 44100 / freq; // ticks per cycle, 44100hz is the sampling rate(cyles per second)
	int cyclePart = int(time) % tpc;//calculate cycles
	int halfCycle = tpc / 2;//calculate half-cycle, sine square wave is full amp half the time, 0 the rest of the time
	short amplitude = 32767 * amp; // set real amplitude, 32767 is the highest value output form sine function, so amp is relative: .5 passed into function produces half the amplitude
	if (cyclePart < halfCycle)//if the current cyclePart is less than halfCycle, meaning the part of the wave at full amplitude, or on
	{
		result = amplitude; // then set result to value of amplitude
	}
	//if it isn't, skip the previous statement and keep the result at 0
	return result;// result will either be 0 or value of amplitude, since square wave is either at full amp or 0
}

void Generator::PlayNote(double freq)
{
	
	samples = new std::vector<Int16>();
	samples->reserve(44100);
	freq *= OctaveMult[octave];//change octave with multiplier
//	Filter filt(LPF, 10, 44100.0, 1000.0);
	switch (wave)
	{
	case Sine:
		for (int i = 0; i < 44100; i++)
		{
			samples->push_back(SineWave(i, freq, Amp));
		}
		break;
	case Square:
		for (int i = 0; i < 44100; i++)
		{
			samples->push_back(SquareWave(i, freq, Amp));
		}
		break;
	case FmSine:
		for (int i = 0; i < 44100; i++)
		{
			samples->push_back(SineWave(i, freq + (SquareWave(i, freq, ModAmp)), Amp));
		}
		break;
	case FmSquare:
		for (int i = 0; i < 44100; i++)
		{
			samples->push_back(SquareWave(i, freq + (SquareWave(i, freq / 4, ModAmp)), Amp));
		}
		break;
	default:
		break;
	}
	
	

}



