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
#include <iostream>

#ifdef _MSC_VER
	#pragma warning( push )
	#pragma warning ( disable : 4127 )
	#pragma warning ( disable : 4231 )
	#pragma warning ( disable : 4251 )
	#pragma warning ( disable : 4800 )
#endif
#include <QApplication>
#ifdef _MSC_VER
	#pragma warning( pop )
#endif

#include "RPMSerialInterface.h"
#include "RPMQSerialInterfaceWidget.h"

int main( int argc, char** argv )
{		
	QApplication app( argc, argv );
	
	std::string portName;
#ifdef _WIN32
	portName = "COM4";
#else
	portName = "/dev/ttyACM0";
#endif
	if ( argc>=2 )
		portName = argv[1];

	unsigned char numChannels = 6; 
	if ( argc>=3 )
		numChannels = static_cast<unsigned char>( atoi( argv[2] ) );
	
	std::cout << "Opening Pololu Maestro on serial interface \"" << portName << "\"..." << std::endl;
	std::string errorMessage;
	RPM::SerialInterface* serialInterface = RPM::SerialInterface::createSerialInterface(portName, 9600, &errorMessage );
	if ( !serialInterface )
		std::cerr << "Error: " << errorMessage << std::endl;

	std::cout << "Starting widget with " << static_cast<int>(numChannels) << " channels..." << std::endl;
	RPM::QSerialInterfaceWidget* serialInterfaceWidget = new RPM::QSerialInterfaceWidget(NULL, serialInterface, numChannels);
	
	serialInterfaceWidget->setWindowTitle( QString("Pololu Maestro - ") + QString(portName.c_str()) );
	serialInterfaceWidget->resize(180, 150);
	serialInterfaceWidget->show();
	if ( !serialInterface )
		serialInterfaceWidget->getStatusBar()->showMessage( errorMessage.c_str() );
	int ret = app.exec();

	delete serialInterfaceWidget;
	serialInterfaceWidget = NULL;

	delete serialInterface;
	serialInterface = NULL;

	return ret;
}