# avr-experiments

This repository contains random code I am writing for Arduino / AVR programs.

I am intested in robotics and other electronics things and enjoy coding for 
Arduino and ATmega328 in C and Assembly.  I tend to not use the Arduino IDE
but prefer to code in `vi` and use a GNU Makefile to run the avr toolchain
commands needed to compile and upload the code.  Here are some of the 
experiments I have:

 * Blink - the basic LED blink program
 * HardDriveMotor - code for running a Brushless DC Motor salvaged from an old Hard Drive
 * clock-testing - Simple code for generating signals on a pin from the arduino. My goal
   was to simply turn a pin on and off every clock cycle and hook the pin up to my 
   oscilloscope and see the clock frequency that the Arduino was running at.
 * WiiNunchuck - code to interface an Arduino to a Nunchuck controller from a Nintendo Wii

