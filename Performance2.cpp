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
#define armSpeed 10
#define ninetyDegreeCount 220
#define SERVO_MIN 510
#define SERVO_MAX 2410

DigitalEncoder right_encoder(FEHIO::P1_0);
DigitalEncoder left_encoder(FEHIO::P3_0);
FEHMotor right_motor(FEHMotor::Motor1,9.0);
FEHMotor left_motor(FEHMotor::Motor0,9.0);
AnalogInputPin cds(FEHIO::P0_1);
AnalogInputPin left_opt(FEHIO::P2_7);
AnalogInputPin middle_opt(FEHIO::P2_0);
AnalogInputPin right_opt(FEHIO::P1_4);
FEHMotor bucket_arm(FEHMotor::Motor2,9.0);
FEHServo sliding_arm(FEHServo::Servo0); 

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

LCD.WriteLine("Left");

//Reset encoder counts

right_encoder.ResetCounts();

left_encoder.ResetCounts();

//Set both motors to desired percent

left_motor.SetPercent(percent);
right_motor.SetPercent(percent);

//While the average of the left and right encoder is less than counts,
//keep running motors

while (((left_encoder.Counts() + right_encoder.Counts()) / 2) < counts);

//Turn off motors

right_motor.Stop();

left_motor.Stop();

}

void turn_right(int percent, int counts) //using encoders
{

LCD.WriteLine("Right");

//Reset encoder counts

right_encoder.ResetCounts();

left_encoder.ResetCounts();

//Set both motors to desired percent

//hint: set right motor backwards, left motor forwards

left_motor.SetPercent(-1 * percent);
right_motor.SetPercent(-1 * percent);

//While the average of the left and right encoder is less than counts,
//keep running motors

while (((left_encoder.Counts() + right_encoder.Counts()) / 2) < counts);

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

    //move the arm for a certain amount of time
    while(TimeNow() - time < seconds){
        if(percent > 0){
            LCD.WriteLine("Dropping bucket");
            Sleep(0.5);
        }
        else {
            LCD.WriteLine("Raising bucket");
            Sleep(0.5);
        }
    }
    bucket_arm.Stop();
}

void move_sliding_arm(float start_angle, float end_angle){

    //Set the servo min and max
    sliding_arm.SetMin(SERVO_MIN);
    sliding_arm.SetMax(SERVO_MAX);

    //Start at 0 degrees
    LCD.WriteLine("Sliding arm");
    LCD.WriteLine("Please stand clear");
    sliding_arm.SetDegree(start_angle);
    Sleep(1.0);
    //Set end angle
    LCD.WriteLine("Sliding arm");
    LCD.WriteLine("Please stand clear");
    sliding_arm.SetDegree(end_angle);
}

int main(void)
{
    //Check if the starting light is not red
    while (cds.Value() <= 0.3 || cds.Value() >= 0.7) {
        left_motor.Stop();
        right_motor.Stop();
        LCD.WriteLine(cds.Value());
        Sleep(1.0);
    }

    //Move to trash can
    move_forward(testSpeed, 200); //move forward from starting light
    Sleep(1.0);
    turn_left(testSpeed, 98); //make a slight turn
    Sleep(1.0);
    move_forward(testSpeed, 366); //move forward to be level with trash can
    Sleep(1.0);
    turn_right(testSpeed, ninetyDegreeCount - 5); //make a 90 degree turn right
    Sleep(1.0);
    move_forward(testSpeed, 327); //move forward to trash can

    //Dump tray into trash can
    move_bucket_arm(armSpeed, 1.75); //move arm down
    Sleep(1.0);
    move_bucket_arm(-1 * armSpeed, 4.0); //retract arm back up

    //Move up the ramp
    move_backward(15, 250); //move back from trash can
    Sleep(1.0);
    turn_left(testSpeed, ninetyDegreeCount - 10); //turn left to move to the ramp
    Sleep(1.0);
    move_backward(15, 330); //move closer to the ramp
    Sleep(1.0);
    turn_left(testSpeed, 183); //turn to face the ramp
    Sleep(1.0);
    move_backward(2 * testSpeed, 475); //move up the ramp
    Sleep(1.0);
    // move_backward(15, 25);
    // Sleep(1.0);

    //move to the ticket
    turn_right(testSpeed, ninetyDegreeCount + 5); //move to closer to ticket
    Sleep(1.0);
    move_forward(testSpeed, 150); //move up the ramp
    Sleep(1.0);
    move_backward(testSpeed, 500); //move to middle of ticket station
    move_forward(10, 2); //move to middle of ticket station
    turn_right(testSpeed, 10); //closer baby
    move_backward(10, 6); //move to middle of ticket station
    Sleep(1.0);

    //pull out arm to move ticket
    move_sliding_arm(180.0, 170.0); //Move ticket to final position
    Sleep(1.0);
    turn_right(testSpeed, 10); //closer baby
    Sleep(1.0);
    move_sliding_arm(170.0, 160.0); //Move ticket to final position
    Sleep(1.0);
    turn_right(testSpeed, ninetyDegreeCount); //turn robot to slide ticket
    move_sliding_arm(160.0 , 0.0); //Move ticket from initial position
    Sleep(1.0);
    move_sliding_arm(0.0 , 180.0); //Move ticket from initial position
    Sleep(1.0);

    //touch stove
    turn_left(testSpeed, ninetyDegreeCount); //face the stove
    Sleep(1.0);
    move_backward(2 * testSpeed, 850); //Move closer to the stove
        
    //Celebrate that the code ran all the way through
    Buzzer.Tone(FEHBuzzer::G3, 83);
    Buzzer.Tone(FEHBuzzer::C4, 83);
    Buzzer.Tone(FEHBuzzer::E4, 83);
    Buzzer.Tone(FEHBuzzer::G4, 83);
    Buzzer.Tone(FEHBuzzer::C5, 83);
    Buzzer.Tone(FEHBuzzer::E5, 83);
    Buzzer.Tone(FEHBuzzer::G5, 250);
    Buzzer.Tone(FEHBuzzer::E5, 250);
    Buzzer.Tone(FEHBuzzer::Af3, 83);
    Buzzer.Tone(FEHBuzzer::C4, 83);
    Buzzer.Tone(FEHBuzzer::Ef4, 83);
    Buzzer.Tone(FEHBuzzer::Af4, 83);
    Buzzer.Tone(FEHBuzzer::C5, 83);
    Buzzer.Tone(FEHBuzzer::Ef5, 83);
    Buzzer.Tone(FEHBuzzer::Af5, 250);
    Buzzer.Tone(FEHBuzzer::E5, 250);
    Buzzer.Tone(FEHBuzzer::Bf3, 83);
    Buzzer.Tone(FEHBuzzer::D4, 83);
    Buzzer.Tone(FEHBuzzer::F4, 83);
    Buzzer.Tone(FEHBuzzer::Bf4, 83);
    Buzzer.Tone(FEHBuzzer::D5, 83);
    Buzzer.Tone(FEHBuzzer::F5, 83);
    Buzzer.Tone(FEHBuzzer::Bf5, 250);
    Buzzer.Tone(FEHBuzzer::Bf5, 83);
    Buzzer.Tone(FEHBuzzer::Bf5, 83);
    Buzzer.Tone(FEHBuzzer::Bf5, 83);
    Buzzer.Tone(FEHBuzzer::C6, 1000);

    LCD.WriteLine("Hell yeah");
    
}
