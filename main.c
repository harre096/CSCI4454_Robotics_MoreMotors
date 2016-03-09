//*****************************************************************************
//
// MSP432 main.c - Dolan Clock - Myles Gavic, Thomas Harren, & Brian Mitchell
//
//****************************************************************************

#include "msp.h"

void selectPortFunction(int port, int line, int sel10, int sel1){
	//(p,l,0,0) will set port to Digital I/O
	if(port==1){
		if(P1SEL0 & BIT(line)!=sel10){
			if(P1SEL1 & BIT(line)!=sel1){
				P1SELC|=BIT(line);
			}else{
				P1SEL0^=BIT(line);
			}
		}else{
			if(P1SEL1 & BIT(line)!=sel1)
				P1SEL1^=BIT(line);
		}
	}else if (port==2){
		if(P2SEL0 & BIT(line)!=sel10){
			if(P2SEL1 & BIT(line)!=sel1){
				P2SELC|=BIT(line);
			}else{
				P2SEL0^=BIT(line);
			}
		}else{
			if(P2SEL1 & BIT(line)!=sel1)
				P2SEL1^=BIT(line);
		}
	}else if (port==4){
		if(P4SEL0 & BIT(line)!=sel10){
			if(P4SEL1 & BIT(line)!=sel1){
				P4SELC|=BIT(line);
			}else{
				P4SEL0^=BIT(line);
			}
		}else{
			if(P4SEL1 & BIT(line)!=sel1)
				P4SEL1^=BIT(line);
		}
	}
}


void setClockFrequency(void){
	CSKEY=0x695A;       //unlock
	CSCTL1=0x00000223;  //run at 128, enable stuff for clock
	CSCLKEN=0x0000800F;
	CSKEY=0xA596;       //lock
}

void configureTimer(void){
	TA0CTL=0x0100;   //Picks clock (above), count up mode, sets internal divider, shuts timer off.

	TA0CCTL0=0x2000; //Pick compare (not capture) register, interrupt off
	TA0CCR0=0x0080;  //(128)//sets the max time compare register (1,2,3 depend on this peak)
	//interrups every milisecond

	TA0CCTL1=0x2010; //Pick compare (not capture) register, interrupt on
	TA0CCR1=0x0010;   //(32) //sets the max time compare for this capture, will wait until overflow (will be overwritten)

	TA0CTL=0x0116;   //Sets counter to 0, turns counter on, enables timeout (aka overflow) interrups
}

void TimerA0Interrupt(void) {
	unsigned short intv=TA0IV;
	if(intv==0x0E) {// set bit low (ON) at OE (overflow interrupt)
		P2OUT&=~(BIT7);
	}
	if(intv==0x02) {// set bit high (OFF) at 02 interrupt
		P2OUT|=BIT7;
	}
}


void main(void){

	WDTCTL = WDTPW | WDTHOLD; //Stop watchdog timer
	configureTimer();
	setClockFrequency();
	//init port to motor
	P2DIR|=BIT7;  //1 aka output
	selectPortFunction(2,7,0,0);

	NVIC_EnableIRQ(TA0_N_IRQn); //Enable TA0 interrupts using the NVIC

	while(1){}
}
