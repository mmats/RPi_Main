#include <Disp.h>
#include <GPIO.h>

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
	Disp* pvc160101 = new Disp();

	while(1)
	{
		led1->setValue( button1->getValue() );
		led2->setValue( button2->getValue() );
		led3->setValue( button3->getValue() );

		msleep(300);

		if( ctrl_c_pressed )
			break;
	}

	delete pvc160101;
	delete led1; delete led2; delete led3;
	delete button1; delete button2; delete button3;
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
