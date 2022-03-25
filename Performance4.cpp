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

void move_forward(int percent, float counts, float timeFailSafe) //using encoders
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

void move_backward(int percent, float counts, float timeFailSafe) //using encoders
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
    move_forward(testSpeed + 10, 200, 5.0); //move forward from starting light
    Sleep(1.0);
    turn_left(testSpeed + 10, 305); //turn to ramp
    Sleep(1.0);
    move_backward(3 * testSpeed, 500, 5.0); //move up ramp
    move_backward(testSpeed, 300, 5.0); //move up ramp
    Sleep(1.0);

    // Check which ice cream lever to flip
    if(RPS.GetIceCream() == 0)
    {
        // Flip vanilla lever
        check_y(vanilla_y, MINUS);

        Sleep(1.0);

        //move toward ice cream
        turn_right(testSpeed, ninetyDegreeCount + 69);

        Sleep(1.0);

        //Check position in front of vanilla lever
        check_heading(icecream_heading);

        Sleep(1.0);

        //Keep running until it detects the black line

        right_motor.SetPercent(-testSpeed);
        left_motor.SetPercent(testSpeed);

        bool keepMoving = true;
        while ((left_opt.Value() < 1.5 || middle_opt.Value() < 1.0) && keepMoving){
            right_motor.SetPercent(-testSpeed);
            left_motor.SetPercent(testSpeed);

            if (left_opt.Value() >= 1.5 || middle_opt.Value() >= 1.0) {
                right_motor.Stop();
                left_motor.Stop();

                Sleep(1.0);

                move_forward(slowSpeed, 6, 5.0);
                keepMoving = false;
            }
        }
    } 
    else if(RPS.GetIceCream() == 1)
    {
        // Flip twist lever
        check_y(twist_y, MINUS);

        Sleep(1.0);

        move_backward(slowSpeed, 2, 5.0);

        //move toward ice cream
        turn_right(testSpeed, ninetyDegreeCount + 69);

        Sleep(1.0);

        //Check position in front of vanilla lever
        check_heading(icecream_heading);

        Sleep(1.0);

        //Keep running until it detects the black line

        right_motor.SetPercent(-testSpeed);
        left_motor.SetPercent(testSpeed);

        bool keepMoving = true;
        while ((left_opt.Value() < 1.5 || middle_opt.Value() < 1.0) && keepMoving){
        

            if (left_opt.Value() >= 1.5 || middle_opt.Value() >= 1.0) {
                right_motor.Stop();
                left_motor.Stop();

                Sleep(1.0);

                move_forward(slowSpeed, 6, 5.0);
                keepMoving = false;
            }
        }

    }
    else if(RPS.GetIceCream() == 2)
    {

       // Flip twist lever
        check_y(chocolate_y, MINUS);

        Sleep(1.0);

        move_backward(slowSpeed, 3, 5.0);

        //move toward ice cream
        turn_right(testSpeed, ninetyDegreeCount + 80);

        Sleep(1.0);

        //Check position in front of vanilla lever
        check_heading(119.0);

        Sleep(1.0);

        //Keep running until it detects the black line

        right_motor.SetPercent(-testSpeed);
        left_motor.SetPercent(testSpeed);

        bool keepMoving = true;
        while ((left_opt.Value() < 1.5 || middle_opt.Value() < 1.0) && keepMoving){
        

            if (left_opt.Value() >= 1.5 || middle_opt.Value() >= 1.0) {
                right_motor.Stop();
                left_motor.Stop();

                Sleep(1.0);

                move_forward(slowSpeed, 6, 5.0);
                keepMoving = false;
            }
        }

    }

    Sleep(1.0);

    //Flip the lever
    move_bucket_arm(3 * armSpeed, 1.5); //move arm down
    Sleep(1.0);
    move_backward(slowSpeed, 10, 5.0); //move back from lever
    Sleep(1.0);
    move_bucket_arm(armSpeed, 0.5); //move arm down
    Sleep(1.0);
    move_forward(slowSpeed, 12, 5.0); //move back into lever
    Sleep(5.0);
    move_bucket_arm(-4 * armSpeed, 1.0); //move arm back up
    Sleep(1.0);
    move_bucket_arm(armSpeed, 1.5); //move arm back down little
    Sleep(1.0);

    //back up from lever
    move_backward(testSpeed, 205, 5.0); //move back from lever
    Sleep(1.0);
    check_x(goingDown_x, MINUS); //check if robot is aligned with ramp
    Sleep(1.0);

    //Move down ramp
    turn_right(testSpeed, 90); //turn to face the ramp
    Sleep(1.0);
    check_heading(goingDown_heading); //check the angle of the robot
    Sleep(1.0);
    move_backward(testSpeed, 750, 10.0); //go down the ramp
    Sleep(1.0);
    check_y(dowmRamp_y, PLUS); //check the y coordinate after going down the ramp
    Sleep(1.0);

    //Final button
    turn_left(testSpeed, 100); //face the final button
    Sleep(1.0);
    check_heading(finalButton_heading); //Check if the robot is aligned with the button
    Sleep(1.0);
    move_backward(2 * testSpeed, 500, 5.0); //Ram that shit into the button

    Sleep(2.0);

    //Celebrate that the code ran all the way through
    celebrate();

    LCD.WriteLine("Hell yeah");    

}
