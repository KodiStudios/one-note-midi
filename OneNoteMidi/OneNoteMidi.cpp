// Copyright (c) Kodi Studios 2021.
// Licensed under the MIT license.

// REQUIREMENTS!
// 
// Add following lib file to C++ Linker:
// winmm.lib
// For example, in Visual Studio:
// Project > Properties > Configuration Properties > Linker > Input > Additional Dependencies
// Add: winmm.lib

#include <Windows.h> // Midi Apis

#include <exception>
#include <iostream>
#include <thread>
#include <filesystem>

#include "CLI11.hpp" // Command Line Argument Parser Library

// Midi message is 4 bytes.
// Windows Midi midiOutShortMsg() Api passes
// those 4 bytes as DWORD type.
// Use Union structure to easily overlap DWORD onto 
// Midi's 4 bytes.
union MidiMessage {
	BYTE bData[4];
	DWORD dwData{ 0 }; // Note: because it's a "union", this also zeros out all 4 bytes in bData array
};

// Wraps Midi MMRESULT into Exception object
class MidiException : public std::exception
{
public:
	explicit MidiException(MMRESULT midiFuncResult)
		: MidiFuncResult{ midiFuncResult }
		, m_exceptionMessage{ GenerateExceptionMessage(midiFuncResult) }
	{
	}

	char const* what() const override
	{
		return m_exceptionMessage.c_str();
	}

private:
	static std::string GenerateExceptionMessage(MMRESULT midiFuncResult)
	{
		std::stringstream s;
		s << "Midi Error: " << midiFuncResult;
		return s.str();
	}

public:
	const MMRESULT MidiFuncResult;

private:
	std::string m_exceptionMessage;
};

// Helper Macro
// Invokes MidiFunc Api
// If MidiFunc Api returns error, throws exception
#define VerifyMidi(midiFuncAndParams)                              \
{                                                                  \
	MMRESULT midiFuncResult = midiFuncAndParams;                   \
	if (midiFuncResult != MMSYSERR_NOERROR)                        \
	{                                                              \
		throw MidiException(midiFuncResult);                       \
	}                                                              \
}

// Helper Function
// If value falls out of limit, throws exception
// Max Value is inclusive, allowed
void VerifyLimit(uint32_t currentValue, uint32_t maxValue, const char* valueName)
{
	if (currentValue > maxValue)
	{
		std::stringstream s;
		s << valueName << ", Current: " << currentValue << ", Max: " << maxValue;
		throw std::invalid_argument(s.str().c_str());
	}
}

void SelectMidiInstrument(
	HMIDIOUT hMidiOut,
	uint8_t channel,       // 4 bits, 0 to 15
	uint8_t instrument     // 7 bits, 0 to 127
)
{
	VerifyLimit(channel, 15, "Channel");
	VerifyLimit(instrument, 127, "Instrument");

	// Select Midi Instrument Protocol:
	// [0] Status byte          : 0b 1100 CCCC
	//     Select Instrument Signature      : 0b 1100
	//     Channel 4-bits                   : 0b CCCC
	// [1] Instrument 7-bits    : 0b 0III IIII
	// [2] Unused               : 0b 0000 0000
	// [3] Unused               : 0b 0000 0000

	const uint8_t SetInstrumentSignature = 0b1100;
	uint8_t statusByte = SetInstrumentSignature; // 0b 0000 1100
	statusByte = statusByte << 4;                // 0b 1100 0000
	statusByte |= channel;                       // 0b 1100 CCCC

	MidiMessage midiMessage;
	midiMessage.bData[0] = statusByte;       // MIDI Status byte
	midiMessage.bData[1] = instrument;       // First MIDI data byte
	// Bytes [2] and [3] are unused

	// Invoke Windows Midi Api
	VerifyMidi(midiOutShortMsg(hMidiOut, midiMessage.dwData));
}

// Plays Midi Note
// To Stop playing, set velocity parameter to 0
void SendMidiNote(
	HMIDIOUT hMidiOut,
	uint8_t channel,  // 4 bits, 0 to 15
	uint8_t pitch,    // 7 bits, 0 to 127
	uint8_t velocity  // 7 bits, 0 to 127
)
{
	VerifyLimit(channel, 15, "Channel");
	VerifyLimit(pitch, 127, "Pitch");
	VerifyLimit(velocity, 127, "Velocity");

	// Note On Protocol:
	// [0] Status byte     : 0b 1001 CCCC
	//     Note On Signature   : 0b 1001
	//     Channel 4-bits      : 0b CCCC
	// [1] Pitch 7-bits    : 0b 0PPP PPPP
	// [2] Velocity 7-bits : 0b 0VVV VVVV
	// [3] Unused          : 0b 0000 0000
	// Reference: https://www.cs.cmu.edu/~music/cmsip/readings/MIDI%20tutorial%20for%20programmers.html

	// To Turn Note Off, simply pass 0 as Velocity (Volume)

	const uint8_t NoteOnSignature = 0b1001;
	uint8_t statusByte = NoteOnSignature;      // 0b 0000 1001
	statusByte = statusByte << 4;              // 0b 1001 0000
	statusByte = statusByte | channel;         // 0b 1001 CCCC

	MidiMessage midiMessage;
	midiMessage.bData[0] = statusByte;  // MIDI Status byte
	midiMessage.bData[1] = pitch;       // First MIDI data byte
	midiMessage.bData[2] = velocity;    // Second MIDI data byte
	// Byte [3] is unused

	// Invoke Windows Midi Api
	VerifyMidi(midiOutShortMsg(hMidiOut, midiMessage.dwData));
}

void PlayNote(
	uint8_t channel,
	uint8_t instrument,
	uint8_t pitch, // Note
	uint8_t velocity, // Volume
	uint32_t noteLength)
{
	std::cout
		<< "Playing "
		<< "Channel: " << (uint32_t)channel << ", "
		<< "Instrument: " << (uint32_t)instrument << ", "
		<< "Pitch: " << (uint32_t)pitch << ", "
		<< "Velocity: " << (uint32_t)velocity << ", "
		<< "Length: " << (uint32_t)noteLength << "\n";

	HMIDIOUT hMidiOut;
	VerifyMidi(midiOutOpen(
		/*out*/ &hMidiOut,
		/*uDeviceID*/ 0, // System's Midi device is at index 0
		/*dwCallback*/ NULL,
		/*dwInstance*/ NULL,
		/*fdwOpen*/ CALLBACK_NULL
	));

	SelectMidiInstrument(hMidiOut, channel, instrument);

	// Start Playing Note
	SendMidiNote(hMidiOut, channel, pitch, velocity);
	std::this_thread::sleep_for(std::chrono::milliseconds(noteLength));
	SendMidiNote(hMidiOut, channel, pitch, 0); // Stop

	VerifyMidi(midiOutClose(hMidiOut));
}

namespace ArgumentParsing {

	class AppArguments
	{
	public:
		uint8_t Channel{ 0 };
		uint8_t Instrument{ 0 };       // 0 is Grand Piano
		uint8_t Pitch{ 60 };           // 60 is C Note
		uint8_t Velocity{ 90 };       // 127 is Max Velocity (Volume)
		uint32_t Length{ 3000 };       // Note Length, in Milliseconds
	};

	void VerifyLimits(const AppArguments& appArguments)
	{
		VerifyLimit(appArguments.Channel, 15, "-c");
		VerifyLimit(appArguments.Instrument, 127, "-i");
		VerifyLimit(appArguments.Pitch, 127, "-p");
		VerifyLimit(appArguments.Velocity, 127, "-v");
	}

	void PrintHelp(const char* appName)
	{
		AppArguments defaultArguments;
		std::cout << "Plays one note through Midi\n";
		std::cout << "\n";
		std::cout << "Usage: " << appName << " [FLAGS]";
		std::cout << "\n";
		std::cout << "  -c [0-15]" << "           Channel. Default: " << (uint32_t)defaultArguments.Channel << "\n";
		std::cout << "  -i [0-127]" << "          Instrument. Default: " << (uint32_t)defaultArguments.Instrument << " (Grand Piano)\n";
		std::cout << "  -p [0-127]" << "          Pitch (Note). Default: " << (uint32_t)defaultArguments.Pitch << " (Middle C Note)\n";
		std::cout << "  -v [0-127]" << "          Velocity (Volume). Default: " << (uint32_t)defaultArguments.Velocity << " (" << defaultArguments.Velocity / 127.0 * 100.0 << "% Loud)\n";
		std::cout << "  -l [milliseconds]" << "   Length (Note Length), in Milliseconds. Default: " << (uint32_t)defaultArguments.Length << " milliseconds\n";
		std::cout << "  -?" << "            Prints this help\n";
		std::cout << "\n";
		std::cout << "Examples:\n";
		std::cout << "\n";
		std::cout << appName << " -i 24 -p 80\n";
		std::cout << "Play Guitar Note\n";
		std::cout << "\n";
		std::cout << appName << " -c 1 -i 24 -p 81 -v 120 -l 2000\n";
		std::cout << "Sets Channel 1 to Guitar, Plays G Note, at Volume 120, for 2 seconds\n";

		// Violin: 41
		// C Note: 60
	}

	std::unique_ptr<AppArguments> Parse(int argc, char* argv[])
	{
		AppArguments appArguments;

		CLI::App app{ "One Note Midi", "Plays the requested Midi Note" };

		app.add_option("-c", /*out*/ appArguments.Channel, "Channel");
		app.add_option("-i", /*out*/ appArguments.Instrument, "Instrument");
		app.add_option("-p", /*out*/ appArguments.Pitch, "Pitch (Note)");
		app.add_option("-v", /*out*/ appArguments.Velocity, "Velocity (Volume)");
		app.add_option("-l", /*out*/ appArguments.Length, "Note Length");

		try
		{
			app.parse(argc, argv);
		}
		catch (CLI::CallForHelp)
		{
			// Weird Pattern
			PrintHelp(/*appName*/ std::filesystem::path(argv[0]).filename().generic_u8string().c_str());
			return nullptr;
		}
		catch (CLI::Error e)
		{
			// Any other parse error
			std::cout << "Flag Error: " << e.what() << "\n";
			PrintHelp(/*appName*/ std::filesystem::path(argv[0]).filename().generic_u8string().c_str());
			return nullptr;
		}

		try
		{
			VerifyLimits(appArguments);
		}
		catch (std::invalid_argument e)
		{
			std::cout << "Flag Limit Error: " << e.what() << "\n";
			PrintHelp(/*appName*/ std::filesystem::path(argv[0]).filename().generic_u8string().c_str());
			return nullptr;
		}

		return std::make_unique<AppArguments>(appArguments);
	}
}

int main(int argc, char* argv[])
{
	try
	{
		std::unique_ptr<ArgumentParsing::AppArguments> appArguments = ArgumentParsing::Parse(argc, argv);
		if (appArguments)
		{
			PlayNote(
				appArguments->Channel,
				appArguments->Instrument,
				appArguments->Pitch,
				appArguments->Velocity,
				appArguments->Length);
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception: " << e.what() << "\n";
		throw;
	}
	catch (...)
	{
		std::cout << "Exception: " << "Unknown" << "\n";
		throw;
	}

	return 0;
}
