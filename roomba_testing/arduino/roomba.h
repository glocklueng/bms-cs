#ifndef ROOMBA_H
#define ROOMBA_H

#if(defined(__AVR))

	#include <Arduino.h>

	class roomba_serial_t
	{
		public:
			roomba_serial_t(const HardwareSerial& serial);
			int available();
			size_t write(const void* buffer,const size_t size);
			size_t read(void* buffer,const size_t size);

		private:
			HardwareSerial* serial_m;
	};

#else

	#include <cctype>
	#include <msl/serial.hpp>
	#include <msl/time.hpp>

	typedef msl::serial_t roomba_serial_t;

#endif

class roomba_t
{
	public:
		enum mode_t
		{
			PASSIVE,
			SAFE,
			FULL
		};

		enum parser_state_t
		{
			HEADER,
			SIZE,
			DATA,
			CHECKSUM
		};

		roomba_t(const roomba_serial_t& serial);

		void start();
		void stop();
		void reset();

		void update();

		void set_mode(const mode_t& mode);

		void drive(const int16_t left,const int16_t right);

		void set_led_check(const bool on);
		void set_led_dock(const bool on);
		void set_led_spot(const bool on);
		void set_led_debris(const bool on);
		void set_led_clean(const uint8_t color,const uint8_t brightness);
		void led_update();

		void set_7_segment(const uint8_t digits[4]);
		void set_7_segment(const char* text);

		void play_song(const uint8_t number);

		void set_receive_sensors(const bool on);

		void dump_sensors() const;

	private:
		roomba_serial_t* serial_m;
		uint8_t leds_m;
		uint8_t led_clean_color_m;
		uint8_t led_clean_brightness_m;
		uint8_t serial_size_m;
		uint8_t* serial_buffer_m;
		uint8_t serial_pointer_m;
		parser_state_t serial_state_m;

		void parse_sensor_packet();

		uint8_t bumper_drop_m;
		uint8_t charge_state_m;
		uint16_t batt_voltage_m;
		int8_t batt_temp_m;
		uint16_t batt_charge_m;
		uint16_t batt_capacity_m;
		uint16_t encoder_l_m;
		uint16_t encoder_r_m;
		uint16_t cliff_l_m;
		uint16_t cliff_fl_m;
		uint16_t cliff_fr_m;
		uint16_t cliff_r_m;
		uint8_t mode_m;
		uint8_t light_field_m;
		uint16_t light_l_m;
		uint16_t light_fl_m;
		uint16_t light_cl_m;
		uint16_t light_cr_m;
		uint16_t light_fr_m;
		uint16_t light_r_m;
};

#endif
