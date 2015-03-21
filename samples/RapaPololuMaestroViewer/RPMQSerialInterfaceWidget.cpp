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
#include "RPMQSerialInterfaceWidget.h"

#include <assert.h>

namespace RPM
{

/*
	QSerialInterfaceWidget
*/
QSerialInterfaceWidget::QSerialInterfaceWidget( QWidget* parent, SerialInterface* serialInterface, unsigned char numChannels, Qt::WindowFlags flags )
	: QFrame(parent, flags),
	  mSerialInterface(serialInterface),
	  mUpdateTimer(NULL),
	  mGoHomeButton(NULL),
	  mAutoRefreshButton(NULL),
	  mStatusBar(NULL)
{
	createWidgets(numChannels);
}

QSerialInterfaceWidget::~QSerialInterfaceWidget()
{	
}

void QSerialInterfaceWidget::createWidgets( unsigned char numChannels )
{
	bool ret = false;
	mUpdateTimer = new QTimer();
	mUpdateTimer->setInterval(200);
	
	QVBoxLayout* mainLayout = new QVBoxLayout();
	setLayout(mainLayout);

	QHBoxLayout* buttonLayout = new QHBoxLayout();
	mainLayout->addLayout(buttonLayout);

	mGoHomeButton = new QPushButton("Go Home", this);
	ret = connect( mGoHomeButton, SIGNAL( clicked(bool) ), SLOT( onGoHomeButtonClicked(bool) ) );
	assert(ret);
	buttonLayout->addWidget(mGoHomeButton); 

	mAutoRefreshButton = new QPushButton("Auto Refresh", this);
	mAutoRefreshButton->setCheckable(true);
	ret = connect( mAutoRefreshButton, SIGNAL( clicked(bool) ), SLOT( onAutoRefreshButtonClicked(bool) ) );
	assert(ret);
	mAutoRefreshButton->setChecked(false);
	onAutoRefreshButtonClicked(false);

	buttonLayout->addWidget(mAutoRefreshButton); 
	
	buttonLayout->addStretch(); 

	for ( unsigned char i=0; i<numChannels; ++i )
	{
		QChannelWidget* channelWidget = new QChannelWidget( this, mSerialInterface, i );
		ret = connect( mUpdateTimer, SIGNAL( timeout() ), channelWidget, SLOT( updateWidgetsFromHardware() ) );
		assert(ret);
		ret = connect( channelWidget, SIGNAL( channelError(const QString&) ), this, SLOT( onChannelError(const QString&) ) );
		assert(ret);
		mainLayout->addWidget( channelWidget );
	}
	mainLayout->addStretch();

	mStatusBar = new QStatusBar(this);
	mainLayout->addWidget( mStatusBar );
	
	if ( !mSerialInterface )
		setEnabled(false);
}

void QSerialInterfaceWidget::displayError()
{
	if ( !mSerialInterface )
		return;
	QString message( mSerialInterface->getErrorMessage().c_str() );
	onChannelError( message );
}

void QSerialInterfaceWidget::onChannelError( const QString& message )
{
	mStatusBar->showMessage( message, 2000 );
}

void QSerialInterfaceWidget::onGoHomeButtonClicked(bool /*checked*/)
{
	if ( !mSerialInterface )
		return;
	if ( !mSerialInterface->goHomeCP() )
		displayError();
}

void QSerialInterfaceWidget::onAutoRefreshButtonClicked(bool checked)
{
	if ( !mSerialInterface )
		return;

	if ( checked )
		mUpdateTimer->start();
	else
		mUpdateTimer->stop();
}


/*
	QChannelWidget
*/
QChannelWidget::QChannelWidget( QWidget* parent, SerialInterface* serialInterface, unsigned char channelNumber  )
	: QWidget(parent),
	  mSerialInterface(serialInterface),
	  mChannelNumber(channelNumber),
	  mTargetValue( SerialInterface::getMinChannelValue() ),
	  mPositionSpinBox(NULL),
	  mTargetSpinBox(NULL),
	  mTargetSlider(NULL),
	  mSpeedSpinBox(NULL),
	  mAccelerationSpinBox(NULL)
{
	if ( mSerialInterface )
	{
		if ( !mSerialInterface->getPositionCP(mChannelNumber, mTargetValue) )
			displayError();
		if ( !mSerialInterface->setSpeedCP(mChannelNumber, 0) )
			displayError();
		if ( !mSerialInterface->setAccelerationCP(mChannelNumber, 0) )
			displayError();
	}

	createWidgets();
	updateWidgets();
	updateWidgetsFromHardware();
}

void QChannelWidget::createWidgets()
{
	QHBoxLayout* mainLayout = new QHBoxLayout();
	setLayout(mainLayout);
	//mainLayout->setMargin(0);
	QMargins margins = mainLayout->contentsMargins();
	mainLayout->setContentsMargins( margins.left(), margins.top()/3, margins.right(), margins.bottom()/3 );

	mainLayout->addWidget( new QLabel( QString("Channel #%1").arg(mChannelNumber), this) );

	unsigned short minValue = SerialInterface::getMinChannelValue();
	unsigned short maxValue = SerialInterface::getMaxChannelValue();

	mainLayout->addWidget( new QLabel( "Position", this ) );
	mPositionSpinBox = new QSpinBox(this);
	mPositionSpinBox->setKeyboardTracking(false);
	mPositionSpinBox->setSingleStep( (maxValue-minValue) / 50 );
	mPositionSpinBox->setMinimum( minValue );
	mPositionSpinBox->setMaximum( maxValue );
	mPositionSpinBox->setValue( minValue );
	mPositionSpinBox->setEnabled(false);			// The position is read from the hardware and is therefore read-only
	mainLayout->addWidget( mPositionSpinBox );

	mainLayout->addWidget( new QLabel( "Target", this ) );
	mTargetSpinBox = new QSpinBox(this);
	mTargetSpinBox->setKeyboardTracking(false);
	mTargetSpinBox->setSingleStep( (maxValue-minValue) / 50 );
	mTargetSpinBox->setMinimum( minValue );
	mTargetSpinBox->setMaximum( maxValue );
	mTargetSpinBox->setValue( minValue );
	bool ret = connect( mTargetSpinBox, SIGNAL(valueChanged(int)), SLOT(onPositionSpinBoxChanged(int)) );
	assert(ret);
	mainLayout->addWidget( mTargetSpinBox );
		
	mTargetSlider = new QSlider(Qt::Horizontal, this);
	mTargetSlider->setMinimumWidth( 100 );
	mTargetSlider->setMinimum( minValue );
	mTargetSlider->setMaximum( maxValue );
	mTargetSlider->setValue( minValue );
	ret = connect( mTargetSlider, SIGNAL( valueChanged(int) ), SLOT( onPositionSliderChanged(int) ) );
	assert(ret);
		
	mainLayout->addWidget( mTargetSlider );

	mainLayout->addWidget( new QLabel( "Speed", this ) );
	mSpeedSpinBox = new QSpinBox(this);
	mSpeedSpinBox->setKeyboardTracking(false);
	ret = connect( mSpeedSpinBox, SIGNAL(valueChanged(int)), SLOT(onSpeedSpinBoxChanged(int)) );
	assert(ret);
	mainLayout->addWidget( mSpeedSpinBox );

	mainLayout->addWidget( new QLabel( "Acceleration", this ) );
	mAccelerationSpinBox = new QSpinBox(this);
	mAccelerationSpinBox->setKeyboardTracking(false);
	ret = connect( mAccelerationSpinBox, SIGNAL(valueChanged(int)), SLOT(onAccelerationSpinBoxChanged(int)) );
	assert(ret);
	mainLayout->addWidget( mAccelerationSpinBox );

	if ( !mSerialInterface )
		setEnabled(false);
}

void QChannelWidget::displayError()
{
	if ( !mSerialInterface )
		return;
//	QMessageBox::critical( this, "Error", mSerialInterface->getErrorMessage().c_str() );
	QString message( mSerialInterface->getErrorMessage().c_str() );
	emit channelError(message);
}

void QChannelWidget::updateWidgetsFromHardware()
{
	if ( !mSerialInterface )
		return;
	unsigned short position = 0;
	if ( !mSerialInterface->getPositionCP(mChannelNumber, position) )
		displayError();
	else
		mPositionSpinBox->setValue(position);
}

void QChannelWidget::updateWidgets()
{
	if ( mTargetSpinBox->value()!=mTargetValue )
	{
		bool blocked = mTargetSpinBox->blockSignals(true);
		mTargetSpinBox->setValue( mTargetValue );
		mTargetSpinBox->blockSignals(blocked);
	}

	if ( mTargetSlider->value()!=mTargetValue )
	{
		bool blocked = mTargetSlider->blockSignals(true);
		mTargetSlider->setValue( mTargetValue );
		mTargetSlider->blockSignals(blocked);
	}
}

void QChannelWidget::onPositionSpinBoxChanged( int value )
{
	if ( !mSerialInterface )
		return;
	unsigned short targetValue = static_cast<unsigned short>(value);
	if ( !mSerialInterface->setTargetCP( mChannelNumber, targetValue ) )
		displayError();
	else
		mTargetValue = targetValue;

	updateWidgets();
}

void QChannelWidget::onPositionSliderChanged( int value )
{
	if ( !mSerialInterface )
		return;
	unsigned short targetValue = static_cast<unsigned short>(value);
	if ( !mSerialInterface->setTargetCP( mChannelNumber, targetValue ) )
		displayError();
	else
		mTargetValue = targetValue;

	updateWidgets();
}

void QChannelWidget::onSpeedSpinBoxChanged( int value )
{
	if ( !mSerialInterface )
		return;
	if ( !mSerialInterface->setSpeedCP( mChannelNumber, static_cast<unsigned short>(value) ) )
		displayError();
}

void QChannelWidget::onAccelerationSpinBoxChanged( int value )
{
	if ( !mSerialInterface )
		return;
	if ( !mSerialInterface->setAccelerationCP( mChannelNumber, static_cast<unsigned char>(value) ) )
		displayError();
}

}