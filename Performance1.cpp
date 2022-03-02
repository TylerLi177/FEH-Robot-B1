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

DigitalEncoder right_encoder(FEHIO::P1_0);
DigitalEncoder left_encoder(FEHIO::P3_0);
FEHMotor right_motor(FEHMotor::Motor1,9.0);
FEHMotor left_motor(FEHMotor::Motor0,9.0);
AnalogInputPin cds(FEHIO::P0_1);
AnalogInputPin left_opt(FEHIO::P2_7);
AnalogInputPin middle_opt(FEHIO::P2_0);
AnalogInputPin right_opt(FEHIO::P1_4);

void move_forward(int percent, int counts) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(-1 * percent);

    while (left_encoder.Counts() < counts){
        LCD.WriteLine("forward");
        Sleep(0.5);
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
    right_motor.SetPercent(-1 * percent);
    left_motor.SetPercent(percent);

    while (left_encoder.Counts() < counts){
        LCD.WriteLine("backward");
        Sleep(0.5);
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
    Sleep(0.5);
    left_motor.SetPercent(percent);
    right_motor.SetPercent(percent);
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

left_motor.SetPercent(percent);
right_motor.SetPercent(percent);

//While the average of the left and right encoder is less than counts,
//keep running motors

while (((left_encoder.Counts() + right_encoder.Counts()) / 2) < counts) {
    LCD.WriteLine("Right");
    Sleep(0.5);
    left_motor.SetPercent(percent);
    right_motor.SetPercent(percent);
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

int main(void)
{
    //Initialize testing speed
    int testSpeed = 25;
    int ninetyDegreeCount = 265;

    //Check if the starting light is not red
    while (cds.Value() <= 0.3 || cds.Value() >= 0.7) {
        left_motor.Stop();
        right_motor.Stop();
        LCD.WriteLine(cds.Value());
        Sleep(0.5);
    }

    //Move forward
    move_forward(testSpeed, 240);

    Sleep(1.0);

    //Make a slight turn left
    turn_left(testSpeed, 135);

    Sleep(1.0);

    //Move forward
    move_forward(testSpeed, 260);

    //Check if the light is red
    if (cds.Value() > 0.3 && cds.Value() < 0.7){
        left_motor.Stop();
        right_motor.Stop();
        LCD.WriteLine("RED detected");
        turn_left(testSpeed, ninetyDegreeCount); //Make a 90 degree turn 
        Sleep(1.0);
        move_forward(testSpeed, 10); //press the button
        Sleep(1.0);
        move_backward(testSpeed, 50); //move back from the button 
        Sleep(1.0);
        turn_left(testSpeed, ninetyDegreeCount); //make a 90 degree turn left
        Sleep(1.0);
    }

    //Check if light is blue
    else if (cds.Value() > 0.8 && cds.Value() < 1.2){
        left_motor.Stop();
        right_motor.Stop();
        LCD.WriteLine("BLUE detected");
        Sleep(1.0);
        turn_left(testSpeed, ninetyDegreeCount); //make a 90 degree turn left
        Sleep(1.0);
        move_forward(testSpeed, 10); //press the button
        Sleep(1.0);
        move_backward(testSpeed, 50); //move back from the button
        Sleep(1.0);
        turn_left(testSpeed, ninetyDegreeCount); //make a 90 degree turn left
        Sleep(1.0);
    }

    else {
        LCD.WriteLine("RED");
        left_motor.Stop();
        right_motor.Stop();
    } 

    move_forward(testSpeed, 100); //Move close to the ramp

    turn_left(testSpeed, ninetyDegreeCount); //turn left to face the ramp
    
    Sleep(1.0);

    move_forward(testSpeed, 1000); //move up the ramp

    Sleep(1.0);
    
    move_backward(testSpeed, 1000); //move down the ramp

    Sleep(1.0);

    LCD.WriteLine("Hell yeah");

}