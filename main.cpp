#include <Disp.h>
#include <GPIO.h>
#include <IRadio.h>

#include <iostream>
#include <unistd.h>
#include <signal.h>

// FUNCTION PROTOTYPING
void msleep( unsigned milisec );
void sig_handler( int sig );

// GLOBAL VARIABLES
bool ctrl_c_pressed = false;

int main()
{
	if( signal(SIGINT, sig_handler) == SIG_ERR )
		std::cout << "Can't catch SIGINT" << std::endl;

	GPIO* led1 = new GPIO(4, OUT);
	GPIO* led2 = new GPIO(3, OUT);
	GPIO* led3 = new GPIO(2, OUT);
	GPIO* button1 = new GPIO(17, IN);
	GPIO* button2 = new GPIO(27, IN);
	GPIO* button3 = new GPIO(22, IN);
	Disp* lcd = new Disp{14,15,18,11,23,24,9,25,8,7,10};
	IRadio* rstream = new IRadio();

	unsigned char textLine1[] = "0123456789ABCDEF";

	rstream->startStream();

	while(1)
	{
		led1->setValue( button1->getValue() );
		if( !button1->getValue() )
			rstream->decreaseStreamNr();

		led2->setValue( button2->getValue() );
		if( !button2->getValue() )
			rstream->stopStream();

		led3->setValue( button3->getValue() );
		if( !button3->getValue() )
			rstream->increaseStreamNr();

		lcd->process();
		if( lcd->disp_job==lcd->no_job )
		{
			lcd->writeText( textLine1, 1 );
			lcd->writeText( textLine1, 2 );
		}

		if( ctrl_c_pressed )
			break;
	}

	delete lcd;
	delete led1; delete led2; delete led3;
	delete button1; delete button2; delete button3;
	delete rstream;
}

void msleep( unsigned milisec )
{
	for(unsigned i=0; i<milisec; ++i)
		usleep( 1000 );
}

void sig_handler( int sig )
{
	if (sig == SIGINT)
		ctrl_c_pressed = true;
}
