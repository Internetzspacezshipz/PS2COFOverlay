#pragma once

//avoid includes by using bogus templates.
namespace MiscUtils
{
	// Get the horizontal and vertical screen sizes in pixel https://stackoverflow.com/questions/8690619/how-to-get-screen-resolution-in-c
	template<typename _DO_NOT_USE = void>
	void GetDesktopResolution(int& horizontal, int& vertical)
	{
		RECT desktop;
		// Get a handle to the desktop window
		const HWND hDesktop = GetDesktopWindow();
		// Get the size of screen to the variable desktop
		GetWindowRect(hDesktop, &desktop);
		// The top left corner will have coordinates (0,0)
		// and the bottom right corner will have coordinates
		// (horizontal, vertical)
		horizontal = desktop.right;
		vertical = desktop.bottom;
	}

	//Countdown state system
	template<typename ValueType>
	struct Countdown
	{
		ValueType TimeUntilCompleted = 0;

		ValueType Val() const { return TimeUntilCompleted; }
		bool Completed() const { return TimeUntilCompleted <= 0; }

		void Set(ValueType Time)
		{
			TimeUntilCompleted = Time;
		}

		void Tick(ValueType ReduceBy)
		{
			if (TimeUntilCompleted > 0)
			{
				TimeUntilCompleted -= ReduceBy;
			}
		}
	};
}

