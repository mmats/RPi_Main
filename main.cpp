#include <Disp.h>
#include <GPIO.h>
#include <IRadio.h>
#include <Config.h>

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
	std::string line1{}, line2{}, str{};

	IRadio* rstream = new IRadio();
	rstream->startStream();

	Config* c = new Config("RPi_Config");

	std::string welcomeMessageLine1 = c->translate2String("welcomeMessageLine1");
	std::string welcomeMessageLine2 = c->translate2String("welcomeMessageLine2");

	line1 = welcomeMessageLine1;
	line2 = welcomeMessageLine2;

	std::chrono::time_point<std::chrono::system_clock> start_1, start_2;
	std::chrono::duration<double> elapsed_time;
	start_1 = start_2 = std::chrono::system_clock::now();

	/**********************************/
    // int x  = c->translate2Int("x");
    // double x = c->translate2Double("x");
	// std::string x  = c->translate2String("x");
	/**********************************/

	while(1)
	{
		led1->setValue( button1->getValue() );
		led2->setValue( button2->getValue() );
		led3->setValue( button3->getValue() );

		elapsed_time = std::chrono::system_clock::now() - start_1;
		if( elapsed_time.count() >= 20.0 )
		{
			start_1 = std::chrono::system_clock::now();

			if( !rstream->streamIsRunning() )
			{
				line1 = welcomeMessageLine1;
				line2 = welcomeMessageLine2;
			}
			else
			{
				rstream->getStreamInfos();
				if( rstream->streamHasChanged() )
				{
					line1 = "[" + std::to_string(rstream->getStreamNr()) + "] " + rstream->getStreamName() + "   ";
					line2 = rstream->getInterpret() + " - " + rstream->getTitle()  + "   ";

					//std::cout << "[" << rstream->getStreamNr() << "] " <<  rstream->getStreamName() << ", " << rstream->getInterpret() << " - " << rstream->getTitle() << "\n";
				}
			}
		}

		elapsed_time = std::chrono::system_clock::now() - start_2;
		if( elapsed_time.count() >= 1.0 )
		{
			start_2 = std::chrono::system_clock::now();
			if( lcd->disp_job==lcd->no_job )
			{
				if( !line1.empty() )
				{
					if( line1.size()<DISP_LINE_LENGTH+3 )
					{
						str = line1.substr(0,DISP_LINE_LENGTH);
						lcd->writeText( &str, 1 );
					}
					else
					{
						str = line1.substr(0,DISP_LINE_LENGTH);
						line1 = line1.substr(1) + line1.at(0);
						lcd->writeText( &str, 1 );
					}
				}
				if( !line2.empty() )
				{
					if( line2.size()<DISP_LINE_LENGTH+3 )
					{
						str = line2.substr(0,DISP_LINE_LENGTH);
						lcd->writeText( &str, 2 );
					}
					else
					{
						str = line2.substr(0,DISP_LINE_LENGTH);
						line2 = line2.substr(1) + line2.at(0);
						lcd->writeText( &str, 2 );
					}
				}
			}
		}

		lcd->process();

		if( !button1->getDebouncedValue() )
			rstream->increaseStreamNr();
		if( !button2->getDebouncedValue() )
			rstream->stopOrResumeStream();
		if( !button3->getDebouncedValue() )
			rstream->decreaseStreamNr();

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
