/********************************/
/*      Proteus Test Code       */
/*     OSU FEH Spring 2020      */
/*        Drew Phillips         */
/*    02/03/20  Version 3.0.1     */
/********************************/

// AM 02/03/20

/* Include preprocessor directives */
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

AnalogInputPin cds(FEHIO::P0_1);
FEHServo servo(FEHServo::Servo7);
DigitalInputPin micro1(FEHIO::P0_2);
DigitalInputPin micro2(FEHIO::P0_3);
DigitalInputPin micro3(FEHIO::P3_5);
DigitalInputPin micro4(FEHIO::P0_4);
FEHServo arm_servo(FEHServo::Servo3); 
FEHMotor drive_motor1(FEHMotor::Motor0,9.0); 
FEHMotor drive_motor2(FEHMotor::Motor1,9.0); 
AnalogInputPin left_opt(FEHIO::P2_7);
AnalogInputPin middle_opt(FEHIO::P2_0);
AnalogInputPin right_opt(FEHIO::P1_4);
//

int main() {

while (true) {

//Drive at 15% power
    drive_motor1.SetPercent(15);
    drive_motor2.SetPercent(15);
    LCD.Write("Sexy\n");

    //See if the left and middle sensor are off the track
    while (left_opt.Value() <= 1.2 && middle_opt.Value() <= 0.85) {

        //Set the right wheel to go slower
        drive_motor1.SetPercent(15);
        drive_motor2.SetPercent(6.9);

        //Set the right wheel to go slower
        LCD.Write("Going Right\n");

        //Stop moving right if the middle sensor is over the track
        if (middle_opt.Value() >= 2) {
            break;
        }
    }

    //See if the middle and right sensor are off the track
    while (middle_opt.Value() <= 0.85 && right_opt.Value() <= 1.1) {
        drive_motor1.SetPercent(6.9);
        drive_motor2.SetPercent(15);

        LCD.Write("Going Left\n");

        //Stop moving right if the middle sensor is over the track
        if (middle_opt.Value() >= 2) {
            break;
        }
    }

}
}