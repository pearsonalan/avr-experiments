/* Nunchuck.cpp
 *
 * Based on the code originally posted by Chad Phillips at http://www.windmeadow.com/node/42
 * Sadly, his site is no longer up, but I found the original, often cited blog post thanks
 * to the Wayback Machine at https://web.archive.org/web/20140325144624/http://www.windmeadow.com/node/42
 */

#include <Arduino.h>
#include <string.h>
#include <stdio.h>

#include "Wire.h"

/* array to store arduino output */
uint8_t outbuf[6];
int cnt = 0;
int ledPin = 13;

void nunchuck_init();
char nunchuk_decode_byte(char x);

void setup()
{
	Serial.begin(19200);
	
	/* Initialize TWI and send the initilization handshake to the Nunchuck */
	Wire.begin();
	nunchuck_init();					

	Serial.print("Finished setup\n");
}

void nunchuck_init()
{
	/* transmit to device 0x52 */
	Wire.beginTransmission(0x52);		

	/* sends memory address */
	Wire.write(0x40);					

	/* sends sent a zero.   */
	Wire.write(0x00);					

	/* stop transmitting */
	Wire.endTransmission();				
}

void send_zero()
{
	/* transmit to device 0x52 */
	Wire.beginTransmission(0x52);		

	/* sends one byte */
	Wire.write(0x00);					

	/* stop transmitting */
	Wire.endTransmission();				
}

/* print the input data we have recieved
 * accel data is 10 bits long so we read 8 bits, then we have to add
 * on the last 2 bits.  That is why I multiply them by 2 * 2 
 */
void print()
{
	int joy_x_axis = outbuf[0];
	int joy_y_axis = outbuf[1];
	int accel_x_axis = outbuf[2] * 2 * 2; 
	int accel_y_axis = outbuf[3] * 2 * 2;
	int accel_z_axis = outbuf[4] * 2 * 2;

	int z_button = 0;
	int c_button = 0;

	/* byte outbuf[5] contains bits for z and c buttons
	 * it also contains the least significant bits for the accelerometer data
	 * so we have to check each bit of byte outbuf[5]
	 */
	if ((outbuf[5] >> 0) & 1)
	{
		z_button = 1;
	}
	if ((outbuf[5] >> 1) & 1)
	{
		c_button = 1;
	}

	if ((outbuf[5] >> 2) & 1)
	{
		accel_x_axis += 2;
	}
	if ((outbuf[5] >> 3) & 1)
	{
		accel_x_axis += 1;
	}

	if ((outbuf[5] >> 4) & 1)
	{
		accel_y_axis += 2;
	}
	if ((outbuf[5] >> 5) & 1)
	{
		accel_y_axis += 1;
	}

	if ((outbuf[5] >> 6) & 1)
	{
		accel_z_axis += 2;
	}
	if ((outbuf[5] >> 7) & 1)
	{
		accel_z_axis += 1;
	}

	Serial.print(joy_x_axis, DEC);
	Serial.print("\t");

	Serial.print(joy_y_axis, DEC);
	Serial.print("\t");

	Serial.print(accel_x_axis, DEC);
	Serial.print("\t");

	Serial.print(accel_y_axis, DEC);
	Serial.print("\t");

	Serial.print(accel_z_axis, DEC);
	Serial.print("\t");

	Serial.print(z_button, DEC);
	Serial.print("\t");

	Serial.print(c_button, DEC);
	Serial.print("\t");

	Serial.print("\r\n");
}

/* Encode data to format that most wiimote drivers except
 * only needed if you use one of the regular wiimote drivers
 */
char nunchuk_decode_byte(char x)
{
	x = (x ^ 0x17) + 0x17;
	return x;
}


void loop()
{
	/* request data from nunchuck */
	Wire.requestFrom(0x52, 6);			

	/* read the bytes from the controller and decode them */
	while (Wire.available())
	{
		/* set the LED on */
		digitalWrite(ledPin, HIGH);		

		/* receive byte as an integer */
		outbuf[cnt] = nunchuk_decode_byte(Wire.read());

		/* set the LED off */
		digitalWrite(ledPin, LOW);		

		cnt++;
	}

	/* If we recieved the 6 bytes, then go print them */
	if (cnt >= 5)
	{
		print();
	}

	cnt = 0;

	/* send the request for next bytes */
	send_zero();						

	delay(100);
}


