/**
 Convert string object descriptors into actual runnable objects.
This involves registering object "factories", and parsing serial
commands to find the right factory and call it.

The main things most people need to call are the 
REGISTER_TABULA_DEVICE macro, and the tabula_configure_source object.

 Dr. Orion Lawlor, lawlor@alaska.edu, 2015-03-17 (Public Domain)
*/
#ifndef __TABULA_CONFIG_H
#define __TABULA_CONFIG_H

#include <SoftwareSerial.h>
#include <Arduino.h>
#include "action.h"

/**
 This class supplies configuration data such as strings, ints, etc
 to create devices.
 
 Currently it reads these from a serial port in ASCII.
*/
class tabula_configure_source {
	Stream &s; // serial input stream
public:
	tabula_configure_source(Stream &s_) :s(s_), failure(NULL)
	{}

	/**
	 This flag is used for error reporting.
	 If it's not NULL, it's a human-readable string
	 describing what went wrong.
	*/
	const char *failure;
	
	// This is the string we were parsing before the error.
	String failed_value;
	
	/* A parse error happened--store the error internally */
	void failed(const char *failure_,const String &failed_value_) {
		failure=failure_; failed_value=failed_value_;
	}

	/** 
	Read an Arduino String object from this stream,
	typically a serial port connected to the PC.
	Blocks until a delimiter is reached.
	Stops at spaces or commas, and ignores quotes, like csv.
	*/
	String read_string() {
		String ret="";
		while (true) {
			int c=s.read(); // read one char
			if (c==-1) 
			{ // no char to read right now--run actions in the meantime.
				action_loop();
			}
			else if (c=='"' || c=='\'') 
			{ /* ignore quotes (optional in CSV) */ }
			else if (c==',' || c==' ' || c=='\n' || c=='\r') 
			{ /* delimiter */
				if (ret!="")
					return ret; // done with string
				/* else ignore spare delimiters */
			}
			else ret+=(char)c; // add to string
		}
	}

	/**
	Read an integer from this stream object.
	Returns zero if the integer could not be read.
	*/
	long read_int() {
		String str=read_string();
		long ret=str.toInt();
		if (ret==0 && str[0]!='0') failed("Error reading integer",str);
		return ret;
	}

	/**
	Read an integer pin number, for an I/O pin.
	Returns zero if the pin could not be read.
	*/
	long read_pin() {
		String str=read_string(); str.toUpperCase();
		bool is_analog=false;
		if (str[0]=='A') {
			is_analog=true;
			str=str.substring(1); // trim off "A" part
		}
		long pin=str.toInt();
		
		long maxPin=1000, maxAnalog=100;
	#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
		maxPin=53; maxAnalog=16;
	#elif	defined(__AVR_ATmega328P__) || defined(__AVR_ATmega168__)
		maxPin=13; maxAnalog=6;
	#endif
		if (is_analog) {
			if (pin<=maxAnalog) pin+=A0;
			else {failed("Invalid analog pin number",str); return 0;}
		}
		else if (pin<=0 || pin>maxPin) {failed("Invalid pin number",str); return 0;}

		return pin;
	}

	/**
	Read a description of a serial port, 
		and initialize it to this baud rate.
	*/
	Stream *read_serial(long baud=0) {
		String str=read_string(); str.toUpperCase();

	#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	/* Mega has 4 hardware serial ports, like "X1" for Serial1 (RX1/TX1) */
		HardwareSerial *ret=NULL;
		if (str[0]=='X') {
			switch(str[1]) {
			case '1': ret=&Serial1; break;
			case '2': ret=&Serial2; break;
			case '3': ret=&Serial3; break;
			default: break;
			}
		}
		if (ret==NULL) failed("Error reading Arduino Mega serial port (like X1)",str);
	#else 
	/* Arduino UNO?	Use software serial, like "pins 8 9" (pin 8 RX, pin 9 TX) */
		SoftwareSerial *ret=NULL;
		if (str=="PINS") {
			long rx=read_pin();
			long tx=read_pin();
			if (rx!=0 && tx!=0)
				 ret=new SoftwareSerial(rx,tx); // FIXME: how can we ever delete this object?
		}
		else failed("Error reading Arduino Uno software serial port (like 'pins 8 9')",str);
	#endif

		if (ret!=NULL && baud>0)
		{
			ret->begin(baud);
		}
		return ret;
	}

	// Report this sensor value index back up to the control PC.  
	// Name is a short one-word value, like "range".
	// Description is human readable, like "distance in mm from ultrasonic sensor"
	void sensor_index(const char *name,const __FlashStringHelper *description,int index) {
		s.print(F("2 index "));
		s.print(index);
		s.print(F(" sensor "));
		s.print(name);
		s.print(F(": "));
		s.println(description);
	}

	// Report this command value index back up to the control PC.  
	// Name is a short one-word value, like "power".
	// Description is human readable, like "motor power, from -255 to +255"
	void command_index(const char *name,const __FlashStringHelper *description,int index) {
		s.print(F("3 index "));
		s.print(index);
		s.print(F(" command "));
		s.print(name);
		s.print(F(": "));
		s.println(description);
	}
};

/** This factory is used by tabula_setup to create
 instances of your device when requested.  
 These classes are typically singletons,
 and are automatically registered when created.

Use REGISTER_TABULA_DEVICE to create your factory.
*/
class tabula_factory {
public:
	// The machine-readable string name of this device, like "bts_controller_t"
	//   By convention, it's also the name of the class.
	const char *device_name;
	
	tabula_factory(const char *device_name_)
		:device_name(device_name_) 
	{
		register_factory(this);
	}
	virtual ~tabula_factory() {}
	
	/// Create your device based on these configuration parameters.
	virtual void create(tabula_configure_source &src) =0;

	/// Link this factory into our list
	static void register_factory(tabula_factory *f) {
		f->next=head;
		head=f;
	}

	static tabula_factory *head;
	tabula_factory *next;
};

/**
  This macro is used to register your device with tabula_setup.
  Use this pattern at file scope:

REGISTER_TABULA_DEVICE(my_motor_controller,
	// This code can use src, a tabula_configure_source
	int outputPin=src.read_pin();
	actions_10ms.add(new my_motor_controller(outputPin));
)
*/
#define REGISTER_TABULA_DEVICE(name, createCode) \
	class name##_factory : public tabula_factory { \
	public: \
		name##_factory() :tabula_factory(#name) {} \
		virtual void create(tabula_configure_source &src) { createCode ; } \
	}; \
	const static name##_factory name##_factory_singleton;


extern void tabula_setup();

#endif

