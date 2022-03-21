#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHUtility.h>
#include <FEHMotor.h>
#include <FEHServo.h>
#include <FEHAccel.h>
#include <FEHBattery.h>
#include <FEHBuzzer.h>
#include <FEHRPS.h>
#include <FEHSD.h>
#include <string.h>
#include <stdio.h>

// RPS Delay time
#define RPS_WAIT_TIME_IN_SEC 0.35

// Shaft encoding counts for CrayolaBots
#define COUNTS_PER_INCH 40.5
#define COUNTS_PER_DEGREE 2.48

// Defines for pulsing the robot
#define PULSE_TIME 0.1
#define PULSE_POWER 30

// Define for the motor power
#define POWER 25

// Orientation of QR Code
#define PLUS 0
#define MINUS 1

//Declarations for encoders & motors
DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P0_1);
FEHMotor right_motor(FEHMotor::Motor1, 9.0);
FEHMotor left_motor(FEHMotor::Motor0, 9.0);

/*
 * Pulse forward a short distance using time
 */
void pulse_forward(int percent, float seconds) 
{
    // Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    // Wait for the correct number of seconds
    Sleep(seconds);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

/*
 * Pulse counterclockwise a short distance using time
 */
void pulse_counterclockwise(int percent, float seconds) 
{
    // Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(-percent);

    // Wait for the correct number of seconds
    Sleep(seconds);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

/*
 * Move forward using shaft encoders where percent is the motor percent and counts is the distance to travel
 */
void move_forward(int percent, int counts) //using encoders
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    // While the average of the left and right encoder are less than counts,
    // keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

/*
 * Turn counterclockwise using shaft encoders where percent is the motor percent and counts is the distance to travel
 */
void turn_counterclockwise(int percent, int counts) 
{
    // Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    // Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(-percent);

    // While the average of the left and right encoder are less than counts,
    // keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    // Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

/* 
 * Use RPS to move to the desired x_coordinate based on the orientation of the QR code
 */
void check_x(float x_coordinate, int orientation)
{
    // Determine the direction of the motors based on the orientation of the QR code 
    int power = PULSE_POWER;
    if(orientation == MINUS){
        power = -PULSE_POWER;
    }

    // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
    while((RPS.X() >= 0) && (RPS.X() < x_coordinate - 0.5 || RPS.X() > x_coordinate + 0.5))
    {
        LCD.WriteLine("X: ");
        LCD.WriteLine(RPS.X());
        if(RPS.X() < x_coordinate)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_forward(power, PULSE_TIME);
        }
        else
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_forward(power, PULSE_TIME);
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}

/* 
 * Use RPS to move to the desired y_coordinate based on the orientation of the QR code
 */
void check_y(float y_coordinate, int orientation)
{
    // Determine the direction of the motors based on the orientation of the QR code
    int power = PULSE_POWER;
    if(orientation == MINUS){
        power = -PULSE_POWER;
    }

    // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
    while((RPS.Y() >= 0) && (RPS.Y() < y_coordinate - 0.5 || RPS.Y() > y_coordinate + 0.5))
    {
        LCD.WriteLine("Y: ");
        LCD.WriteLine(RPS.Y());
        if(RPS.Y() < y_coordinate)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_forward(power, PULSE_TIME);
        }
        else
        {
            // Pulse the motors for a short duration in the correct direction
           pulse_forward(power, PULSE_TIME);
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}

/* 
 * Use RPS to move to the desired heading
 */
void check_heading(float heading)
{
    //You will need to fill out this one yourself and take into account
    //checking for proper RPS data and the edge conditions
    //(when you want the robot to go to 0 degrees or close to 0 degrees)

    /*
        SUGGESTED ALGORITHM:
        1. Check the current orientation of the QR code and the desired orientation of the QR code
        2. Check if the robot is within the desired threshold for the heading based on the orientation
        3. Pulse in the correct direction based on the orientation
    */
    // Determine the direction of the motors based on the orientation of the QR code 
    int power = PULSE_POWER;
    if(heading == MINUS){
        power = -PULSE_POWER;
    }

    // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
    while((RPS.Heading() >= 0) && (RPS.Heading() < heading - 2 || RPS.Heading() > heading + 2))
    {
        LCD.WriteLine("Heading: ");
        LCD.WriteLine(RPS.Heading());
        if(RPS.Heading() < heading)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_counterclockwise(power, PULSE_TIME);
        }
        else
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_counterclockwise(power, PULSE_TIME);
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }

}

int main(void)
{
    float touch_x,touch_y;
    float A_x, A_y, B_x, B_y, C_x, C_y, D_x, D_y;
    float A_heading, B_heading, C_heading, D_heading;
    int B_C_counts, C_D_counts, turn_90_counts;

    RPS.InitializeTouchMenu();

    LCD.WriteLine("RPS & Data Logging Test");
    LCD.WriteLine("Press Screen To Start");
    while(!LCD.Touch(&touch_x,&touch_y));
    while(LCD.Touch(&touch_x,&touch_y));

    // COMPLETE CODE HERE TO READ SD CARD FOR LOGGED X AND Y DATA POINTS
    FEHFile *fptr = SD.FOpen("RPS_TEST.txt", "r");
    SD.FScanf(fptr, "%f%f", &A_x, &A_y);
    SD.FScanf(fptr, "%f%f", &B_x, &B_y);
    SD.FScanf(fptr, "%f%f", &C_x, &C_y);
    SD.FScanf(fptr, "%f%f", &D_x, &D_y);

    FEHFile *ofptr = SD.FOpen("Output.txt", "w");
    SD.FPrintf(ofptr, "X: %f, Y: %f", &A_x, &A_y);
    SD.FPrintf(ofptr, "X: %f, Y: %f", &B_x, &B_y);
    SD.FPrintf(ofptr, "X: %f, Y: %f", &C_x, &C_y);
    SD.FPrintf(ofptr, "X: %f, Y: %f", &D_x, &D_y);
    SD.FClose(ofptr);
    SD.FClose(fptr);

    // WRITE CODE HERE TO SET THE HEADING DEGREES AND COUNTS VALUES
    A_heading = 90.0;
    B_heading = 180.0;
    C_heading = 270.0;
    D_heading = 0.0;

    B_C_counts = 648;
    C_D_counts = 405;

    turn_90_counts = 240;

    // A --> B
    check_y(B_y, PLUS);
    check_heading(B_heading);

    // B --> C
    move_forward(POWER, B_C_counts);
    check_x(C_x, MINUS);
    turn_counterclockwise(POWER, turn_90_counts);
    check_heading(C_heading);

    // C --> D
    move_forward(POWER, C_D_counts);
    check_y(D_y, MINUS);
    turn_counterclockwise(POWER, turn_90_counts);
    check_heading(D_heading);

    return 0;
}
