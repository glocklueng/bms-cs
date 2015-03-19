#include "motor_controller.h"
#include "tabula_config.h"

bts_controller_t::bts_controller_t(uint16_t left_pwms[2],uint16_t right_pwms[2])
{
	for(int16_t ii=0;ii<2;++ii)
	{
		left_pwms_m[ii]=left_pwms[ii];
		right_pwms_m[ii]=right_pwms[ii];
	}
}

void bts_controller_t::setup()
{
	for(int16_t ii=0;ii<2;++ii)
	{
		pinMode(left_pwms_m[ii],OUTPUT);
		pinMode(right_pwms_m[ii],OUTPUT);
		digitalWrite(left_pwms_m[ii],LOW);
		digitalWrite(right_pwms_m[ii],LOW);
	}
}

void bts_controller_t::drive(const int16_t left,const int16_t right)
{
	for(int16_t ii=0;ii<2;++ii)
	{
		digitalWrite(left_pwms_m[ii],LOW);
		digitalWrite(right_pwms_m[ii],LOW);
	}

	analogWrite(left_pwms_m[left>=0],abs(left));
	analogWrite(right_pwms_m[right>=0],abs(right));
}

sabertooth_controller_t::sabertooth_controller_t(Stream& serial):
	serial_m(&serial)
{}

void sabertooth_controller_t::setup()
{}

void sabertooth_controller_t::drive(const int16_t left,const int16_t right)
{
	send_motor_m(128,6,left);
	send_motor_m(128,7,right);
}

sabertooth_v1_controller_t::sabertooth_v1_controller_t(Stream& serial):
	sabertooth_controller_t(serial)
{}

void sabertooth_v1_controller_t::send_motor_m(const uint8_t address,const uint8_t motor,const int16_t value)
{
	uint8_t value_raw=64+(value>>2);

	if(motor==7)
		value_raw+=128;

	serial_m->write(&value_raw,1);
}

sabertooth_v2_controller_t::sabertooth_v2_controller_t(Stream& serial):
	sabertooth_controller_t(serial)
{}

//untested
void sabertooth_v2_controller_t::send_motor_m(const uint8_t address,const uint8_t motor,const int16_t value)
{
	uint8_t value_raw=64+(value>>2);

	uint8_t data[4]=
	{
		address,
		motor,
		value_raw,
		(address+motor+value_raw)&127
	};

	serial_m->write(data,4);
}

/// Register our devices with tabula_setup
REGISTER_TABULA_DEVICE(bts_controller_t, 
	uint16_t left[2]; uint16_t right[2]; 
	left[0]=src.read_pin();	left[1]=src.read_pin();
	right[0]=src.read_pin(); right[1]=src.read_pin();
	actions_10ms.add(new bts_controller_t(left,right));
)

REGISTER_TABULA_DEVICE(sabertooth_v1_controller_t,
	Stream *saber=src.read_serial(9600);
	actions_10ms.add(new sabertooth_v1_controller_t(*saber));
)

REGISTER_TABULA_DEVICE(sabertooth_v2_controller_t,
	Stream *saber=src.read_serial(9600);
	delay(2000); // datasheet: wait 2 seconds after startup
	saber->write(0xAA); // send the "bauding character"
	delay(10); // paranoia
	actions_10ms.add(new sabertooth_v2_controller_t(*saber));
)

