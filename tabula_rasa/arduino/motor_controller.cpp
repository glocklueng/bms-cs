#include "motor_controller.h"

//Totally Wrong...
bts_controller_t::bts_controller_t(const uint16_t left_pwm_pin,const uint16_t right_pwm_pin):
	left_pwm_pin_m(left_pwm_pin),right_pwm_pin_m(right_pwm_pin)
{}

//Totally Wrong...
void bts_controller_t::setup()
{
	pinMode(left_pwm_pin_m,OUTPUT);
	pinMode(right_pwm_pin_m,OUTPUT);
	digitalWrite(left_pwm_pin_m,0);
	digitalWrite(right_pwm_pin_m,0);
}

//Totally Wrong...
void bts_controller_t::drive(const int16_t left_pwm,const int16_t right_pwm)
{
	digitalWrite(left_pwm_pin_m,0);
	digitalWrite(right_pwm_pin_m,0);
	analogWrite(left_pwm_pin_m,left_pwm);
	analogWrite(right_pwm_pin_m,right_pwm);
}