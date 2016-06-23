/*
 * Arduino IRremote Daikin 2015
 * Copyright 2015 danny
 *
 *
 * enableIROut declare base on  Ken Shirriff's IRremote library.
 * http://arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
 *
 *
 */

#include "IRremoteDaikinSend.h"
#include "IRremoteIntDaikin.h"

// Provides ISR
#include <avr/interrupt.h>


int IRpin;
int FreqK;

void delayMicrosecondsPrecision(uint32_t us)
{
	if(us > 100)
	{
		uint32_t t0,tn;

		t0 = micros();
		do
		{
			tn=micros();
		}while(tn >= t0 && tn < (t0 + us - 1));
	}
	else
	{
		if(us > 1)
			delayMicroseconds(us-1);
	}
}

void IRDaikinSend::sendDaikin(unsigned char buf[], int len, int start) {
	int data2;
	enableIROut(38);
	mark(DAIKIN_HDR_MARK);
	space(DAIKIN_HDR_SPACE);

	for (int i = start; i < start+len; i++) {
		data2=buf[i];

		for (int j = 0; j < 8; j++) {
			if ((1 << j & data2)) {
				mark(DAIKIN_ONE_MARK);
				space(DAIKIN_ONE_SPACE);
			}
			else {
				mark(DAIKIN_ZERO_MARK);
				space(DAIKIN_ZERO_SPACE);
			}
		}

	}
	mark(DAIKIN_ONE_MARK);
	space(DAIKIN_ZERO_SPACE);
}

void IRDaikinSend::sendDaikinWake() {
  enableIROut(38);
  space(DAIKIN_ZERO_MARK);
  //
  mark(DAIKIN_ZERO_MARK);
  space(DAIKIN_ZERO_MARK);
  //
  mark(DAIKIN_ZERO_MARK);
  space(DAIKIN_ZERO_MARK);
  //
  mark(DAIKIN_ZERO_MARK);
  space(DAIKIN_ZERO_MARK);
  //
  mark(DAIKIN_ZERO_MARK);
  space(DAIKIN_ZERO_MARK);
  //
  mark(DAIKIN_ZERO_MARK);
  space(DAIKIN_ZERO_MARK);
}

void IRDaikinSend::sendRaw(unsigned int buf[], int len, int hz)
{
  enableIROut(hz);
  for (int i = 0; i < len; i++) {
    if (i & 1) {
      space(buf[i]);
    }
    else {
      mark(buf[i]);
    }
  }
  space(0); // Just to be sure
}

void IRDaikinSend::setPin(int pin) {
  IRpin = pin;	
  pinMode(IRpin, OUTPUT);
  digitalWrite(IRpin, LOW); // When not sending PWM, we want it low  
}

//~ #if defined(SIMULATE)

//~ void IRDaikinSend::mark(int time) {
  //~ int cycleTime = time/25;
  //~ for (int i = 0; i < cycleTime; ++i)
  //~ {
    //~ digitalWrite(IRpin,HIGH);
    //~ delayMicroseconds(11);
    //~ digitalWrite(IRpin,LOW);
    //~ delayMicroseconds(5);
  //~ }
//~ }

//~ void IRDaikinSend::space(int time) {
  //~ digitalWrite(IRpin,LOW);
  //~ delayMicroseconds(time);
//~ }
//~ #else
void IRDaikinSend::mark(int time) {
  // Sends an IR mark for the specified number of microseconds.
  // The mark output is modulated at the PWM frequency.
//  TIMER_ENABLE_PWM; // Enable pin 3 PWM output
//  delayMicroseconds(time);

	uint32_t t0,tn;
	
	t0 = micros();
	do
	{
	  	digitalWrite(IRpin,HIGH);
		delayMicrosecondsPrecision(8);		
		digitalWrite(IRpin,LOW);
		delayMicrosecondsPrecision(18-1);	// micros() function will take 1us
		
		tn=micros();
	}while(tn >= t0 && tn < (t0 + time));
}

/* Leave pin off for time (given in microseconds) */
void IRDaikinSend::space(int time) {
  // Sends an IR space for the specified number of microseconds.
  // A space is no output, so the PWM output is disabled.
//  TIMER_DISABLE_PWM; // Disable pin 3 PWM output
  delayMicrosecondsPrecision(time);
}

//~ #endif


void IRDaikinSend::enableIROut(int khz) {
  // Enables IR output.  The khz value controls the modulation frequency in kilohertz.
  // The IR output will be on pin 3 (OC2B).
  // This routine is designed for 36-40KHz; if you use it for other values, it's up to you
  // to make sure it gives reasonable results.  (Watch out for overflow / underflow / rounding.)
  // TIMER2 is used in phase-correct PWM mode, with OCR2A controlling the frequency and OCR2B
  // controlling the duty cycle.
  // There is no prescaling, so the output frequency is 16MHz / (2 * OCR2A)
  // To turn the output on and off, we leave the PWM running, but connect and disconnect the output pin.
  // A few hours staring at the ATmega documentation and this will all make sense.
  // See my Secrets of Arduino PWM at http://arcfn.com/2009/07/secrets-of-arduino-pwm.html for details.


  // Disable the Timer2 Interrupt (which is used for receiving IR)
  //TIMER_DISABLE_INTR; //Timer2 Overflow Interrupt

//  pinMode(TIMER_PWM_PIN, OUTPUT);
  //digitalWrite(TIMER_PWM_PIN, HIGH); // When not sending PWM, we want it low
 //


  // COM2A = 00: disconnect OC2A
  // COM2B = 00: disconnect OC2B; to send signal set to 10: OC2B non-inverted
  // WGM2 = 101: phase-correct PWM with OCRA as top
  // CS2 = 000: no prescaling
  // The top value for the timer.  The modulation frequency will be SYSCLOCK / 2 / OCR2A.
//  TIMER_CONFIG_KHZ(khz);
	FreqK = khz;
}
