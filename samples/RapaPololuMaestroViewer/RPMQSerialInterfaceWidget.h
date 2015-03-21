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
#pragma once

#ifdef _MSC_VER
	#pragma warning( push )
	#pragma warning ( disable : 4127 )
	#pragma warning ( disable : 4231 )
	#pragma warning ( disable : 4251 )
	#pragma warning ( disable : 4800 )
#endif
#include <QTimer>
#include <QFrame>
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QMessageBox>
#include <QStatusBar>
#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#include "RPMSerialInterface.h"

namespace RPM
{

class QChannelWidget;

class QSerialInterfaceWidget :	public QFrame							
{ 
	Q_OBJECT

public:
	QSerialInterfaceWidget( QWidget* parent, SerialInterface* serialInterface, unsigned char numChannels, Qt::WindowFlags flags=0 );
	virtual ~QSerialInterfaceWidget();

	SerialInterface*	getSerialInterface() const  { return mSerialInterface; }
	QStatusBar*			getStatusBar() const		{ return mStatusBar; }

protected slots:
	void				onChannelError( const QString& message );
	void				onGoHomeButtonClicked(bool checked);
	void				onAutoRefreshButtonClicked(bool checked);

private:
	void				createWidgets( unsigned char numChannels );
	void				displayError();

	SerialInterface*	mSerialInterface;
	QTimer*				mUpdateTimer;
	QPushButton*		mGoHomeButton;
	QPushButton*		mAutoRefreshButton;
	QStatusBar*			mStatusBar;
};

class QChannelWidget : public QWidget
{
	Q_OBJECT
	
public:
	QChannelWidget( QWidget* parent, SerialInterface* serialInterface, unsigned char channelNumber  );

signals:
	void channelError(const QString& message);

public slots:
	void updateWidgetsFromHardware();

private slots:
	void onPositionSpinBoxChanged( int value );
	void onPositionSliderChanged( int value );
	void onSpeedSpinBoxChanged( int value );
	void onAccelerationSpinBoxChanged( int value );
	
private:
	void createWidgets();
	void updateWidgets();
	void displayError();

	SerialInterface*	mSerialInterface;
	unsigned char		mChannelNumber;
	unsigned short		mTargetValue;	// When setting the target to Maestro, we cache the value and use it for the UI. It differs from the actual position returned by the Maestro 

	QSpinBox*			mPositionSpinBox;
	QSpinBox*			mTargetSpinBox;
	QSlider*			mTargetSlider;
	QSpinBox*			mSpeedSpinBox;
	QSpinBox*			mAccelerationSpinBox;
};

}