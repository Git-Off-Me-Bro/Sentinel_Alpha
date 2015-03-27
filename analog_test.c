#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include <pcf8591.h>
#include <time.h>
#include <string.h>

#define FOUR_FEET 46
#define MAX_VAL 255
#define SUPPLY_V 3.3
#define SCALE_FACTOR SUPPLY_V/2.55
#define F_SENSOR analogRead(75 + 0)
#define R_SENSOR analogRead(75 + 1)
#define POLL_B_PIN 4
#define POLL_F_PIN 3
#define SIZE 256
#define ON 1
#define OFF 0
#define REPORT_NUM(X) "/home/pi/sentinel/reports/new/reportX.csv"

float FRONT_DIST;
float REAR_DIST;
FILE *report;
int count = 0;

//TODO: Use these for Bluetooth transfer in the future
//int buffer[10] = 0;
//int ptr* = buffer[1];

time_t curtime;
char timebuff[SIZE];
struct tm *loctime;

int main()
{
	// Init
	printf("Starting Sentinel\r\n");
	wiringPiSetup();
	pcf8591Setup(75, 0x48);
	pinMode(POLL_F_PIN, OUTPUT);
	pinMode(POLL_B_PIN, OUTPUT);
	digitalWrite(POLL_B_PIN, ON);
	digitalWrite(POLL_F_PIN, ON);

	for(;;)
	{
	// Allow reading from front sensor	
	digitalWrite(POLL_F_PIN, ON);
	usleep(1000);
	// Read from front sensor
	FRONT_DIST = F_SENSOR * SCALE_FACTOR;
	// Disallow reading
	digitalWrite(POLL_F_PIN,OFF);

	// Allow reading from back sensor
	digitalWrite(POLL_B_PIN, ON);
	usleep(1000);
	// Read from back sensor
	REAR_DIST = R_SENSOR * SCALE_FACTOR;
	// Disallow reading
	digitalWrite(POLL_B_PIN, OFF);
	
	//NOTE: Delay for debugging remove later
	usleep(2000);
	

	//NOTE: Term output for debug
//		if (REAR_DIST != MAX_VAL)
//		{
//			printf("‌rear=%4.3f ‌front=%4.3f", REAR_DIST, FRONT_DIST);
//			fflush(stdout);
//			for(int i =0; i<26; i++)
//			{
//				printf("\b");
//			}

		// Build report
		if((REAR_DIST < FOUR_FEET) && (REAR_DIST != MAX_VAL))
		{   
			//TODO: Send this data to phone
			
			// Setup and write time to file
 			curtime = time (NULL);
 			loctime = localtime (&curtime);
			fputs(asctime (loctime), stdout);

 			// pre-increment event counter
			count++;			
			report = fopen(REPORT_NUM(count), "w");
			if(report != 0)
			{
				fprintf(report,"rear,%f,front,%f", REAR_DIST, FRONT_DIST);
			}
			fclose(report);
			
			// Snap Picture of event
			system("/bin/get_image");

			//TODO: Add system call for bluetooth data transfer
		}
	}
}
