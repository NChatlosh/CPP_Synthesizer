#pragma once
#include <SFML\Audio.hpp>
#include <math.h>
#include <SFML\Window.hpp>
#include <vector>
#include <functional>
#include "Generator.h"
#include <iostream>
#include <SFML\System.hpp>
#include "GenUnit.h"

using namespace std;
const enum Types{filter, LFO, Amp, Osc};
class UI
{
public:
	void FindKey(sf::Event* event);
	UI();
	~UI();
	void DisplayValues();
	void DisplayInstructions();
	void ChangeKeyOctave(bool up);
	void ChangeSelection(bool up);
	void Highlight(Types t);
	void LeftRightHandler(bool up);
	void OneKeyHandler();
	void ShiftHandler(bool up);
private:
	GenUnit unit;
	FX* fx;
	int oct;
	int currentID;
	Types currentType;
};

UI::UI()
{
	currentID = 3;
	fx = unit.GetFX();
	oct = 1;
	currentType = Osc;
}

UI::~UI()
{
	
}

void UI::ChangeKeyOctave(bool up)
{
	if (up)
	{
		if (oct != 3)
		{
			oct++;
		}
	}
	else {
		if (oct != 0)
		{
			oct--;
		}
	}
}

inline void UI::ChangeSelection(bool up)
{
	if (up)
	{
		currentID++;
		if (currentID >= 5)
			currentID = 5;
	}
	else
	{
		currentID--;
		if (currentID <= 0)
			currentID = 0;
	}

	switch (currentID)
	{
	case 0:
		unit.ChangeCurrentGen(-1);//Amp Env
		currentType = Amp;
		break;
	case 1:
		unit.ChangeCurrentGen(-1);//Filter
		currentType = filter;
		break;
	case 2:
		unit.ChangeCurrentGen(-1);//LFO
		currentType = LFO;
		break;
	case 3:
		unit.ChangeCurrentGen(0); //first osc
		currentType = Osc;
		break;
	case 4:
		unit.ChangeCurrentGen(1);//second osc
		currentType = Osc;
		break;
	case 5:
		unit.ChangeCurrentGen(2);//third osc
		currentType = Osc;
		break;
	}

}

inline void UI::Highlight(Types t)
{
	if(t == currentType)
	cout << "==================================================================================================" << endl;
}

inline void UI::LeftRightHandler(bool up)
{
	switch (currentType)
	{
	case Amp:
		fx->SetAttack(up);
		break;
	case filter:
		fx->ChangeCutoff(up);
		break;
	case LFO:
		fx->ChangeLFOFreq(up);
		break;
	case Osc:
		unit.ChangeCurrentAmp(up);
		break;
	default:
		break;
	}
}

inline void UI::OneKeyHandler()
{
	switch (currentType)
	{
	case filter:
		fx->ToggleFilter();
		break;
	case LFO:
		fx->ToggleLFO();
		break;
	case Osc:
		unit.ToggleOn();
		break;
	default:
		break;
	}
}

inline void UI::ShiftHandler(bool up)
{
	switch (currentType)
	{
	case Amp:
		fx->SetRelease(up);
		break;
	case Osc:
		unit.ChangeCurrentOctave(up);
		break;
	default:
		break;
	}
}

void UI::DisplayValues()
{
	//*fx = unit.GetFX();
	system("CLS");
	string octave = "C4-B4";
	switch (oct)
	{
	case 0:
		octave = "C3-B3";
		break;
	case 1:
		//no change
		break;
	case 2:
		octave = "C5-B5";
		break;
	case 3:
		octave = "C6-B6";
		break;
	default:
		break;
	}
	string fOn = fx->GetFilterOn() ? "ON" : "OFF";
	string LFOon = fx->GetLFOon() ? "ON" : "OFF";
	cout << "[MASTER]  " << " Volume: " << unit.GetVolume() << " Octave: " << octave << endl;
	Highlight(Amp);
	cout << "[Amp Envelope]   Attack: " << fx->GetAttack() << " Release: " << fx->GetRelease() << endl;
	Highlight(Amp);
	Highlight(filter);
	cout << "[Filter]  Status: " << fOn << " Cutoff: " << fx->GetCutoff() << " Type: " << fx->GetFiltType() << endl;
	Highlight(filter);
	Highlight(LFO);
	cout << "[LFO]  Status: " << LFOon << " Freq: " << fx->GetLFOFreq() <<  endl;
	Highlight(LFO);
	unit.DisplayGens();
	DisplayInstructions();
}

void UI::DisplayInstructions()
{
	cout << "" << endl;
	cout << "//////////////////////////////////////////////////////////////////////" << endl;
	cout << "Key Instructions:" << endl;
	cout << "[(-/+): Change Master Volume] [(z,x): Change Keyboard Octave] [(a-l): natural notes] [(w,e,t,y,u): sharps]" << endl;
	cout << "[(up/down): Change selection] ";

	switch (currentType)
	{
	case Amp:
		cout << "[(left/right): Change Attack] [(LShift, RShift): Change Release]" << endl;
		break;
	case filter:
		cout << "[(left/right): Change cutoff frequency] [(1): Toggle Filter on/off]" << endl;
		cout << "[(SpaceBar): Change Filter Type]" << endl;
		break;
	case LFO:
		cout << "[(left/right): Change LFO frequency] [(1): Toggle LFO on/off]" << endl;
		break;
	case Osc:
		cout << "[(left/right): Change oscillator amp] [(LShift, RShift): Change osc octave]" << endl;
		cout << "[(LCtrl/RCtrl): Change Modulator Amp] [(1): Toggle Osc on/off] [(SpaceBar): Change Wave]" << endl;
		break;
	default:
		break;
	}
}


void UI::FindKey(sf::Event* event) 
{
	//*fx = unit.GetFX();
	double freq;
	switch ((*event).key.code)
	{
	case Keyboard::A:
		freq = 261.626 * OctaveMult[oct]; // middle c
		unit.Play(freq);
		break;
	case Keyboard::S:
		freq = 293.665 * OctaveMult[oct]; // D4
		unit.Play(freq);
		break;
	case Keyboard::D:
		freq = 329.628 * OctaveMult[oct]; // E4
		unit.Play(freq);
		break;
	case Keyboard::F:
		freq = 349.228 * OctaveMult[oct]; // F4
		unit.Play(freq);
		break;
	case Keyboard::G:
		freq = 391.995 * OctaveMult[oct]; // G4
		unit.Play(freq);
		break;
	case Keyboard::H: // A4
		freq = 440 * OctaveMult[oct];
		unit.Play(freq);
		break;
	case Keyboard::J:
		freq = 493.883 * OctaveMult[oct];//B4
		unit.Play(freq);
		break;
	case Keyboard::K:
		freq = 523.251 * OctaveMult[oct];//B4
		unit.Play(freq);
		break;
	case Keyboard::W:
		freq = 277.181 * OctaveMult[oct];//C#4
		unit.Play(freq);
		break;
	case Keyboard::E:
		freq = 311.127 * OctaveMult[oct];//D#4
		unit.Play(freq);
		break;
	case Keyboard::T:
		freq = 369.994 * OctaveMult[oct];//F#4
		unit.Play(freq);
		break;
	case Keyboard::Y:
		freq = 415.305 * OctaveMult[oct];//G#4
		unit.Play(freq);
		break;
	case Keyboard::U:
		freq = 466.164 * OctaveMult[oct];//A#4
		unit.Play(freq);
		break;
	case Keyboard::Z:
		ChangeKeyOctave(false);
		break;
	case Keyboard::X:
		ChangeKeyOctave(true);
		break;
	case Keyboard::Up:
			ChangeSelection(false);
		break;
	case Keyboard::Down:
			ChangeSelection(true);
		break;
	case Keyboard::Equal:
		unit.ChangeVolume(true);
		break;
	case Keyboard::Dash:
		unit.ChangeVolume(false);
		break;
	case Keyboard::Left:
		LeftRightHandler(false);
		break;
	case Keyboard::Right:
		LeftRightHandler(true);
		break;
	case Keyboard::LShift:
		ShiftHandler(false);
		break;
	case Keyboard::RShift:
		ShiftHandler(true);
		break;
	case Keyboard::LControl:
		unit.ChangeModAmp(false);
		break;
	case Keyboard::RControl:
		unit.ChangeModAmp(true);
		break;
	case Keyboard::Space:
		if (currentType == Osc)
			unit.ChangeCurrentWave();
		else
			fx->ChangeFilterType();
		break;
	case Keyboard::Num1:
		OneKeyHandler();
		break;
	default:
		//do nothing
		break;
	}
}