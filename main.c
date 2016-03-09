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

void initPorts(void){
	P2DIR|=(BIT4|BIT5|BIT6|BIT7);  //1 aka output
	selectPortFunction(2,4,0,0); //red (wire)
	selectPortFunction(2,5,0,0); //red&white
	selectPortFunction(2,6,0,0); //green&white
	selectPortFunction(2,7,0,0); //green
}

void configureTimer(void){
	TA0CTL=0x0100;   //Picks clock (above), count up mode, sets internal divider, shuts timer off.

	TA0CCTL0=0x2000; //Pick compare (not capture) register, interrupt off

	//shorter overflow period => faster motor speed
	//0x1000 is optimal speed for music box :)
	TA0CCR0=0x1000;  //sets the max time compare register (1,2,3 depend on this peak)

	TA0CTL=0x0116;   //Sets counter to 0, turns counter on, enables timeout (aka overflow) interrups
}

void TimerA0Interrupt(void) {
	static int cycle = 0;
	unsigned short intv=TA0IV;
	if(intv==0x0E) {// set bit low (ON) at OE (overflow interrupt)
		P2OUT&=0x0F; // re-setting register to 0000 xxxx
		switch(cycle){
		case 0:
			P2OUT|=0xE0; // setting register to 1110 xxxx
			break;
		case 1:
			P2OUT|=0x60; //setting register to 0110
			break;
		case 2:
			P2OUT|=0x70; //setting register to 0111
			break;
		case 3:
			P2OUT|=0x50; //setting register to 0101
			break;
		case 4:
			P2OUT|=0xD0; //setting register to 1101
			break;
		case 5:
			P2OUT|=0x90; //setting register to 1001
			break;
		case 6:
			P2OUT|=0xB0; //setting register to 1011
			break;
		case 7:
			P2OUT|=0xA0; //setting register to 1010
			break;
		}
		//motor forward
//		if(++cycle == 8){
//			cycle = 0;
//		}
		//motor backward
		if(cycle == 0){
			cycle = 7;
		} else {
			cycle--;
		}
	}
}


void main(void){

	WDTCTL = WDTPW | WDTHOLD; //Stop watchdog timer
	configureTimer();
	setClockFrequency();
	initPorts();


	NVIC_EnableIRQ(TA0_N_IRQn); //Enable TA0 interrupts using the NVIC

	while(1){}
}
