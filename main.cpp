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

//Define RPS values
#define vanilla_y 49.0
#define twist_y 54.5
#define chocolate_y 53.5
#define goingDown_x 20.3
#define goingDown_heading 88.7
#define icecream_heading 135.3
#define downRamp_y 16.0
#define finalButton_heading 137.0
#define first_turn_x 18.7
#define first_turn_y 18.5
#define first_turn_heading 180.0
#define tray_turn_x 10.0
#define jukebox_light_x 12.0
#define tray_turn_heading 93.0
#define before_color_heading 92.6
#define jukebox_light_y 14.0
#define after_color_y 17.9
#define turn_ramp_1_heading 181.9
#define turn_ramp_1_x 22.0
#define turn_ramp_2_heading 270.0
#define burger_flip_1_heading 5.0
#define burger_flip_x 24.9
#define burger_flip_y 63.5
#define burger_flip_2_heading 273.0
#define sliding_ticket_heading  92.1
#define before_icecream_x 17.0
#define before_icecream_heading 273.0

DigitalEncoder right_encoder(FEHIO::P3_0);
DigitalEncoder left_encoder(FEHIO::P1_0);
FEHMotor right_motor(FEHMotor::Motor1,9.0);
FEHMotor left_motor(FEHMotor::Motor0,9.0);
AnalogInputPin cds(FEHIO::P0_1);
AnalogInputPin left_opt(FEHIO::P2_7);
AnalogInputPin middle_opt(FEHIO::P2_0);
AnalogInputPin right_opt(FEHIO::P1_3);
FEHMotor bucket_arm(FEHMotor::Motor2,9.0);
FEHServo sliding_arm(FEHServo::Servo0); 
FEHMotor prong_arm(FEHMotor::Motor3,9.0);
DigitalInputPin bump_switch1(FEHIO::P0_3);
DigitalInputPin bump_switch2(FEHIO::P3_2);
DigitalInputPin bump_switch3(FEHIO::P3_4);

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
    while (middle_opt.Value() <= 1.0 && right_opt.Value() <= 3.2) {
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

void check_heading_two(float heading)
{
    
    int power = PULSE_POWER;
    if(heading == MINUS){
        power = -PULSE_POWER;
    }

    // Check if receiving proper RPS coordinates and whether the robot is within an acceptable range
    float time = TimeNow();
    while(((RPS.Heading() >= 0) && (RPS.Heading() < heading - 2 || RPS.Heading() > heading + 2)) && (TimeNow() - time < 2.0))
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

    //Tell the robot which course it's on
    RPS.InitializeTouchMenu();
    Sleep(0.5);

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
    check_starting_light(0.25, 0.7);
    
    /*
    *TRAY DUMP
    */

    //move bucket arm down
    move_bucket_arm(armSpeed, 0.70);

    //Move to trash
    move_forward(2 * testSpeed, 200, 5.0); //move forward from starting light
    Sleep(0.25);
    
    check_y(first_turn_y, MINUS);

    //turn to go to trash can
    turn_left(testSpeed, 100);

    check_heading(first_turn_heading);

    right_motor.SetPercent(testSpeed + 10);
    left_motor.SetPercent(-testSpeed - 10);

    //Correct position against wall
    while (bump_switch1.Value() == 1 || bump_switch2.Value() == 1);

    right_motor.Stop();
    left_motor.Stop();

    //move back to line up with the trash can
    move_backward(slowSpeed + 5, 10, 5.0);

    //pulse back to line up with the trash can
    check_x(tray_turn_x, PLUS);

    //turn to trash can
    turn_right(testSpeed, ninetyDegreeCount);

    check_heading(tray_turn_heading);

    //Keep moving until at least one of the bump switches is against the trash can
    right_motor.SetPercent(testSpeed);
    left_motor.SetPercent(-testSpeed);

    //Correct position against wall
    while (bump_switch1.Value() == 1 && bump_switch2.Value() == 1);

    right_motor.Stop();
    left_motor.Stop();

    //move bucket arm down
    move_bucket_arm(armSpeed, 0.9);

    //move back from trash can
    move_backward(slowSpeed, 2, 5.0);

    //move bucket arm  back up
    move_bucket_arm(-armSpeed, 1.75);

    //move back from trash can
    move_backward(slowSpeed + 5, 3, 5.0);

    /*
    END TRAY DUMP

    START JUKEBOX LIGHT
    */

    //turn from tray
    turn_left(testSpeed, ninetyDegreeCount - 20);
    Sleep(0.25);

    //move away from tray
    check_x(jukebox_light_x + 0.5, PLUS);
    Sleep(0.25);
    turn_right(testSpeed, ninetyDegreeCount);
    check_heading(before_color_heading);


    right_motor.SetPercent(-testSpeed);
    left_motor.SetPercent(testSpeed);

    bool keepMoving = true;
    while ((cds.Value() < 0.3 || cds.Value() > 2.0) && keepMoving){

        if ((cds.Value() >= 0.3) && (cds.Value() <= 2.0)) {
            right_motor.Stop();
            left_motor.Stop();

            Sleep(1.0);

            keepMoving = false;
        }
    }

    check_y(jukebox_light_y, MINUS);

    check_heading(before_color_heading);

    //check whether the cds light is red or blue and stop
    if (cds.Value() >= 0.3 && cds.Value() <= 2.0) {
            left_motor.Stop();
            right_motor.Stop();
            LCD.WriteLine("Light Detected");
            LCD.WriteLine(cds.Value());
            Sleep(0.25);

            if (cds.Value() >= 0.3 && cds.Value() <= 0.7){
                LCD.WriteLine("RED");

                move_prong_arm(-armSpeed, 0.5);

                move_backward(slowSpeed, 5, 5.0);

                right_motor.SetPercent(-20);
                left_motor.SetPercent(-20);
                keepMoving = true;

                while ((middle_opt.Value() < 0.7) && keepMoving){
                if (middle_opt.Value() >= 0.7) {
                    right_motor.Stop();
                    left_motor.Stop();

                    Sleep(0.25);

                    keepMoving = false;
                    }
                }
                move_backward(20, 10.0, 1.5);
                move_forward(testSpeed, 8.0, 1.5);

                move_prong_arm(armSpeed, 0.5);

                turn_left(testSpeed, ninetyDegreeCount);

                Sleep(0.25);

                //check heading before moving towards ramp
                check_heading(turn_ramp_1_heading);

                //move towards ramp
                move_backward(testSpeed, 70, 5.0);

                //check x value before turning
                check_x(turn_ramp_1_x, PLUS);
            }
            else {
                LCD.WriteLine("BLUE");

                move_prong_arm(armSpeed, 0.5);

                move_backward(slowSpeed, 5, 5.0);

                right_motor.SetPercent(20);
                left_motor.SetPercent(20);
                keepMoving = true;

                while ((middle_opt.Value() < 0.7) && keepMoving){
                if (middle_opt.Value() >= 0.7) {
                    right_motor.Stop();
                    left_motor.Stop();

                    Sleep(0.25);

                    keepMoving = false;
                    }
                }
                
                move_backward(20, 10.0, 1.5);
                move_forward(testSpeed, 5.0, 1.5);

                move_prong_arm(-armSpeed, 0.5);

                turn_left(testSpeed, ninetyDegreeCount - 40);

                Sleep(0.25);

                //check heading before moving towards ramp
                check_heading(turn_ramp_1_heading);

                //move towards ramp
                move_backward(testSpeed + 10, 160, 5.0);

                Sleep(0.25);

                //check x value before turning
                check_x(turn_ramp_1_x, PLUS);
            }
    }
    else {
        LCD.WriteLine("Aya, why you so failure");
        Sleep(1.0);
    }


    /*
    END JUKEBOX LIGHT

    START RAMP
    */


    //turn to ramp
    turn_left(testSpeed, ninetyDegreeCount - 25);

    Sleep(0.25);

    check_heading(turn_ramp_2_heading);

    // move up ramp
    right_motor.SetPercent((-3 * testSpeed) + 2);
    left_motor.SetPercent(3 * testSpeed);

    while (RPS.Y() < 45.0);

    right_motor.Stop();
    left_motor.Stop();


    /*
    END RAMP

    START BURGER FLIP
    */

    //Move to burger flip
    turn_left(testSpeed, ninetyDegreeCount + 30);

    move_forward(35, 30, 2.0);

    Sleep(0.25);

    check_heading(burger_flip_1_heading);
    
    right_motor.SetPercent(testSpeed + 10);
    left_motor.SetPercent(-testSpeed - 10);

    //Correct position against wall
    while (bump_switch1.Value() == 1 || bump_switch2.Value() == 1);

    right_motor.Stop();
    left_motor.Stop();

    // move in front of burger flip
    move_backward(20, 20.0, 5.0);
    Sleep(0.25);

    // check x before turning
    check_x(burger_flip_x, MINUS);

    //turn to burger flip
    turn_right(testSpeed, ninetyDegreeCount + 1);
    Sleep(0.25);

    //check heading
    check_heading(burger_flip_2_heading);

    right_motor.SetPercent(-testSpeed);
    left_motor.SetPercent(testSpeed);

    bool moveBurgerLine = true;
    while ((left_opt.Value() < 2.0 || middle_opt.Value() < 0.8) && moveBurgerLine){
        

        if (left_opt.Value() >= 2.0 || middle_opt.Value() >= 0.8) {
            right_motor.Stop();
            left_motor.Stop();

            Sleep(1.0);

            moveBurgerLine = false;
        }
    }

    //check heading
    check_heading(burger_flip_2_heading);

    // move closer to wheel then check with rps
    move_backward(20, 4.0, 1.0);
    Sleep(0.25);
    
    //while (RPS.Y() < burger_flip_y){
    //    check_heading(burger_flip_2_heading);
    //    pulse_forward(PULSE_POWER, 2 * PULSE_TIME);
    //}
    
    check_heading_two(burger_flip_2_heading);

    Sleep(0.25);

    pulse_forward(PULSE_POWER, 2 * PULSE_TIME);

    Sleep(0.25);
    
    check_heading_two(burger_flip_2_heading);

    Sleep(0.25);

    pulse_forward(PULSE_POWER, 2 * PULSE_TIME);

    Sleep(0.25);
    
    check_heading_two(burger_flip_2_heading);

    Sleep(0.25);

    pulse_forward(PULSE_POWER, 2 * PULSE_TIME);

    Sleep(0.25);
    
    check_heading_two(burger_flip_2_heading);

    Sleep(0.25);

    pulse_forward(PULSE_POWER, 2 * PULSE_TIME);

    Sleep(0.25);

    pulse_forward(-PULSE_POWER, PULSE_TIME);

    //Flip burger
    move_prong_arm(5 * armSpeed, 0.5);
    move_prong_arm(-5 * armSpeed, 0.5);

    /*
    END BURGER FLIP

    START SLIDING TICKET
    */

    //Back out of burger wheel
    move_forward(testSpeed, 150, 5.0);

    //Turn 180 degrees to face the ticket
    turn_left(testSpeed, 2 * ninetyDegreeCount);

    Sleep(0.25);

    check_heading(sliding_ticket_heading);

    //Stick sliding arm out
    move_sliding_arm(90.0, 180.0);

    right_motor.SetPercent(-testSpeed);
    left_motor.SetPercent(testSpeed);

    float slidingArmTime = TimeNow();
    //move until bump switch is pressed
    while ((bump_switch3.Value() == 1) && (TimeNow() - slidingArmTime < 3.0));

    right_motor.Stop();
    left_motor.Stop();

    Sleep(0.25);

    float insertSlidingArmTime = TimeNow();
    //angle arm into ticket crevice
    while ((RPS.Y() < 107) && (TimeNow() - insertSlidingArmTime < 2.0)){
        check_heading(107.0);
    }

    //move more into crevice
    move_backward(slowSpeed, 12, 5.0);

    //move the ticket
    turn_right(testSpeed, 70);

    //try to enter the robot
    turn_left(testSpeed, 50);

    //back out of ticket
    move_forward(testSpeed, 90, 5.0);

    //retract arm back up
    move_sliding_arm(180.0, 90.0);

    /*
    END SLIDING TICKET

    START ICE CREAM LEVER
    */

    //face the wall
    turn_right(testSpeed, ninetyDegreeCount);

    right_motor.SetPercent(testSpeed);
    left_motor.SetPercent(-testSpeed);

    //Correct position against wall
    while (bump_switch1.Value() == 1 || bump_switch2.Value() == 1);

    right_motor.Stop();
    left_motor.Stop();

    //get closer to ice cream levers
    right_motor.SetPercent(-testSpeed);
    left_motor.SetPercent(testSpeed);

    while (RPS.X() > before_icecream_x + 1);

    right_motor.Stop();
    left_motor.Stop();

    check_x(before_icecream_x, PLUS);

    //face ice cream levers
    turn_right(testSpeed, ninetyDegreeCount);

    check_heading(before_icecream_heading);
    
    // Check which ice cream lever to flip
    if(RPS.GetIceCream() == 0)
    {
        // Flip vanilla lever
        check_y(vanilla_y, PLUS);

        //move toward ice cream
        turn_right(testSpeed, ninetyDegreeCount + 90);

        Sleep(0.25);

        //Check position in front of vanilla lever
        check_heading(icecream_heading);

        //Keep running until it detects the black line

        bool keepMoving = true;
        while ((left_opt.Value() < 1.5 || middle_opt.Value() < 1.0) && keepMoving){
            right_motor.SetPercent(testSpeed + 10);
            left_motor.SetPercent(-testSpeed - 10);

            if (left_opt.Value() >= 1.5 || middle_opt.Value() >= 1.0) {
                right_motor.Stop();
                left_motor.Stop();

                move_forward(slowSpeed, 6, 5.0);
                keepMoving = false;
            }
        }

        Sleep(0.25);

        //Flip the lever
        move_bucket_arm(3 * armSpeed, 1.5); //move arm down
        move_backward(slowSpeed, 10, 5.0); //move back from lever
        Sleep(1.0);
        move_bucket_arm(armSpeed, 0.5); //move arm down
        Sleep(1.0);
        move_forward(slowSpeed, 12, 5.0); //move back into lever
        Sleep(2.0);
        move_bucket_arm(-4 * armSpeed, 1.0); //move arm back up
        move_bucket_arm(armSpeed, 1.5); //move arm back down little

        //back up from lever
        move_backward(testSpeed, 205, 5.0); //move back from lever
        Sleep(0.25);
        if (RPS.X() < goingDown_x){
            check_x(goingDown_x, PLUS); //check if robot is aligned with ramp
        }
        else {
            check_x(goingDown_x, MINUS); //check if robot is aligned with ramp
        }
    } 
    else if(RPS.GetIceCream() == 1)
    {
        //move closer to the levers
        //move_backward(testSpeed, 5, 0.5);

        // Flip twist lever
        right_motor.SetPercent(-testSpeed);
        left_motor.SetPercent(testSpeed);

        while (RPS.Y() < (twist_y));

        right_motor.Stop();
        left_motor.Stop();

        Sleep(0.25);
        if (RPS.Y() > twist_y) {
            check_y(twist_y, PLUS); //check the y coordinate after going down the ramp
        }
        else{
            check_y(twist_y, MINUS); //check the y coordinate after going down the ramp
        }

        //move toward ice cream
        turn_right(testSpeed, ninetyDegreeCount + 80);

        Sleep(0.25);

        //Check position in front of vanilla lever
        check_heading(icecream_heading);

        //Keep running until it detects the black line

        right_motor.SetPercent(testSpeed + 10);
        left_motor.SetPercent(-testSpeed - 10);

        bool keepMoving = true;
        while ((left_opt.Value() < 1.5 || middle_opt.Value() < 1.0) && keepMoving){
        

            if (left_opt.Value() >= 1.5 || middle_opt.Value() >= 1.0) {
                right_motor.Stop();
                left_motor.Stop();

                move_forward(slowSpeed, 6, 5.0);
                keepMoving = false;
            }
        }

        Sleep(0.25);

        //Flip the lever
        move_bucket_arm(3 * armSpeed, 1.5); //move arm down
        move_backward(slowSpeed, 10, 5.0); //move back from lever
        Sleep(1.0);
        move_bucket_arm(armSpeed, 0.5); //move arm down
        Sleep(1.0);
        move_forward(slowSpeed, 12, 5.0); //move back into lever
        Sleep(2.0);
        move_bucket_arm(-4 * armSpeed, 1.0); //move arm back up
        move_bucket_arm(armSpeed, 1.5); //move arm back down little

        //back up from lever
        move_backward(testSpeed, 180, 5.0); //move back from lever
        Sleep(0.25);
        if (RPS.X() < goingDown_x){
            check_x(goingDown_x, PLUS); //check if robot is aligned with ramp
        }
        else {
            check_x(goingDown_x, MINUS); //check if robot is aligned with ramp
        }
    }
    else if(RPS.GetIceCream() == 2)
    {

        //move closer to the levers
        //move_backward(testSpeed, 5, 0.5);

        // Flip twist lever
        right_motor.SetPercent(-testSpeed);
        left_motor.SetPercent(testSpeed);

        while (RPS.Y() < (chocolate_y));

        right_motor.Stop();
        left_motor.Stop();

        Sleep(0.25);
        if (RPS.Y() > chocolate_y) {
            check_y(chocolate_y, PLUS); //check the y coordinate after going down the ramp
        }
        else{
            check_y(chocolate_y, MINUS); //check the y coordinate after going down the ramp
        }

        //move toward ice cream
        turn_right(testSpeed, ninetyDegreeCount + 100);

        Sleep(0.25);

        //Check position in front of chocolate lever
        check_heading(119.0);

        //Keep running until it detects the black line
        right_motor.SetPercent(testSpeed + 10);
        left_motor.SetPercent(-testSpeed - 10);

        bool keepMoving = true;
        while ((left_opt.Value() < 1.5 || middle_opt.Value() < 1.0) && keepMoving){
        

            if (left_opt.Value() >= 1.5 || middle_opt.Value() >= 1.0) {
                right_motor.Stop();
                left_motor.Stop();

                move_forward(slowSpeed, 6, 5.0);
                keepMoving = false;
            }
        }
        Sleep(0.25);

        //Flip the lever
        move_bucket_arm(3 * armSpeed, 1.5); //move arm down
        move_backward(slowSpeed, 10, 5.0); //move back from lever
        Sleep(1.0);
        move_bucket_arm(armSpeed, 0.5); //move arm down
        Sleep(1.0);
        move_forward(slowSpeed, 12, 5.0); //move back into lever
        Sleep(2.0);
        move_bucket_arm(-4 * armSpeed, 1.0); //move arm back up
        move_bucket_arm(armSpeed, 1.5); //move arm back down little

        //back up from lever
        move_backward(testSpeed, 205, 5.0); //move back from lever
        Sleep(0.25);
        if (RPS.X() < goingDown_x){
            check_x(goingDown_x, PLUS); //check if robot is aligned with ramp
        }
        else {
            check_x(goingDown_x, MINUS); //check if robot is aligned with ramp
        }
    }


    /*
    END ICE CREAM LEVER

    START FINAL BUTTON
    */


    //Move down ramp
    turn_right(testSpeed, 90); //turn to face the ramp
    Sleep(0.25);
    check_heading(goingDown_heading); //check the angle of the robot

    //move down ramp
    right_motor.SetPercent(-2 * testSpeed);
    left_motor.SetPercent(2 * testSpeed);

    while (RPS.Y() > (downRamp_y + 3.0));

    right_motor.Stop();
    left_motor.Stop();   

    Sleep(0.25);
    if (RPS.Y() < downRamp_y) {
        check_y(downRamp_y, MINUS); //check the y coordinate after going down the ramp
    }
    else{
        check_y(downRamp_y, PLUS); //check the y coordinate after going down the ramp
    }

    //Final button
    turn_left(testSpeed, 100); //face the final button
    Sleep(1.0);
    check_heading(finalButton_heading); //Check if the robot is aligned with the button
    move_backward(testSpeed + 10, 500, 5.0); //Ram that shit into the button

    //Unreachable code
    Sleep(2.0);

    //Celebrate that the code ran all the way through
    celebrate();

    LCD.WriteLine("Hell yeah");    
}
