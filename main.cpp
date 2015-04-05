#include <Disp.h>
#include <GPIO.h>
#include <IRadio.h>

#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sstream>
#include <string>

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

	GPIO* button1 = new GPIO(17, IN, 0.5);
	GPIO* button2 = new GPIO(27, IN, 0.5);
	GPIO* button3 = new GPIO(22, IN, 0.5);

	Disp* lcd = new Disp{14,15,18,11,23,24,9,25,8,7,10};
	std::string str;

	IRadio* rstream = new IRadio();
	rstream->startStream();

	std::chrono::time_point<std::chrono::system_clock> start, end;
	std::chrono::duration<double> elapsed_time;
	start = std::chrono::system_clock::now();

	while(1)
	{
		led1->setValue( button1->getValue() );
		led2->setValue( button2->getValue() );
		led3->setValue( button3->getValue() );

		end = std::chrono::system_clock::now();
		elapsed_time = end-start;
		if( elapsed_time.count() >= 15.0 )
		{
			start = std::chrono::system_clock::now();
			rstream->getStreamInfos();

			if( lcd->disp_job==lcd->no_job )
			{
				str = rstream->getInterpret();
				lcd->writeText( &str, 1 );

				str = rstream->getTitle();
				lcd->writeText( &str, 2 );
			}
		}

		lcd->process();

		if( !button1->getDebouncedValue() )
			rstream->decreaseStreamNr();
		if( !button2->getDebouncedValue() )
			rstream->stopStream();
		if( !button3->getDebouncedValue() )
			rstream->increaseStreamNr();

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
