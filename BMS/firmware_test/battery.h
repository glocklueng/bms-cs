#ifndef BATTERY_H
#define BATTERY_H

#include <SPI.h>
#include <Wire.h>

struct battery_t
{
	float voltage;
	float cell[3];
	uint8_t percentage;
};

#define BATTERY_T_SIZE (4*4+1)

inline void send_battery(const battery_t& bat)
{
  Wire.write((uint8_t*)&bat,BATTERY_T_SIZE);
}

inline battery_t make_battery(float cells[3])
{
  //Create Battery Struct
  battery_t bat;
  bat.voltage=cells[0]+cells[1]+cells[2];
  bat.percentage=(uint8_t)(bat.voltage/12.4*100.0);
  bat.cell[0]=cells[0];
  bat.cell[1]=cells[1];
  bat.cell[2]=cells[2];
  return bat;
}

#endif
