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

DigitalEncoder right_encoder(FEHIO::P0_0);
DigitalEncoder left_encoder(FEHIO::P0_1);
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

    //While the average of the left and right encoder is less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

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
    left_motor.SetPercent(percent);
    right_motor.SetPercent(percent);
}

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
    while (left_opt.Value() <= 1.2 && middle_opt.Value() <= 0.85) {

        //Set the right wheel to go slower
        left_motor.SetPercent(-1 * fast_motor_percent);
        right_motor.SetPercent(slow_motor_percent);

        //Set the right wheel to go slower
        LCD.Write("Going Right\n");

        //Stop moving right if the middle sensor is over the track
        if (middle_opt.Value() >= 2) {
            break;
        }
    }

    //See if the middle and right sensor are off the track
    while (middle_opt.Value() <= 0.85 && right_opt.Value() <= 1.1) {
        left_motor.SetPercent(-1 * slow_motor_percent);
        right_motor.SetPercent(fast_motor_percent);

        LCD.Write("Going Left\n");

        //Stop moving right if the middle sensor is over the track
        if (middle_opt.Value() >= 2) {
            break;
        }
    }
}

int main(void)
{

    // if (cds.Value() > 0.25 && cds.Value() < 0.5){

    //     //Move forward 7 inches
    //     move_forward(25, 238.185);

    //     //Make a slight turn left
    //     turn_left(25, 120);

    //     //Move forward 12 inches
    //     move_forward(25, 404.89);

    //     //Check if light is red
    //     if (cds.Value() > 0.25 && cds.Value() < 0.5) {
    //         move_forward(25, 134.96);
    //         turn_left(25, 200);
    //         lineTracking(15, 6.9);
    //     }

    //     //Check if light is blue
    //     else if (cds.Value() > 1.75 && cds.Value() < 2.5){
    //         move_backward(25, 134.96);
    //         turn_left(25, 200);
    //         lineTracking(15, 6.9);
    //     }
    // }

    //Move forward 7 inches
        move_forward(25, 238.185);

        //Make a slight turn left
        turn_left(25, 120);

        //Move forward 12 inches
        move_forward(25, 404.89);
}
