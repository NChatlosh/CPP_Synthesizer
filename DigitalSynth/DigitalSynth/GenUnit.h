#pragma once
#include <SFML\Audio.hpp>
#include <SFML\System.hpp>
#include <SFML\Audio\Sound.hpp>
#include <SFML\Window.hpp>
#include <vector>
#include "Generator.h"
#include <thread>
#include <iostream>
#include "FX.h"

using namespace std;
//class represents a unit that holds and manages a collection of oscillators, also routes the signal to an instance of FX for further processing
class GenUnit
{
public:
	GenUnit();
	~GenUnit();
	void Play(double freq);
	void ChangeCurrentGen(int id);
	void ChangeVolume(bool up);
	void ChangeCurrentAmp(bool up);
	void ChangeCurrentOctave(bool up);
	void ChangeCurrentWave();
	void ChangeModAmp(bool up);
	void ToggleOn();
	void DisplayGens();
	void HighlightSelected();
	int GetVolume();
	void DoNothing();
	void Dump(vector<sf::Int16>* samples);
	FX* GetFX();
private:
	Generator generators[3];
	int currentIndex;
	sf::Sound* sound;
	sf::SoundBuffer* buffer;
	int volume;
	FX* fx;
	bool IsSelected;
};

GenUnit::GenUnit()
{
	fx = new FX();
	volume = 100;
	//create 3 oscillators
	currentIndex = 0;
	Generator gen1(1);
	Generator gen2(2);
	Generator gen3(3);
	gen2.ToggleIsOn();
	gen3.ToggleIsOn();
	generators[0] = gen1;
	generators[1] = gen2;
	generators[2] = gen3;
	sound = new sf::Sound();
	buffer = new sf::SoundBuffer();
	IsSelected = true;
}

GenUnit::~GenUnit()
{
}

int GenUnit::GetVolume()
{
	return volume;
}

void GenUnit::HighlightSelected()
{
	cout << "==================================================================================================" << endl;
}

void GenUnit::DisplayGens()
{
	for each (Generator gen in generators)
	{
		if (gen.GetID() == generators[currentIndex].GetID() && IsSelected)
		{
			HighlightSelected();
		}
		string status = (gen.GetIsOn()) ? "On" : "Off";
		cout << "Oscillator " << gen.GetID() << ":" << endl;
		cout << status << " Wave: " << gen.GetWave() << " Amplitude: " << gen.GetAmp() << " Octave: " << gen.GetOctave();
		if (gen.GetWave() == "FM-Sine" || gen.GetWave() == "FM-Square")
		{
			cout << " Mod-Amp: " << gen.GetModAmp();
		}
		cout << endl;
		if (gen.GetID() == generators[currentIndex].GetID() && IsSelected)
		{
			HighlightSelected();
		}
	}
}

void GenUnit::ToggleOn()
{
	generators[currentIndex].ToggleIsOn();
}

void GenUnit::ChangeModAmp(bool up)
{
	if (generators[currentIndex].GetWave() == "FM-Sine" || generators[currentIndex].GetWave() == "FM-Square")
	{
		float ca = generators[currentIndex].GetModAmp();
		if (up)
		{
			if (ca != 1.0)
			{
				ca += 0.1;
			}
		}
		else
		{
			if (ca != 0.1)
			{
				ca -= 0.1;
			}
		}

		generators[currentIndex].SetModAmp(ca);
	}
}

void GenUnit::ChangeCurrentWave()
{
	generators[currentIndex].ChangeWave();
}

void GenUnit::ChangeCurrentOctave(bool up)
{
	if (up)
	{
		generators[currentIndex].UpOctave();
	}
	else
	{
		generators[currentIndex].DownOctave();
	}
}

void GenUnit::ChangeCurrentAmp(bool up)
{
	float ca = generators[currentIndex].GetAmp();
	if (up)
	{
		if (ca != 1.0)
		{
			ca += 0.1;
		}
	}
	else
	{
		if (ca != 0.1)
		{
			ca -= 0.1;
		}
	}

	generators[currentIndex].SetAmp(ca);
}

void GenUnit::ChangeVolume(bool up) 
{
	
	if (up)
	{
		if (volume != 100) {
			volume += 10;
			sound->setVolume(volume);
		}
	}
	else
	{
		if (volume != 0) {
			volume -= 10;
			sound->setVolume(volume);
		}
	}
}

void GenUnit::ChangeCurrentGen(int id)
{
	//receieves -1 if none of the oscillators are selected
	if (id == -1)
	{
		//so ensure none are selected or highlighted
		IsSelected = false;
	}
	else
	{
		//otherwise set the current index of the selected osc
		IsSelected = true;
		currentIndex = id;
	}
}

void GenUnit::DoNothing()
{

}

inline void GenUnit::Dump(vector<sf::Int16>* samples)
{
	samples->clear();
	delete samples;

	for (int i = 0; i < 3; ++i)
	{
		if (generators[i].GetIsOn())
		{
			generators[i].DumpSamples();
		}
	}
}

inline FX*  GenUnit::GetFX()
{
	return fx;
}

inline void GenUnit::Play(double freq)
{
	vector<Int16>* combinedSamples = new vector<Int16>();
	combinedSamples->reserve(44100);
	thread t[3];
	for (int i = 0; i < 3; ++i)
	{
		if (generators[i].GetIsOn()) {
			t[i] = thread(bind(&Generator::PlayNote, &generators[i], freq));
		}
		else t[i] = thread(&GenUnit::DoNothing, this);
	}

	for (int i = 0; i < 3; ++i)
	{
		t[i].join();
	}
	short sample;
	float s1, s2, s3;
	for (int i = 0; i < 44100; i++)
	{
		s1 = generators[0].GetSamples(i) * 0.2;
		s2 = generators[1].GetSamples(i) * 0.2;
		s3 = generators[2].GetSamples(i) * 0.2;
		sample = s1 + s2 + s3;

		combinedSamples->push_back(sample);
		sample = 0;
	}
	fx->RouteSignal(combinedSamples);
	buffer->loadFromSamples(&(*combinedSamples)[0], combinedSamples->size(), 1, 44100);

	thread dumpThread(bind(&GenUnit::Dump, this, combinedSamples));

	sound->setBuffer(*buffer);
	sound->play();
	dumpThread.join();
}
