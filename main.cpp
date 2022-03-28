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

// RPS Delay time
#define RPS_WAIT_TIME_IN_SEC 0.35

// Shaft encoding counts for CrayolaBots
#define COUNTS_PER_INCH 40.5
#define COUNTS_PER_DEGREE 2.48

// Defines for pulsing the robot
#define PULSE_TIME 0.05
#define PULSE_POWER 30

// Define for the motor power
#define POWER 25

// Orientation of QR Code
#define PLUS 0
#define MINUS 1

DigitalEncoder right_encoder(FEHIO::P3_0);
DigitalEncoder left_encoder(FEHIO::P1_0);
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

void check_starting_light(float lowerbound, float upperbound){
    while (cds.Value() <= lowerbound || cds.Value() >= upperbound) {
        LCD.Write(left_opt.Value());
        LCD.Write("\t");
        LCD.Write(middle_opt.Value());
        LCD.Write("\t");
        LCD.Write(right_opt.Value());
        LCD.WriteLine("\n");
            left_motor.Stop();
            right_motor.Stop();
            LCD.WriteLine(cds.Value());
            Sleep(1.0);
        }
}

void move_backward(int percent, float counts, float timeFailSafe) //using encoders
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

void move_forward(int percent, float counts, float timeFailSafe) //using encoders
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

/*
 * Pulse forward a short distance using time
 */
void pulse_forward(int percent, float seconds) 
{
    // Set both motors to desired percent
    right_motor.SetPercent(-percent);
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
    left_motor.SetPercent(percent);

    // Wait for the correct number of seconds
    Sleep(seconds);

    // Turn off motors
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

    left_motor.SetPercent(fast_motor_percent);
    right_motor.SetPercent(-fast_motor_percent);
    LCD.Write("Sexy\n");

    //See if the left and middle sensor are off the track
    while (left_opt.Value() <= 2.0 && middle_opt.Value() <= 1.0) {

        //Set the right wheel to go slower
        left_motor.SetPercent(fast_motor_percent);
        right_motor.SetPercent(-slow_motor_percent);

        //Set the right wheel to go slower
        LCD.Write("Going Right\n");

        //Stop moving right if the middle sensor is over the track
        if (middle_opt.Value() >= 2.0) {
            break;
        }
    }

    //See if the middle and right sensor are off the track
    while (middle_opt.Value() <= 1.0 && right_opt.Value() <= 2.5) {
        left_motor.SetPercent(slow_motor_percent);
        right_motor.SetPercent(-fast_motor_percent);

        LCD.Write("Going Left\n");

        //Stop moving right if the middle sensor is over the track
        if (middle_opt.Value() >= 2.0) {
            break;
        }
    }
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
    while((RPS.X() >= 0) && (RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1))
    {
        LCD.WriteLine("X: ");
        LCD.WriteLine(RPS.X());
        LCD.WriteLine(x_coordinate);
        if(RPS.X() < x_coordinate)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_forward(power, PULSE_TIME);
        }
        else
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_forward(-power, PULSE_TIME);
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
    while((RPS.Y() >= 0) && (RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1))
    {
        LCD.WriteLine("Y: ");
        LCD.WriteLine(RPS.Y());
        LCD.WriteLine(y_coordinate);
        if(RPS.Y() < y_coordinate)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_forward(power, PULSE_TIME);
        }
        else
        {
            // Pulse the motors for a short duration in the correct direction
           pulse_forward(-power, PULSE_TIME);
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}

/* 
 * Use RPS to move to the desired heading
 */
void check_heading(float heading)
{
    
    int power = PULSE_POWER;
    if(heading == MINUS){
        power = -PULSE_POWER;
    }

    // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
    float time = TimeNow();
    while(((RPS.Heading() >= 0) && (RPS.Heading() < heading - 2 || RPS.Heading() > heading + 2)) && (TimeNow() - time < 20.0))
    {
        LCD.WriteLine("Heading: ");
        LCD.WriteLine(RPS.Heading());
        LCD.WriteLine(heading);
        if(RPS.Heading() < heading)
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_counterclockwise(power, PULSE_TIME);
        }
        else
        {
            // Pulse the motors for a short duration in the correct direction
            pulse_counterclockwise(-power, PULSE_TIME);
        }
        Sleep(RPS_WAIT_TIME_IN_SEC);
    }
}

void celebrate(){
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
    //intitalize RPS values
    float touch_x,touch_y;
    float vanilla_y = 51.7;
    float twist_y = 58.5;
    float chocolate_y = 58.5;
    float goingDown_x = 20.5;
    float goingDown_heading = 88.7;
    float icecream_heading = 135.3;
    float dowmRamp_y = 13.0;
    float finalButton_heading = 137.0;
    float first_turn_x = 18.7;
    float first_turn_y = 18.5;
    float first_turn_heading = 180.0;
    float tray_turn_x = 10.0;
    float tray_turn_heading = 93.0;
    float before_color_heading = 92.6;
    float jukebox_light_y = 15.3;
    float after_color_y = 17.9;
    float turn_ramp_1_heading = 181.9;
    float turn_ramp_1_x = 21.5;
    float turn_ramp_2_heading = 273.0;

    //Tell the robot which course it's on
    RPS.InitializeTouchMenu();

    LCD.WriteLine("RPS & Data Logging Test");
    LCD.WriteLine("Press Screen To Start");
    while(!LCD.Touch(&touch_x,&touch_y));
    while(LCD.Touch(&touch_x,&touch_y));

    // get the voltage level and display it to the screen
        LCD.WriteLine("Battery Voltage: ");
        LCD.WriteLine(Battery.Voltage());
        LCD.WriteLine("\n");
        Sleep(0.5);

    //Check if the starting light is not red
    check_starting_light(0.3, 0.7);

    //Move to ramp
    move_forward(2 * testSpeed, 200, 5.0); //move forward from starting light
    Sleep(1.0);
    
    check_y(first_turn_y, MINUS);

    //turn to go to trash can
    turn_left(testSpeed, 100);

    check_heading(first_turn_heading);

    //Correct position against wall
    for (int i = 0; bump_switch1.Value() == 1 || bump_switch2.Value() == 1; i++){
        move_forward(2 * testSpeed, i, 5.0);
    }

    //pulse back to line up with the trash can
    check_x(tray_turn_x, PLUS);

    //turn to trash can
    turn_right(testSpeed, ninetyDegreeCount);

    check_heading(tray_turn_heading);

    //Keep moving until at least one of the bump switches is against the trash can
    for (int i = 0; bump_switch1.Value() == 1 && bump_switch2.Value() == 1; i++){
        move_forward(testSpeed, i, 5.0);
    }

    //move bucket arm down
    move_bucket_arm(armSpeed, 1.75);

    //move back from trash can
    move_backward(slowSpeed, 2, 5.0);

    //move bucket arm  back up
    move_bucket_arm(-armSpeed, 1.75);

    //turn until optosensors find black line
    right_motor.SetPercent(slowSpeed);
    left_motor.SetPercent(slowSpeed);

    bool keepMoving = true;
    while ((middle_opt.Value() < 1.0) && keepMoving){
        

        if (middle_opt.Value() >= 1.0) {
            right_motor.Stop();
            left_motor.Stop();

            Sleep(1.0);

            keepMoving = false;
        }
    }

    //do line following
    lineTracking(slowSpeed, 6.9);

    //check whether the cds light is red or blue and stop
    if (cds.Value() >= 0.3 && cds.Value() <= 2.0) {
            left_motor.Stop();
            right_motor.Stop();
            LCD.WriteLine("Light Detected");
            LCD.WriteLine(cds.Value());
            Sleep(1.0);

            if (cds.Value() >= 0.3 && cds.Value() <= 0.7){
                LCD.WriteLine("RED");
            }
            else {
                LCD.WriteLine("BLUE");
            }
    }
    else {
        LCD.WriteLine("Aya, why you so failure");
        SLeep(1.0);
    }
}
