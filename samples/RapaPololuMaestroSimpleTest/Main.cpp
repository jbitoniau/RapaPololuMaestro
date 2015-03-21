/*
   The MIT License (MIT) (http://opensource.org/licenses/MIT)
   
   Copyright (c) 2015 Jacques Menuet
   
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/
#include <stdio.h>
#include <cmath>

#if _WIN32
	#include <Windows.h>
#else
	#include <unistd.h>
	#include <sys/timeb.h>
	#include <time.h>
	#include <sys/time.h>
#endif

#include "RPMSerialInterface.h"

// A utility class to provide cross-platform sleep and simple time methods
class Utils
{
public:
	static void sleep( unsigned int _Milliseconds );
	static unsigned long long int getTickFrequency();
	static unsigned long long int getTimeAsTicks();
	static unsigned int getTimeAsMilliseconds();

private:
	static unsigned long long int mInitialTickCount;
};

int main(int /*argc*/, char** /*argv*/)
{
	unsigned char deviceNumber = 12;
	unsigned char channelNumber = 2;

#ifdef _WIN32
	std::string portName = "COM4";
#else
	std::string portName = "/dev/ttyACM0";
	//std::string portName = "/dev/cu.usbmodem00031501"; // Example for Mac OS, the Maestro creates two devices, use the one with the lowest number (the command port)
#endif
	unsigned int baudRate = 9600;
	printf("Creating serial interface '%s' at %d bauds\n", portName.c_str(), baudRate);
	std::string errorMessage;
	RPM::SerialInterface* serialInterface = RPM::SerialInterface::createSerialInterface( portName, baudRate, &errorMessage );
	if ( !serialInterface )
	{
		printf("Failed to create serial interface. %s\n", errorMessage.c_str());
		return -1;
	}

	// Generate a sinusoid signal to send to the PololuInterface
	const float pi = 3.141592653589793f;
	const unsigned int channelMinValue = 4000;
	const unsigned int channelMaxValue = 8000;
	const unsigned int channelValueRange = channelMaxValue - channelMinValue;
	const unsigned int signalPeriodInMs = 2000;
	unsigned int time0 = Utils::getTimeAsMilliseconds();
	unsigned int timeSinceStart = 0;
	while ( timeSinceStart < 5000 )
	{
		float k = sin( (pi*2)/signalPeriodInMs * timeSinceStart ) * (float)(channelValueRange/2);
		float channelValue = (float)channelMinValue + (float)channelValueRange/2 + k;
		printf("\rchannelValue=%d", (unsigned int)channelValue );
		serialInterface->setTargetCP( channelNumber, (unsigned short)channelValue );
		timeSinceStart = Utils::getTimeAsMilliseconds() - time0;
		Utils::sleep(5);
	}

	// Test all the SerialInterface methods 
	unsigned short position = 0;
	bool ret = false;
	ret = serialInterface->setSpeedCP( channelNumber, 0 );
	ret = serialInterface->setAccelerationCP( channelNumber, 0 );
	Utils::sleep(1000);

	position = 4000;
	ret = serialInterface->setTargetCP( channelNumber, position );
	printf("setTargetCP(%d, %d) (ret=%d)\n", channelNumber, position, ret );
	position = 0;
	ret = serialInterface->getPositionCP( channelNumber, position );
	printf("getPositionCP(%d) (ret=%d position=%d)\n", channelNumber, ret, position );
	position = 0;
	ret = serialInterface->getPositionPP( deviceNumber, channelNumber, position );
	printf("getPositionPP(%d, %d) (ret=%d position=%d)\n", deviceNumber, channelNumber, ret, position );

	Utils::sleep(1000);
	position = 8000;
	ret = serialInterface->setTargetCP( channelNumber, position );
	printf("setTargetCP(%d, %d) (ret=%d)\n", channelNumber, position, ret );
	position = 0;
	ret = serialInterface->getPositionCP( channelNumber, position );
	printf("getPositionCP(%d) (ret=%d position=%d)\n", channelNumber, ret, position );
	position = 0;
	ret = serialInterface->getPositionPP( deviceNumber, channelNumber, position );
	printf("getPositionPP(%d, %d) (ret=%d position=%d)\n", deviceNumber, channelNumber, ret, position );

	Utils::sleep(1000);
	position = 4000;
	ret = serialInterface->setTargetPP( deviceNumber, channelNumber, position );
	printf("setTargetPP(%d, %d, %d) (ret=%d)\n", deviceNumber, channelNumber, position, ret );
	position = 0;
	ret = serialInterface->getPositionCP( channelNumber, position );
	printf("getPositionCP(%d) (ret=%d position=%d)\n", channelNumber, ret, position );
	position = 0;
	ret = serialInterface->getPositionPP( deviceNumber, channelNumber, position );
	printf("getPositionPP(%d, %d) (ret=%d position=%d)\n", deviceNumber, channelNumber, ret, position );

	Utils::sleep(1000);
	unsigned char normalizedTarget = 254;
	ret = serialInterface->setTargetMSSCP( channelNumber, normalizedTarget );
	printf("setTargetMSSCP(%d, %d) (ret=%d)\n", channelNumber, normalizedTarget, ret );
	position = 0;
	ret = serialInterface->getPositionCP( channelNumber, position );
	printf("getPositionCP(%d) (ret=%d position=%d)\n", channelNumber, ret, position );
	position = 0;
	ret = serialInterface->getPositionPP( deviceNumber, channelNumber, position );
	printf("getPositionPP(%d, %d) (ret=%d position=%d)\n", deviceNumber, channelNumber, ret, position );
	Utils::sleep(1000);

	Utils::sleep(1000);
	unsigned short speed = 177;
	ret = serialInterface->setSpeedCP( channelNumber, speed );
	printf("setSpeedCP(%d, %d) (ret=%d)\n", channelNumber, speed, ret );
	Utils::sleep(1000);
	speed = 5;
	ret = serialInterface->setSpeedPP( deviceNumber, channelNumber, speed );
	printf("setSpeedPP(%d, %d, %d) (ret=%d)\n", deviceNumber, channelNumber, speed, ret );
	
	Utils::sleep(1000);
	unsigned char acceleration = 7;
	ret = serialInterface->setAccelerationCP( channelNumber, acceleration );
	printf("setAccelerationCP(%d, %d) (ret=%d)\n", channelNumber, acceleration, ret );
	Utils::sleep(1000);
	acceleration = 11;
	ret = serialInterface->setAccelerationPP( deviceNumber, channelNumber, acceleration );
	printf("setAccelerationPP(%d, %d, %d) (ret=%d)\n", deviceNumber, channelNumber, acceleration, ret );
	Utils::sleep(1000);
	
	Utils::sleep(1000);
	position = 4000;
	ret = serialInterface->setTargetPP( deviceNumber, channelNumber, position );
	printf("setTargetPP(%d, %d, %d) (ret=%d)\n", deviceNumber, channelNumber, position, ret );
	bool areServosMoving = false;
	ret = serialInterface->getMovingStateCP( areServosMoving );
	while ( areServosMoving )
	{
		printf(".");
		Utils::sleep(10);
		ret = serialInterface->getMovingStateCP( areServosMoving );
	}
	printf("\n");

	Utils::sleep(1000);
	position = 8000;
	ret = serialInterface->setTargetPP( deviceNumber, channelNumber, position );
	printf("setTargetPP(%d, %d, %d) (ret=%d)\n", deviceNumber, channelNumber, position, ret );
	areServosMoving = false;
	ret = serialInterface->getMovingStatePP( deviceNumber, areServosMoving );
	while ( areServosMoving )
	{
		printf(".");
		Utils::sleep(10);
		ret = serialInterface->getMovingStatePP( deviceNumber, areServosMoving );
	}
	printf("\n");
	
	Utils::sleep(1000);
	unsigned short errors = 0;
	ret = serialInterface->getErrorsCP( errors );
	printf("getErrorsCP() (ret=%d errors=%d)\n", ret, errors );
	
	Utils::sleep(1000);
	errors = 0;
	ret = serialInterface->getErrorsPP( deviceNumber, errors );
	printf("getErrorsPP(%d) (ret=%d errors=%d)\n", deviceNumber, ret, errors );

	Utils::sleep(1000);
	//position = 8000;
	//ret = serialInterface->setTargetPP( deviceNumber, channelNumber, position );
	//printf("setTargetPP(%d, %d, %d) (ret=%d)\n", deviceNumber, channelNumber, position, ret );
	ret = serialInterface->goHomeCP();
	printf("goHomeCP() (ret=%d)\n", ret );

	Utils::sleep(1000);
	//position = 8000;
	//ret = serialInterface->setTargetPP( deviceNumber, channelNumber, position );
	//printf("setTargetPP(%d, %d, %d) (ret=%d)\n", deviceNumber, channelNumber, position, ret );
	ret = serialInterface->goHomePP( deviceNumber );
	printf("goHomePP(%d) (ret=%d)\n", deviceNumber, ret );

	Utils::sleep(1000);
	printf("Deleting serial interface...\n");
	delete serialInterface;
	serialInterface = NULL;

	return 0;
}

// Utils class implementation
void Utils::sleep( unsigned int _Milliseconds )
{
#if _WIN32
	::Sleep( _Milliseconds );
#else
	struct timespec l_TimeSpec;
	l_TimeSpec.tv_sec = _Milliseconds / 1000;
	l_TimeSpec.tv_nsec = (_Milliseconds % 1000) * 1000000;
	struct timespec l_Ret;
	nanosleep(&l_TimeSpec,&l_Ret);
#endif
}

unsigned long long int Utils::getTickFrequency()
{
#if _WIN32
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	return frequency.QuadPart;
#else
	// The gettimeofday function returns the time in microseconds. So it's frequency is 1,000,000.
	return 1000000;
#endif
}

unsigned long long int Utils::getTimeAsTicks()
{
	unsigned long long int tickCount;
#if _WIN32
	LARGE_INTEGER l;
	QueryPerformanceCounter(&l);
	tickCount = l.QuadPart;
#else
	struct timeval p;
	gettimeofday(&p, NULL);	// Gets the time since the Epoch (00:00:00 UTC, January 1, 1970) in sec, and microsec
	tickCount = (p.tv_sec * 1000LL * 1000LL) + p.tv_usec;
#endif
	if ( mInitialTickCount==0xffffffffffffffffUL )
		mInitialTickCount = tickCount;
	tickCount -= mInitialTickCount;
	return tickCount;
}

unsigned int Utils::getTimeAsMilliseconds()
{
	unsigned int millecondsTime = static_cast<unsigned int>( (getTimeAsTicks() * 1000) / getTickFrequency() );
	return millecondsTime;
}

unsigned long long int Utils::mInitialTickCount = 0xffffffffffffffffUL;
