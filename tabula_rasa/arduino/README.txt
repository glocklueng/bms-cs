The idea here is a fully dynamically configurable Arduino firmware.

"action" is used for scheduling object polling.
    For example:  actions_10ms.add(new mySensor);
    (mySensor inherits from action)

"tabula_control" is used to get commands and send sensor values from a single shared buffer 
(FIXME: it should be possible to access these buffers via an efficient binary protocol)
    For example:  tabula_command_value<unsigned char> pwm;
    The command is stored in the command buffer at pwm.get_index();
    The currently commanded value is accessible via pwm.get();

"tabula_config" is used to build the above objects at runtime.
    You register your classes using REGISTER_TABULA_DEVICE,
    giving the setup name and some code to run when the device is created.
    For example, you can connect over serial port and enter:
	analog_sensor A3   (Create an analog input device on pin A3)
	sensors16         (Starts showing sensor values as 16-bit numbers)
	pwm_pin 3         (Create a PWM output device on pin 3)
	cmd 0 200         (Set command index 0 to value 200)
	ramp 0            (Change the PWM command index continously)
	cmd 1 10          (Set rate of change of PWM command)
	loop!             (Done with configuration, enter main loop())


