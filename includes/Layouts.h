/*
 * Layouts.h
 * Definition for CAN layouts.
 */

#ifndef __Layouts_h
#define __Layouts_h

#include <stdint.h>
#include "MCP2515_defs.h"
#include "MCP2515.h"

/*
 * Packet_IDs.h
 * Constant definitions for CAN packet IDs.
 */

//TRI88 Packet IDs
#define TRI88_MC_BASE_ADDRESS
#define TRI88_DC_BASE_ADDRESS
#define TRI88_DRIVE_ID TRI88_DC_BASE_ADDRESS + 0x01
#define TRI88_POWER_ID TRI88_DC_BASE_ADDRESS + 0x02
#define TRI88_RESET_ID TRI88_DC_BASE_ADDRESS + 0x03
#define TRI88_STATUS_ID TRI88_MC_BASE_ADDRESS + 0x01
#define TRI88_BUS_MEASURE_ID TRI88_MC_BASE_ADDRESS + 0x02
#define TRI88_VELOCITY_MEASURE_ID TRI88_MC_BASE_ADDRESS + 0x03
#define TRI88_TEMP_MEASURE_ID TRI88_MC_BASE_ADDRESS + 0x0B

// BMS19 TX
#define BMS19_VCSOC_ID 0x6B0
#define BMS19_MinMaxTemp_ID 0x6B1
#define BMS19_BATT_STAT_ID 0x36
#define BMS19_OVERHEAT_PRECHARGE_ID 0x6B2
#define BMS19_STROBE_TRIP_ID 0x6B3
#define BMS19_TRIP_STAT_ID 0x6B4

// MPPT
#define MPPT_REQ_BASEADDRESS 0x710
#define MPPT_ANS_BASEADDRESS 0x770
#define MPPT_LEFT_OFFSET 3
#define MPPT_RIGHT_OFFSET 2
#define MPPT_SUB_OFFSET 1

// mitsuba motor controller
#define MTBA_BASEADDRESS 0x08000000
#define MTBA_REQUEST_COMMAND_REAR_LEFT_ID 0x08F89540
#define MTBA_REQUEST_COMMAND_REAR_RIGHT_ID 0x08F91540
#define MTBA_FRAME0_REAR_LEFT_ID 0x08850225
#define MTBA_FRAME0_REAR_RIGHT_ID 0x08850245
#define MTBA_FRAME1_REAR_LEFT_ID 0x08950225
#define MTBA_FRAME1_REAR_RIGHT_ID 0x08950245
#define MTBA_FRAME2_REAR_LEFT_ID 0x08A50225
#define MTBA_FRAME2_REAR_RIGHT_ID 0x08A50245

// driver controls TX
#define DC_BASEADDRESS 0x500
#define DC_HEARTBEAT_ID DC_BASEADDRESS
#define DC_DRIVE_ID 0x501
#define DC_POWER_ID 0x502
#define DC_RESET_ID 0x503
#define DC_INFO_ID 0x505
#define DC_STATUS_ID 0x506
#define DC_TEMP_0_ID 0x5F0 // Max Temp, Min Temp, Module 1 - 6 Temp
#define DC_TEMP_1_ID 0x5F1 // Module 7 - 14 Temp
#define DC_TEMP_2_ID 0x5F2 // Module 15 - 22 Temp
#define DC_TEMP_3_ID 0x5F3 // Module 23 - 26 Temp

/*
 * Mask ID that specifically work with our SIDs
 * (packet ID's for f0-f5 can be found in Layouts.h)
 */
#define MASK_NONE 0x000000
#define MASK_Sx00 0x000700
#define MASK_Sxx0 0x0007F0
#define MASK_Sxxx 0x0007FF
#define MASK_EID 0x07FFFF

/* Macro for easy masking: shifts the frame to the LSB and applies mask
*	f		CAN Frame
*	mask	Mask to specify length of packet
*	lsb 	The rightmost bit of the value within the frame
*/
#define mask(f, mask, lsb) ((((uint64_t)f) >> (unsigned int)lsb) & (uint64_t)mask)

/* Macro for setting member value within frame, reverse of mask process
*	[f		CAN Frame]
*	[mask	Mask to specify length of packet]
*	[lsb 	The rightmost bit of the value within the frame]
*/
#define makePrtlFrame(f, mask, lsb) ((((uint64_t)f) & (uint64_t)mask) << (unsigned int)lsb)

// Macro for converting between 2 byte big Endian to little Endian and vice versa
#define LBE(f) (((f & 0xff) <<  8u ) | ((f & 0xff00) >> 8u))

// Function
/*
 * Abstract base packet.
 */
class Layout
{
public:
	uint32_t id;

	/*
	 * Creates a Frame object to represent this layout.
	 */
	virtual Frame generate_frame() const;

	String toString() const
	{
		return generate_frame().toString();
	}

protected:
	/*
	 * Fill out the header info for a frame.
	 */
	inline void set_header(Frame &f, byte size = 8) const;
};

/*
 * Wavescuplter 22 Motor Driver TRI88 (2019)
 */
class TRI88_Drive : public Layout
{
  public:
    TRI88_Drive(float v, float c) : velocity(v), current(c) {id = TRI88_DRIVE_ID;}
    TRI88_Drive(const Frame &frame) : velocity(frame.low_f), current(frame.high_f) { id = frame.id; }
    Frame generate_frame() const;

  	uint32_t velocity;
  	uint32_t current;
};

class TRI88_Power : public Layout
{
  public:
    TRI88_Power(float bc) : bus_current(bc) {id = TRI88_POWER_ID;}
    TRI88_Power(const Frame &frame) : bus_current(frame.high_f) { id = frame.id; }
    Frame generate_frame() const;

    uint32_t bus_current;
};

class TRI88_Reset : public Layout
{
public:
	TRI88_Reset(void) {id = TRI88_RESET_ID;}
	TRI88_Reset(const Frame &frame) { id = frame.id; }

	Frame generate_frame() const;
};

class TRI88_Status : public Layout
{
public:
  TRI88_Status(uint8_t lf, uint8_t ef, uint8_t am, uint8_t tec, uint8_t rec): limit_flags(lf), error_flags(ef) {id = TRI88_STATUS_ID;}
  TRI88_Status(const Frame &frame) : limit_flags(frame.data[1]), error_flags(frame.data[3]) { id = frame.id; }
  Frame generate_frame() const;

  uint8_t limit_flags, error_flags;
};

class TRI88_Bus_Measure : public Layout
{
public:
  TRI88_Bus_Measure(float bc, float bv) : bus_current_drawn(bc), bus_voltage(bv) {id = TRI88_BUS_MEASURE_ID;}
  TRI88_Bus_Measure(const Frame &frame) : bus_current_drawn(frame.high_f), bus_voltage(frame.low_f) {id = frame.id;}
  Frame generate_frame() const;

  uint32_t bus_current_drawn;
  uint32_t bus_voltage;
};

class TRI88_Velocity_Measure : public Layout
{
  public:
    TRI88_Velocity_Measure(float vv, float mv): vehicle_velocity(vv), motor_velocity(mv) {id = TRI88_VELOCITY_MEASURE_ID;}
    TRI88_Velocity_Measure(const Frame &frame): vehicle_velocity(frame.high_f), motor_velocity(frame.low_f) {id = frame.id;}
    Frame generate_frame() const;

    uint32_t vehicle_velocity;
    uint32_t motor_velocity;
};

class TRI88_Temp_Measure : public Layout
{
  public:
    TRI88_Temp_Measure(float hst, float mt): heat_sink_temp(hst), motor_temp(mt) {id = TRI88_TEMP_MEASURE_ID;}
    TRI88_Temp_Measure(const Frame &frame): heat_sink_temp(frame.high_f), motor_temp(frame.low_f) {id = frame.id;}
    Frame generate_frame() const;

    uint32_t heat_sink_temp;
    uint32_t motor_temp;
};


/*
 * BMS Voltage, Current, and SOC packet (2019 BMS)
 * Note that 2 byte values on the BMS uses Big Endian by default, so conversion necessary
 */
class BMS19_VCSOC : public Layout
{
public:
	BMS19_VCSOC(uint16_t v, uint16_t i, uint8_t soc) : voltage(v),
													   current(i),
													   packSOC(soc)
	{
		id = BMS19_VCSOC_ID;
	}
	BMS19_VCSOC(const Frame &frame) : voltage(LBE(frame.s1)),
									  current(LBE(frame.s0)),
									  packSOC(frame.data[4])
	{
		id = frame.id;
	}

	Frame generate_frame() const;

	uint16_t voltage;
	uint16_t current;
	uint8_t packSOC;
};




/*
* Battery Array Max and Min Temperature (2019 BMS)
*/
class BMS19_MinMaxTemp : public Layout
{
public:
	BMS19_MinMaxTemp(uint8_t minT, uint8_t maxT) : minTemp(minT), maxTemp(maxT)
	{
		id = BMS19_MinMaxTemp_ID;
	}
	BMS19_MinMaxTemp(const Frame &frame) : minTemp(frame.data[4]), maxTemp(frame.data[5])
	{
		id = frame.id;
	}

	Frame generate_frame() const;

	uint8_t minTemp;
	uint8_t maxTemp;
};

//MITSUBA
class MTBA_ReqCommRLeft : public Layout
{
public:
	MTBA_ReqCommRLeft(uint8_t f0_req) : frame0_request(f0_req)
	{
		id = MTBA_REQUEST_COMMAND_REAR_LEFT_ID;
	}
	MTBA_ReqCommRLeft(const Frame &frame) : frame0_request(frame.s0)
	{
		id = frame.id;
	}
	Frame generate_frame() const;

	uint8_t frame0_request;
};
class MTBA_ReqCommRRight : public Layout
{
public:
	MTBA_ReqCommRRight(uint8_t f0_req) : frame0_request(f0_req)
	{
		id = MTBA_REQUEST_COMMAND_REAR_RIGHT_ID;
	}
	MTBA_ReqCommRRight(const Frame &frame) : frame0_request(frame.s0)
	{
		id = frame.id;
	}
	Frame generate_frame() const;

	uint8_t frame0_request;
};
class MTBA_F0_RLeft : public Layout
{
public:
	MTBA_F0_RLeft(uint16_t bv, uint16_t bc, bool bcd, uint8_t mcpa, uint16_t ft, uint16_t mrs, uint16_t pd, uint8_t la) : battery_voltage(bv),			  // 10 bits
																														  battery_current(bc),			  // 9 bits
																														  battery_current_direction(bcd), // 1 bit
																														  motor_current_peak_avg(mcpa),   // 10 bits
																														  fet_temperature(ft),			  // 5 bits
																														  motor_rotating_speed(mrs),	  // 12 bits
																														  pwm_duty(pd),					  // 10 bits
																														  lead_angle(la)				  // 7 bits
	{
		id = MTBA_FRAME0_REAR_LEFT_ID;
	}
	MTBA_F0_RLeft(const Frame &frame) : battery_voltage(mask(frame.value, MASK_MTBA_BATT_VOLT, MTBA_BATT_VOLT_LSB)),
										battery_current(mask(frame.value, MASK_MTBA_BATT_CURR, MTBA_BATT_CURR_LSB)),
										battery_current_direction(mask(frame.value, MASK_MTBA_BATT_CURR_DIREC, MTBA_BATT_CURR_DIREC_LSB)),
										motor_current_peak_avg(mask(frame.value, MASK_MTBA_MOTOR_CURR, MTBA_MOTOR_CURR_LSB)),
										fet_temperature(mask(frame.value, MASK_MTBA_FET_TEMP, MTBA_FET_TEMP_LSB)),
										motor_rotating_speed(mask(frame.value, MASK_MTBA_MOTOR_SPEED, MTBA_MOTOR_SPEED_LSB)),
										pwm_duty(mask(frame.value, MASK_MTBA_PWM_DUTY, MTBA_PWM_DUTY_LSB)),
										lead_angle(mask(frame.value, MASK_MTBA_LEAD_ANGLE, MTBA_LEAD_ANGLE_LSB))
	{
		id = frame.id;
	}

	Frame generate_frame() const;

	uint16_t battery_voltage;
	uint16_t battery_current;
	bool battery_current_direction;
	uint8_t motor_current_peak_avg;
	uint16_t fet_temperature;
	uint16_t motor_rotating_speed;
	uint16_t pwm_duty;
	uint8_t lead_angle;

private:
	/*
	* Apply these masks after shifting frame to LSB
	* MTBA Can packets don't follow bytes, so alot of masking is necessary
	*/
	static const uint16_t MASK_MTBA_BATT_VOLT = 0x3FF;
	static const uint16_t MASK_MTBA_BATT_CURR = 0x1FF;
	static const uint16_t MASK_MTBA_BATT_CURR_DIREC = 0x01;
	static const uint16_t MASK_MTBA_MOTOR_CURR = 0x3FF;
	static const uint16_t MASK_MTBA_FET_TEMP = 0x1F;
	static const uint16_t MASK_MTBA_MOTOR_SPEED = 0x3FF;
	static const uint16_t MASK_MTBA_PWM_DUTY = 0x3FF;
	static const uint16_t MASK_MTBA_LEAD_ANGLE = 0x7F;

	// The position right-most bit (LSB) after masking
	static const int MTBA_BATT_VOLT_LSB = 0;
	static const int MTBA_BATT_CURR_LSB = 10;
	static const int MTBA_BATT_CURR_DIREC_LSB = 19;
	static const int MTBA_MOTOR_CURR_LSB = 20;
	static const int MTBA_FET_TEMP_LSB = 30;
	static const int MTBA_MOTOR_SPEED_LSB = 35;
	static const int MTBA_PWM_DUTY_LSB = 47;
	static const int MTBA_LEAD_ANGLE_LSB = 57;
};
class MTBA_F0_RRight : public Layout
{
public:
	MTBA_F0_RRight(uint16_t bv, uint16_t bc, bool bcd, uint8_t mcpa, uint16_t ft, uint16_t mrs, uint16_t pd, uint8_t la) : battery_voltage(bv),			   // 10 bits
																														   battery_current(bc),			   // 9 bits
																														   battery_current_direction(bcd), // 1 bit
																														   motor_current_peak_avg(mcpa),   // 10 bits
																														   fet_temperature(ft),			   // 5 bits
																														   motor_rotating_speed(mrs),	  // 12 bits
																														   pwm_duty(pd),				   // 10 bits
																														   lead_angle(la)				   // 7 bits
	{
		id = MTBA_FRAME0_REAR_RIGHT_ID;
	}
	MTBA_F0_RRight(const Frame &frame) : battery_voltage(mask(frame.value, MASK_MTBA_BATT_VOLT, MTBA_BATT_VOLT_LSB)),
										 battery_current(mask(frame.value, MASK_MTBA_BATT_CURR, MTBA_BATT_CURR_LSB)),
										 battery_current_direction(mask(frame.value, MASK_MTBA_BATT_CURR_DIREC, MTBA_BATT_CURR_DIREC_LSB)),
										 motor_current_peak_avg(mask(frame.value, MASK_MTBA_MOTOR_CURR, MTBA_MOTOR_CURR_LSB)),
										 fet_temperature(mask(frame.value, MASK_MTBA_FET_TEMP, MTBA_FET_TEMP_LSB)),
										 motor_rotating_speed(mask(frame.value, MASK_MTBA_MOTOR_SPEED, MTBA_MOTOR_SPEED_LSB)),
										 pwm_duty(mask(frame.value, MASK_MTBA_PWM_DUTY, MTBA_PWM_DUTY_LSB)),
										 lead_angle(mask(frame.value, MASK_MTBA_LEAD_ANGLE, MTBA_LEAD_ANGLE_LSB))
	{
		id = frame.id;
	}

	Frame generate_frame() const;

	uint16_t battery_voltage;
	uint16_t battery_current;
	bool battery_current_direction;
	uint8_t motor_current_peak_avg;
	uint16_t fet_temperature;
	uint16_t motor_rotating_speed;
	uint16_t pwm_duty;
	uint8_t lead_angle;

private:
	/*
	* Apply these masks after shifting frame to LSB
	* MTBA Can packets don't follow bytes, so alot of masking is necessary
	*/
	static const uint16_t MASK_MTBA_BATT_VOLT = 0x3FF;
	static const uint16_t MASK_MTBA_BATT_CURR = 0x1FF;
	static const uint16_t MASK_MTBA_BATT_CURR_DIREC = 0x01;
	static const uint16_t MASK_MTBA_MOTOR_CURR = 0x3FF;
	static const uint16_t MASK_MTBA_FET_TEMP = 0x1F;
	static const uint16_t MASK_MTBA_MOTOR_SPEED = 0x3FF;
	static const uint16_t MASK_MTBA_PWM_DUTY = 0x3FF;
	static const uint16_t MASK_MTBA_LEAD_ANGLE = 0x7F;

	// The position right-most bit (LSB) after masking
	static const int MTBA_BATT_VOLT_LSB = 0;
	static const int MTBA_BATT_CURR_LSB = 10;
	static const int MTBA_BATT_CURR_DIREC_LSB = 19;
	static const int MTBA_MOTOR_CURR_LSB = 20;
	static const int MTBA_FET_TEMP_LSB = 30;
	static const int MTBA_MOTOR_SPEED_LSB = 35;
	static const int MTBA_PWM_DUTY_LSB = 47;
	static const int MTBA_LEAD_ANGLE_LSB = 57;
};
class MTBA_F1_RRight : public Layout
{
public:
	MTBA_F1_RRight(bool pm, bool mcm, uint16_t ap, uint16_t rvp, uint16_t dsp, uint16_t otv, uint16_t das, bool rs) : power_mode(pm),
																													  motor_control_mode(mcm),
																													  accelerator_position(ap),
																													  regeneration_vr_position(rvp),
																													  digit_sw_position(dsp),
																													  output_target_value(otv),
																													  drive_action_status(das),
																													  regeneration_status(rs)
	{
		id = MTBA_FRAME1_REAR_RIGHT_ID;
	}
	MTBA_F1_RRight(const Frame &frame) : power_mode(mask(frame.value, MASK_MTBA_POWER_MODE, MTBA_POWER_MODE_LSB)),
										 motor_control_mode(mask(frame.value, MASK_MTBA_MODE_CONTROL_MODE, MTBA_MODE_CONTROL_MODE_LSB)),
										 accelerator_position(mask(frame.value, MASK_MTBA_ACCELERATOR_POSITION, MTBA_ACCELERATOR_POSITION_LSB)),
										 regeneration_vr_position(mask(frame.value, MASK_MTBA_REGENERATION_VR_POSITION, MTBA_REGENERATION_VR_POSITION_LSB)),
										 digit_sw_position(mask(frame.value, MASK_MTBA_DIGITAL_SW_POSITION, MTBA_DIGITAL_SW_POSITION_LSB)),
										 output_target_value(mask(frame.value, MASK_MTBA_OUTPUT_TARGET_VALUE, MTBA_OUTPUT_TARGET_VALUE_LSB)),
										 drive_action_status(mask(frame.value, MASK_MTBA_DRIVE_ACTION_STATUS, MTBA_DRIVE_ACTION_STATUS_LSB)),
										 regeneration_status(mask(frame.value, MASK_MTBA_REGENERATION_STATUS, MTBA_REGENERATION_STATUS_LSB))
	{
		id = frame.id;
	}

	Frame generate_frame() const;

	bool power_mode;				   // 1 bit
	bool motor_control_mode;		   // 1 bit
	uint16_t accelerator_position;	 // 10 bits
	uint16_t regeneration_vr_position; // 10 bits
	uint8_t digit_sw_position;		   // 4 bits
	uint16_t output_target_value;	  // 10 bits
	uint8_t drive_action_status;	   // 2 bits
	bool regeneration_status;		   // 1 bit
private:
	/*
	* Apply these masks after shifting frame to LSB
	* MTBA Can packets don't follow bytes, so alot of masking is necessary
	*/
	static const uint16_t MASK_MTBA_POWER_MODE = 0x01;
	static const uint16_t MASK_MTBA_MODE_CONTROL_MODE = 0x01;
	static const uint16_t MASK_MTBA_ACCELERATOR_POSITION = 0x3FF;
	static const uint16_t MASK_MTBA_REGENERATION_VR_POSITION = 0x3FF;
	static const uint16_t MASK_MTBA_DIGITAL_SW_POSITION = 0x0F;
	static const uint16_t MASK_MTBA_OUTPUT_TARGET_VALUE = 0x3FF;
	static const uint16_t MASK_MTBA_DRIVE_ACTION_STATUS = 0x03;
	static const uint16_t MASK_MTBA_REGENERATION_STATUS = 0x01;

	// The position right-most bit (LSB) after masking
	static const int MTBA_POWER_MODE_LSB = 0;
	static const int MTBA_MODE_CONTROL_MODE_LSB = 1;
	static const int MTBA_ACCELERATOR_POSITION_LSB = 2;
	static const int MTBA_REGENERATION_VR_POSITION_LSB = 12;
	static const int MTBA_DIGITAL_SW_POSITION_LSB = 22;
	static const int MTBA_OUTPUT_TARGET_VALUE_LSB = 26;
	static const int MTBA_DRIVE_ACTION_STATUS_LSB = 36;
	static const int MTBA_REGENERATION_STATUS_LSB = 38;
};
class MTBA_F1_RLeft : public Layout
{
public:
	MTBA_F1_RLeft(bool pm, bool mcm, uint16_t ap, uint16_t rvp, uint16_t dsp, uint16_t otv, uint16_t das, bool rs) : power_mode(pm),
																													 motor_control_mode(mcm),
																													 accelerator_position(ap),
																													 regeneration_vr_position(rvp),
																													 digit_sw_position(dsp),
																													 output_target_value(otv),
																													 drive_action_status(das),
																													 regeneration_status(rs)
	{
		id = MTBA_FRAME1_REAR_LEFT_ID;
	}
	MTBA_F1_RLeft(const Frame &frame) : power_mode(mask(frame.value, MASK_MTBA_POWER_MODE, MTBA_POWER_MODE_LSB)),
										motor_control_mode(mask(frame.value, MASK_MTBA_MODE_CONTROL_MODE, MTBA_MODE_CONTROL_MODE_LSB)),
										accelerator_position(mask(frame.value, MASK_MTBA_ACCELERATOR_POSITION, MTBA_ACCELERATOR_POSITION_LSB)),
										regeneration_vr_position(mask(frame.value, MASK_MTBA_REGENERATION_VR_POSITION, MTBA_REGENERATION_VR_POSITION_LSB)),
										digit_sw_position(mask(frame.value, MASK_MTBA_DIGITAL_SW_POSITION, MTBA_DIGITAL_SW_POSITION_LSB)),
										output_target_value(mask(frame.value, MASK_MTBA_OUTPUT_TARGET_VALUE, MTBA_OUTPUT_TARGET_VALUE_LSB)),
										drive_action_status(mask(frame.value, MASK_MTBA_DRIVE_ACTION_STATUS, MTBA_DRIVE_ACTION_STATUS_LSB)),
										regeneration_status(mask(frame.value, MASK_MTBA_REGENERATION_STATUS, MTBA_REGENERATION_STATUS_LSB))
	{
		id = frame.id;
	}

	Frame generate_frame() const;

	bool power_mode;				   // 1 bit
	bool motor_control_mode;		   // 1 bit
	uint16_t accelerator_position;	 // 10 bits
	uint16_t regeneration_vr_position; // 10 bits
	uint8_t digit_sw_position;		   // 4 bits
	uint16_t output_target_value;	  // 10 bits
	uint8_t drive_action_status;	   // 2 bits
	bool regeneration_status;		   // 1 bit
private:
	/*
	* Apply these masks after shifting frame to LSB
	* MTBA Can packets don't follow bytes, so alot of masking is necessary
	*/
	static const uint16_t MASK_MTBA_POWER_MODE = 0x01;
	static const uint16_t MASK_MTBA_MODE_CONTROL_MODE = 0x01;
	static const uint16_t MASK_MTBA_ACCELERATOR_POSITION = 0x3FF;
	static const uint16_t MASK_MTBA_REGENERATION_VR_POSITION = 0x3FF;
	static const uint16_t MASK_MTBA_DIGITAL_SW_POSITION = 0x0F;
	static const uint16_t MASK_MTBA_OUTPUT_TARGET_VALUE = 0x3FF;
	static const uint16_t MASK_MTBA_DRIVE_ACTION_STATUS = 0x03;
	static const uint16_t MASK_MTBA_REGENERATION_STATUS = 0x01;

	// The position right-most bit (LSB) after masking
	static const int MTBA_POWER_MODE_LSB = 0;
	static const int MTBA_MODE_CONTROL_MODE_LSB = 1;
	static const int MTBA_ACCELERATOR_POSITION_LSB = 2;
	static const int MTBA_REGENERATION_VR_POSITION_LSB = 12;
	static const int MTBA_DIGITAL_SW_POSITION_LSB = 22;
	static const int MTBA_OUTPUT_TARGET_VALUE_LSB = 26;
	static const int MTBA_DRIVE_ACTION_STATUS_LSB = 36;
	static const int MTBA_REGENERATION_STATUS_LSB = 38;
};

class MTBA_F2_RLeft : public Layout
{
public:
	MTBA_F2_RLeft(uint16_t adErr, uint8_t psErr, uint8_t msErr, uint8_t fetOHErr) : ADSensorErr(adErr),
																					powerSysErr(psErr),
																					motorSysErr(msErr),
																					FETOverHeatErr(fetOHErr)
	{
		id = MTBA_FRAME1_REAR_LEFT_ID;
	}
	MTBA_F2_RLeft(const Frame &frame) : ADSensorErr(mask(frame.value, MASK_MTBA_AD_SENS_ERR, MTBA_AD_SENS_ERR_LSB)),
										powerSysErr(mask(frame.value, MASK_MTBA_POWER_SYS_ERR, MTBA_POWER_SYS_ERR_LSB)),
										motorSysErr(mask(frame.value, MASK_MTBA_MOTOR_SYS_ERR, MTBA_MOTOR_SYS_ERR_LSB)),
										FETOverHeatErr(mask(frame.value, MASK_MTBA_FET_OVER_HEAT_ERR, MTBA_FET_OVER_HEAT_ERR_LSB))
	{
		id = frame.id;
	}

	Frame generate_frame() const;

	uint16_t ADSensorErr;
	uint8_t powerSysErr;
	uint8_t motorSysErr;
	uint8_t FETOverHeatErr;

private:
	/*
	* Apply these masks after shifting frame to LSB
	* MTBA Can packets don't follow bytes, so alot of masking is necessary
	*/
	static const uint16_t MASK_MTBA_AD_SENS_ERR = 0xFFFF;
	static const uint16_t MASK_MTBA_POWER_SYS_ERR = 0xFF;
	static const uint16_t MASK_MTBA_MOTOR_SYS_ERR = 0xFF;
	static const uint16_t MASK_MTBA_FET_OVER_HEAT_ERR = 0x03;

	// The position right-most bit (LSB) after masking
	static const int MTBA_AD_SENS_ERR_LSB = 0;
	static const int MTBA_POWER_SYS_ERR_LSB = 16;
	static const int MTBA_MOTOR_SYS_ERR_LSB = 24;
	static const int MTBA_FET_OVER_HEAT_ERR_LSB = 32;
};

class MTBA_F2_RRight : public Layout
{
public:
	MTBA_F2_RRight(uint16_t adErr, uint8_t psErr, uint8_t msErr, uint8_t fetOHErr) : ADSensorErr(adErr),
																					 powerSysErr(psErr),
																					 motorSysErr(msErr),
																					 FETOverHeatErr(fetOHErr)
	{
		id = MTBA_FRAME1_REAR_RIGHT_ID;
	}
	MTBA_F2_RRight(const Frame &frame) : ADSensorErr(mask(frame.value, MASK_MTBA_AD_SENS_ERR, MTBA_AD_SENS_ERR_LSB)),
										 powerSysErr(mask(frame.value, MASK_MTBA_POWER_SYS_ERR, MTBA_POWER_SYS_ERR_LSB)),
										 motorSysErr(mask(frame.value, MASK_MTBA_MOTOR_SYS_ERR, MTBA_MOTOR_SYS_ERR_LSB)),
										 FETOverHeatErr(mask(frame.value, MASK_MTBA_FET_OVER_HEAT_ERR, MTBA_FET_OVER_HEAT_ERR_LSB))
	{
		id = frame.id;
	}

	Frame generate_frame() const;

	uint16_t ADSensorErr;
	uint8_t powerSysErr;
	uint8_t motorSysErr;
	uint8_t FETOverHeatErr;

private:
	/*
	* Apply these masks after shifting frame to LSB
	* MTBA Can packets don't follow bytes, so alot of masking is necessary
	*/
	static const uint16_t MASK_MTBA_AD_SENS_ERR = 0xFFFF;
	static const uint16_t MASK_MTBA_POWER_SYS_ERR = 0xFF;
	static const uint16_t MASK_MTBA_MOTOR_SYS_ERR = 0xFF;
	static const uint16_t MASK_MTBA_FET_OVER_HEAT_ERR = 0x03;

	// The position right-most bit (LSB) after masking
	static const int MTBA_AD_SENS_ERR_LSB = 0;
	static const int MTBA_POWER_SYS_ERR_LSB = 16;
	static const int MTBA_MOTOR_SYS_ERR_LSB = 24;
	static const int MTBA_FET_OVER_HEAT_ERR_LSB = 32;
};

/*
 * Driver controls heartbeat packet.
 */
class DC_Heartbeat : public Layout
{
public:
	DC_Heartbeat(uint32_t d_id, uint32_t s_no) : dc_id(d_id), serial_no(s_no) { id = DC_HEARTBEAT_ID; }
	DC_Heartbeat(Frame &frame) : dc_id(frame.low), serial_no(frame.high) { id = frame.id; }

	Frame generate_frame() const;

	uint32_t dc_id;
	uint32_t serial_no;
};

/*
 * Driver controls drive command packet.
 */
class DC_Drive : public Layout
{
public:
	DC_Drive(float v, float c) : velocity(v), current(c) { id = DC_DRIVE_ID; }
	DC_Drive(const Frame &frame) : velocity(frame.low_f), current(frame.high_f) { id = frame.id; }

	Frame generate_frame() const;

	float velocity;
	float current;
};

/*
 * Driver controls power command packet.
 */
class DC_Power : public Layout
{
public:
	DC_Power(float bc) : bus_current(bc) { id = DC_POWER_ID; }
	DC_Power(const Frame &frame) : bus_current(frame.high_f) { id = frame.id; }

	Frame generate_frame() const;

	float bus_current;
};

/*
 * Driver controls reset packet.
 */
class DC_Reset : public Layout
{
public:
	DC_Reset() { id = DC_RESET_ID; }
	DC_Reset(const Frame &frame) { id = frame.id; }

	Frame generate_frame() const;
};

/*
 * Driver controls information packet.
 */
class DC_Info : public Layout
{
public:
	DC_Info(float accel,
			float regen,
			bool brake,
			uint16_t can_errors,
			byte dc_errors,
			bool reset,
			bool fuel,
			byte current_gear,
			uint16_t ignition,
			bool tripcondition)
	{

		accel_ratio = accel;
		regen_ratio = regen;
		brake_engaged = brake;
		can_error_flags = can_errors;
		dc_error_flags = dc_errors;
		was_reset = reset;
		gear = current_gear;
		ignition_state = ignition;
		fuel_door = fuel;
		tripped = tripcondition;

		id = DC_INFO_ID;
	}

	DC_Info(const Frame &frame)
	{
		// bytes 0, 1 (ignition switch, fuel door)
		ignition_state = frame.s0 & 0x0070;
		fuel_door = (bool)(frame.s0 & 0x0100);

		// byte 2
		accel_ratio = frame.data[2] / 100.0f;

		// byte 3
		regen_ratio = frame.data[3] / 100.0f;

		// byte 4 + 5
		can_error_flags = frame.s2;

		// byte 6
		dc_error_flags = frame.data[6];

		// byte 7 (status flags)
		gear = frame.data[7] & 0x0F;
		brake_engaged = (bool)(frame.data[7] & 0x10);
		was_reset = (bool)(frame.data[7] & 0x20);
		tripped = (bool)(frame.data[7] & 0x40);

		id = frame.id;
	}

	float accel_ratio, regen_ratio; // these will be stored as integers 0-100 in frame
	uint16_t can_error_flags;
	byte dc_error_flags, gear;
	bool brake_engaged, was_reset, fuel_door;
	uint16_t ignition_state;
	bool tripped;

	Frame generate_frame() const;
};

/*
 * Driver controls status packet.
 */
class DC_Status : public Layout
{
public:
	DC_Status(uint32_t flags)
	{

		this->flags = flags;

		id = DC_STATUS_ID;
	}

	DC_Status(const Frame &frame)
	{
		// bytes 0, 1
		flags = frame.low;

		id = frame.id;
	}

	uint32_t flags;

	static const uint8_t F_CHARGING_OVER_TEMP = 0x01;
	static const uint8_t F_DISCHARGING_OVER_TEMP = 0x02;
	static const uint8_t F_CHARGING_OVER_CURRENT = 0x04;
	static const uint8_t F_DISCHARGING_OVER_CURRENT = 0x08;
	static const uint8_t F_NO_TRIP = 0x00;

	// 0x80 also defined by driver controls as the flag for a BMS-controlled trip

	Frame generate_frame() const;
};

/* Temperature Sensor Data Packets
   Packet 1: Min, Max, and Modules 1 - 6
   Packet 2: Modules 7 - 14
   Packet 3: Modules 15 - 22
   Packet 4: Modules 23 - 30 */

class DC_Temp_0 : public Layout
{
public:
	DC_Temp_0(uint8_t maxT, uint8_t avgT, uint8_t T1, uint8_t T2, uint8_t T3, uint8_t T4, uint8_t T5, uint8_t T6)
	{

		max_temp = maxT;
		avg_temp = avgT;
		temp[1] = T1;
		temp[2] = T2;
		temp[3] = T3;
		temp[4] = T4;
		temp[5] = T5;
		temp[6] = T6;

		id = DC_TEMP_0_ID;
	}

	DC_Temp_0(uint8_t maxT, uint8_t avgT, uint8_t *_temps)
	{

		max_temp = maxT;
		avg_temp = avgT;
		memcpy(temp + 1, _temps, 6);

		id = DC_TEMP_0_ID;
	}

	DC_Temp_0(const Frame &frame)
	{
		max_temp = frame.data[0];
		avg_temp = frame.data[1];
		temp[1] = frame.data[2];
		temp[2] = frame.data[3];
		temp[3] = frame.data[4];
		temp[4] = frame.data[5];
		temp[5] = frame.data[6];
		temp[6] = frame.data[7];
	}

	uint8_t max_temp, avg_temp;
	uint8_t temp[7];

	Frame generate_frame() const;
};

class DC_Temp_1 : public Layout
{
public:
	DC_Temp_1(uint8_t T7, uint8_t T8, uint8_t T9, uint8_t T10, uint8_t T11, uint8_t T12, uint8_t T13, uint8_t T14)
	{

		temp[1] = T7;
		temp[2] = T8;
		temp[3] = T9;
		temp[4] = T10;
		temp[5] = T11;
		temp[6] = T12;
		temp[7] = T13;
		temp[8] = T13;

		id = DC_TEMP_1_ID;
	}

	DC_Temp_1(uint8_t *_temps)
	{

		memcpy(temp + 1, _temps, 8);

		id = DC_TEMP_1_ID;
	}

	DC_Temp_1(const Frame &frame)
	{
		temp[1] = frame.data[0];
		temp[2] = frame.data[1];
		temp[3] = frame.data[2];
		temp[4] = frame.data[3];
		temp[5] = frame.data[4];
		temp[6] = frame.data[5];
		temp[7] = frame.data[6];
		temp[8] = frame.data[7];
	}

	uint8_t temp[9];

	Frame generate_frame() const;
};

class DC_Temp_2 : public Layout
{
public:
	DC_Temp_2(uint8_t T15, uint8_t T16, uint8_t T17, uint8_t T18, uint8_t T19, uint8_t T20, uint8_t T21, uint8_t T22)
	{

		temp[1] = T15;
		temp[2] = T16;
		temp[3] = T17;
		temp[4] = T18;
		temp[5] = T19;
		temp[6] = T20;
		temp[7] = T21;
		temp[8] = T22;

		id = DC_TEMP_2_ID;
	}

	DC_Temp_2(uint8_t *_temps)
	{

		memcpy(temp + 1, _temps, 8);

		id = DC_TEMP_2_ID;
	}

	DC_Temp_2(const Frame &frame)
	{
		temp[1] = frame.data[0];
		temp[2] = frame.data[1];
		temp[3] = frame.data[2];
		temp[4] = frame.data[3];
		temp[5] = frame.data[4];
		temp[6] = frame.data[5];
		temp[7] = frame.data[6];
		temp[8] = frame.data[7];
	}

	uint8_t temp[9];

	Frame generate_frame() const;
};

class DC_Temp_3 : public Layout
{
public:
	DC_Temp_3(uint8_t T23, uint8_t T24, uint8_t T25, uint8_t T26, uint8_t T27, uint8_t T28, uint8_t T29, uint8_t T30)
	{

		temp[1] = T23;
		temp[2] = T24;
		temp[3] = T25;
		temp[4] = T26;
		temp[5] = T27;
		temp[6] = T28;
		temp[7] = T29;
		temp[8] = T30;

		id = DC_TEMP_3_ID;
	}

	DC_Temp_3(uint8_t *_temps)
	{

		memcpy(temp + 1, _temps, 8);

		id = DC_TEMP_3_ID;
	}

	DC_Temp_3(const Frame &frame)
	{
		temp[1] = frame.data[0];
		temp[2] = frame.data[1];
		temp[3] = frame.data[2];
		temp[4] = frame.data[3];
		temp[5] = frame.data[4];
		temp[6] = frame.data[5];
		temp[7] = frame.data[6];
		temp[8] = frame.data[7];
	}

	uint8_t temp[9];

	Frame generate_frame() const;
};


/*
* BMS battery status packet
*	Cycles through each battery cell with the same CAN ID
*/
class BMS19_Batt_Stat : public Layout
{
public:
	BMS19_Batt_Stat(uint8_t _ci, uint16_t _instv, uint16_t _intR, bool _sh, uint16_t _ocv) : cellID(_ci),
																							 instVolt(_instv),
																							 intR(_intR),
																							 shunt(_sh),
																							 ocVolt(_ocv)
	{
		id = BMS19_BATT_STAT_ID;
	}
	BMS19_Batt_Stat(const Frame &frame) : cellID(mask(frame.value, MASK_CELL_ID, CELL_ID_LSB)),
										  instVolt(mask(frame.value, MASK_INST_VOLT, INST_VOLT_LSB)),
										  intR(mask(frame.value, MASK_INT_RESIS, INT_RESIS_LSB)),
										  shunt(mask(frame.value, MASK_SHUNT, SHUNT_LSB)),
										  ocVolt(mask(frame.value, MASK_OC_VOLT, OC_VOLT_LSB))
	{
		id = frame.id;
	}

	Frame generate_frame() const;

	uint8_t cellID;
	uint16_t instVolt;
	uint16_t intR;
	bool shunt;
	uint16_t ocVolt;

private:
	static const uint16_t MASK_CELL_ID = 0xFF;
	static const uint16_t MASK_INST_VOLT = 0xFFFF;
	static const uint16_t MASK_INT_RESIS = 0x7FFF;
	static const uint16_t MASK_SHUNT = 0x01;
	static const uint16_t MASK_OC_VOLT = 0xFFFF;

	static const int CELL_ID_LSB = 0;
	static const int INST_VOLT_LSB = 8;
	static const int INT_RESIS_LSB = 25;
	static const int SHUNT_LSB = 24;
	static const int OC_VOLT_LSB = 40;
};

class BMS19_Overheat_Precharge : public Layout
{
public:
	BMS19_Overheat_Precharge(bool _overTempLimit, bool _precharge) : overTempLimit(_overTempLimit),
																	 precharge(_precharge)
	{
		id = BMS19_OVERHEAT_PRECHARGE_ID;
	}
	BMS19_Overheat_Precharge(const Frame &frame) : overTempLimit((frame.value >> OVERTEMPLIMIT_LSB) & 1u),
												   precharge((frame.value >> PRECHARGE_LSB) & 1u)
	{
		id = frame.id;
	}

	bool overTempLimit;
	bool precharge;

	Frame generate_frame() const;

private:
	static const int OVERTEMPLIMIT_LSB = 0;
	static const int PRECHARGE_LSB = 8;
};

class BMS19_Strobe_Trip : public Layout
{
public:
	BMS19_Strobe_Trip(bool _strobeTrip) : strobeTrip(_strobeTrip)
	{
		id = BMS19_STROBE_TRIP_ID;
	}
	BMS19_Strobe_Trip(const Frame &frame) : strobeTrip((bool)(frame.value >> STROBE_TRIP_LSB))
	{
		id = frame.id;
	}

	bool strobeTrip;

	Frame generate_frame() const;

private:
	static const int STROBE_TRIP_LSB = 63;
};

class MPPT_Request : public Layout
{
public:
	MPPT_Request(uint32_t baseAddress)
	{
		id = (MPPT_REQ_BASEADDRESS | baseAddress);
	}

	MPPT_Request(const Frame &frame)
	{
		id = frame.id;
	}

	Frame generate_frame() const;
};

class MPPT_Status : public Layout
{
public:
	MPPT_Status(uint32_t baseAddress, bool _battVoltFlag, bool _overTempFlag, bool _noChargeFlag, bool _undervoltFlag,
				uint16_t _uIn, uint16_t _iIn, uint16_t _uOut, uint8_t _tAmb) :
																			   battVoltFlag(_battVoltFlag),
																			   overTempFlag(_overTempFlag),
																			   noChargeFlag(_noChargeFlag),
																			   undervoltFlag(_undervoltFlag),
																			   uIn(_uIn),
																			   iIn(_iIn),
																			   uOut(_uOut),
																			   tAmb(_tAmb)
	{
		id = (MPPT_ANS_BASEADDRESS | baseAddress); // Applies base address
	}

	MPPT_Status(const Frame &frame) : battVoltFlag(mask(frame.value, flagLength, battVoltFlagLSB)),
									  overTempFlag(mask(frame.value, flagLength, overTempFlagLSB)),
									  noChargeFlag(mask(frame.value, flagLength, noChargeFlagLSB)),
									  undervoltFlag(mask(frame.value, flagLength, undervoltFlagLSB)),
									  uIn(mask(frame.value, uiLength, uInLSB)),
									  iIn(mask(frame.value, uiLength, iInLSB)),
									  uOut(mask(frame.value, uiLength, uOutLSB)),
									  tAmb(frame.data[0])
	{
		id = frame.id;
	}

	bool battVoltFlag;
	bool overTempFlag;
	bool noChargeFlag;
	bool undervoltFlag;
	uint16_t uIn;
	uint16_t iIn;
	uint16_t uOut;
	uint8_t tAmb;

	Frame generate_frame() const;

private:
	static const unsigned int battVoltFlagLSB = 7 + 6 * 8;
	static const unsigned int overTempFlagLSB = 6 + 6 * 8;
	static const unsigned int noChargeFlagLSB = 5 + 6 * 8;
	static const unsigned int undervoltFlagLSB = 4 + 6 * 8;
	static const unsigned int uInLSB = 5 * 8;
	static const unsigned int iInLSB = 3 * 8;
	static const unsigned int uOutLSB = 8;

	static const unsigned int flagLength = 1;
	static const unsigned int uiLength = 10;
};

class BMS19_Trip_Stat : public Layout
{
public:
	BMS19_Trip_Stat(bool _dischargeRelay, bool _chargeRelay, bool _MPO): dischargeRelay(_dischargeRelay),
																		 chargeRelay(_chargeRelay),
																		 MPO(_MPO)
	{
		id = BMS19_TRIP_STAT_ID;
	}
	BMS19_Trip_Stat(const Frame &frame) : dischargeRelay((frame.data[0] << dischargeRelayLSB) & 1u),
										  chargeRelay((frame.data[0] << chargeRelayLSB) & 1u),
										  MPO((frame.data[0] << MPOLSB) & 1u)
	{
		id = frame.id;
	}

	Frame generate_frame() const;
	bool dischargeRelay;
	bool chargeRelay;
	bool MPO;

private:
	const static int dischargeRelayLSB = 0;
	const static int chargeRelayLSB = 1;
	const static int MPOLSB = 2;
};

class debugPacket0 : public Layout
{
public:
	debugPacket0(void)
	{
		id = 0x1;
	}
	debugPacket0(const Frame &frame) : contents(frame.value)
	{
		id = frame.id;
	}

	uint64_t contents;
	Frame generate_frame() const;
};

class debugPacket1 : public Layout
{
public:
	debugPacket1(void)
	{
		id = 0x2;
	}
	debugPacket1(const Frame &frame) : contents(frame.value)
	{
		id = frame.id;
	}

	uint64_t contents;
	Frame generate_frame() const;
};

class debugPacket10 : public Layout
{
public:
	debugPacket10(void)
	{
		id = 0x3;
	}
	debugPacket10(const Frame &frame) : contents(frame.value)
	{
		id = frame.id;
	}

	uint64_t contents;
	Frame generate_frame() const;
};

#endif
