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
#define slowSpeed 15
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
FEHMotor prong_arm(FEHMotor::Motor3,9.0);
DigitalInputPin bump_switch1(FEHIO::P0_3);
DigitalInputPin bump_switch2(FEHIO::P3_2);


void move_forward(int percent, int counts, float timeFailSafe) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(-1 * percent);
    left_motor.SetPercent(percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors

    float time = TimeNow();
    while(((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts) && (TimeNow() - time < timeFailSafe)){
        LCD.WriteLine("forward");
        Sleep(1.0);
    }

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void move_backward(int percent, int counts, float timeFailSafe) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(-1 * percent);

    //While the average of the left and right encoder is less than counts,
    //keep running motors

    float time = TimeNow();
    while(((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts) && (TimeNow() - time < timeFailSafe)){
        LCD.WriteLine("backward");
        Sleep(1.0);
    }
    
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

void move_prong_arm(int percent, float seconds){

    //Set desired motor percentage
    prong_arm.SetPercent(percent);

    float time = TimeNow();

    //move the arm for a certain amount of time
    while(TimeNow() - time < seconds){
        if(percent > 0){
            LCD.WriteLine("Turning arm clockwise");
            Sleep(0.5);
        }
        else {
            LCD.WriteLine("Turning arm counterclockwise");
            Sleep(0.5);
        }
    }
    prong_arm.Stop();
}

int main(void)
{
    // get the voltage level and display it to the screen
        LCD.WriteLine("Battery Voltage: ");
        LCD.WriteLine(Battery.Voltage());
        LCD.WriteLine("\n");
        Sleep(0.5);
    //Check if the starting light is not red
    while (cds.Value() <= 0.3 || cds.Value() >= 0.7) {
        left_motor.Stop();
        right_motor.Stop();
        LCD.WriteLine(cds.Value());
        Sleep(1.0);
    }

    //Move up ramp
    move_forward(slowSpeed, 240, 5.0); //move forward from starting light
    Sleep(1.0);
    turn_left(slowSpeed, 305); //turn to ramp
    Sleep(1.0);
    move_backward(3 * testSpeed, 750, 5.0); //move up ramp
    Sleep(1.0);

    //Move to burger flip
    turn_right(testSpeed, ninetyDegreeCount);
    Sleep(1.0);

    //Correct position against wall
    for (int i = 0; bump_switch1.Value() == 1 || bump_switch2.Value() == 1; i++){
        move_forward(testSpeed, i, 5.0);
    }

    //align with stove
    move_backward(slowSpeed, 104, 5.0);
    Sleep(1.0);

    //turn towards stove
    turn_right(testSpeed, ninetyDegreeCount + 3);
    Sleep(1.0);

    //move to stove
    move_backward(slowSpeed, 500, 10.0);

    Sleep(1.0);

    //Flip burger
    move_prong_arm(5 * armSpeed, 0.5);
    move_prong_arm(-5 * armSpeed, 0.5);

    //back out of wheel
    move_forward(testSpeed, 50, 2.0);

    //move toward ice cream
    turn_right(testSpeed, ninetyDegreeCount + 69);

    //move toward a lever
    move_forward(testSpeed, 750, 10.0);

    //Flip a lever
    move_bucket_arm(armSpeed, 2.0);
    Sleep(1.0);
    move_bucket_arm(-1 * armSpeed, 2.0);

    Sleep(1.0);

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
