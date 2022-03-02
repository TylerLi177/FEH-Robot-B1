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
#define testSpeed 25
#define ninetyDegreeCount 135
#define PI 3.14159265358979323846

DigitalEncoder right_encoder(FEHIO::P1_0);
DigitalEncoder left_encoder(FEHIO::P3_0);
FEHMotor right_motor(FEHMotor::Motor1,9.0);
FEHMotor left_motor(FEHMotor::Motor0,9.0);
AnalogInputPin cds(FEHIO::P0_1);
AnalogInputPin left_opt(FEHIO::P2_7);
AnalogInputPin middle_opt(FEHIO::P2_0);
AnalogInputPin right_opt(FEHIO::P1_4);
FEHMotor bucket_arm(FEHMotor::Motor2,9.0);

void move_forward(int percent, int counts) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(-1 * percent);
    left_motor.SetPercent(percent);

    while (left_encoder.Counts() < counts){
        LCD.WriteLine("forward");
        Sleep(1.0);
    }

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void move_backward(int percent, int counts) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(-1 * percent);

    while (left_encoder.Counts() < counts){
        LCD.WriteLine("backward");
        Sleep(1.0);
    }

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);
    
    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void turn_left(int percent, int counts) //using encoders
{

//Reset encoder counts

right_encoder.ResetCounts();

left_encoder.ResetCounts();

//Set both motors to desired percent

left_motor.SetPercent(percent);
right_motor.SetPercent(percent);

//While the average of the left and right encoder is less than counts,

//keep running motors

while (((left_encoder.Counts() + right_encoder.Counts()) / 2) < counts) {
    LCD.WriteLine("Left");
    Sleep(1.0);
}

//Turn off motors

right_motor.Stop();

left_motor.Stop();

}

void turn_right(int percent, int counts) //using encoders
{

//Reset encoder counts

right_encoder.ResetCounts();

left_encoder.ResetCounts();

//Set both motors to desired percent

//hint: set right motor backwards, left motor forwards

left_motor.SetPercent(-1 * percent);
right_motor.SetPercent(-1 * percent);

//While the average of the left and right encoder is less than counts,
//keep running motors

while (((left_encoder.Counts() + right_encoder.Counts()) / 2) < counts) {
    LCD.WriteLine("Right");
    Sleep(1.0);
}

//Turn off motors

right_motor.Stop();

left_motor.Stop();

}

void lineTracking(float fast_motor_percent, float slow_motor_percent){

    left_motor.SetPercent(-1 * fast_motor_percent);
    right_motor.SetPercent(fast_motor_percent);
    LCD.Write("Sexy\n");

    //See if the left and middle sensor are off the track
    while (left_opt.Value() <= 0.8 && middle_opt.Value() <= 0.3) {

        //Set the right wheel to go slower
        left_motor.SetPercent(-1 * fast_motor_percent);
        right_motor.SetPercent(slow_motor_percent);

        //Set the right wheel to go slower
        LCD.Write("Going Right\n");

        //Stop moving right if the middle sensor is over the track
        if (middle_opt.Value() >= 1.5) {
            break;
        }
    }

    //See if the middle and right sensor are off the track
    while (middle_opt.Value() <= 0.3 && right_opt.Value() <= 1.2) {
        left_motor.SetPercent(-1 * slow_motor_percent);
        right_motor.SetPercent(fast_motor_percent);

        LCD.Write("Going Left\n");

        //Stop moving right if the middle sensor is over the track
        if (middle_opt.Value() >= 1.5) {
            break;
        }
    }
}

void move_bucket_arm(int percent, float seconds){

    //Set desired motor percentage
    bucket_arm.SetPercent(percent);

    float time = TimeNow();

    //move thr arm for a certain amoutn of time
    while(time - TimeNow() < seconds){
    if(percent > 0){
        LCD.WriteLine("Dropping bucket");
        Sleep(0.5);
    }
    else {
        LCD.WriteLine("Raising bucket");
        Sleep(0.5);
    }
    
    //Stop the arm
    bucket_arm.Stop();
    }
}

int main(void)
{
    LCD.WriteLine("TESTING TESTING 1 2 3");
    //Check if the starting light is not red
    while (cds.Value() <= 0.3 || cds.Value() >= 0.7) {
        left_motor.Stop();
        right_motor.Stop();
        LCD.WriteLine(cds.Value());
        Sleep(1.0);
    }

    //Move to trash can
    move_forward(testSpeed, 250); //move forward from starting light
    Sleep(1.0);
    turn_left(testSpeed, 60); //make a slight turn
    Sleep(1.0);
    move_forward(testSpeed, 350); //move forward to be level with trash can
    Sleep(1.0);
    turn_right(testSpeed, ninetyDegreeCount); //make a 90 degree turn right
    Sleep(1.0);
    move_forward(testSpeed, 400); //move forward to trash can

    //Dump tray into trash can
    move_bucket_arm(testSpeed, 2); //move arm down
    Sleep(1.0);
    move_bucket_arm(-1 * testSpeed, 2); //retract arm back up

    // //Move up the ramp
    // move_backward(testSpeed, 100); //move back from trash can
    // Sleep(1.0);
    // turn_right(testSpeed, ninetyDegreeCount); //turn 90 dgerees to move to the ramp
    // Sleep(1.0);
    // move_forward(testSpeed, 200); //move closer to the ramp
    // Sleep(1.0);
    // turn_left(testSpeed, ninetyDegreeCount); //turn to face the ramp
    // Sleep(1.0);
    // move_forward(testSpeed, 500); //move up the ramp
    
    LCD.WriteLine("Hell yeah");

}
