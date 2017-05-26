/***************************************************
 Title : Driver class for CAP1114
 Modified by : Kaz Wong
 Begin Date : 7 Dec 2016
 Datasheet Link : http://ww1.microchip.com/downloads/en/DeviceDoc/CAP1114.pdf
 Description : Class for read/write register for CAP1114
 ****************************************************/
#include "CAP1114_Driver.h"

namespace CAP1114 {

uint8_t CAP1114_Driver::st_00 = 0, CAP1114_Driver::st_03 = 0,
		CAP1114_Driver::st_04 = 0;
uint8_t CAP1114_Driver::st_73 = 0, CAP1114_Driver::st_74 = 0,
		CAP1114_Driver::st_0F = 0;
uint8_t CAP1114_Driver::st_27 = 0, CAP1114_Driver::st_28 = 0;

bool CAP1114_Driver::initWireI2C() {
	Serial.print("Product ID: 0x");
	Serial.println(ReadRegister(CAP1114_PRODID), HEX);
	Serial.print("Manuf. ID: 0x");
	Serial.println(ReadRegister(CAP1114_MANUID), HEX);
	Serial.print("Revision: 0x");
	Serial.println(ReadRegister(CAP1114_REV), HEX);

	if ((ReadRegister(CAP1114_PRODID) != 0x3A)
			|| (ReadRegister(CAP1114_MANUID) != 0x5D)
			|| (ReadRegister(CAP1114_REV) != 0x10))
		return false;

	return true;
}

void CAP1114_Driver::UpdateLED(uint8_t *status1, uint8_t *status2) {
	st_73 = ReadRegister(CAP1114_LEDOUTPUT1);
	st_74 = ReadRegister(CAP1114_LEDOUTPUT2);

	if (status1)
		*status1 = st_73;
	if (status2)
		*status2 = st_74;
}
State CAP1114_Driver::GetLED(LED b) const {
	if ((uint8_t) b <= 0xFF)
		return (st_73 & (uint8_t) b);
	return st_74 & ((uint8_t) b & HBMASK);
}
void CAP1114_Driver::SetLED(uint16_t value) {
	if (value <= 0xFF) {
		WriteRegister(CAP1114_LEDOUTPUT1, value);
		st_73 = ReadRegister(CAP1114_LEDOUTPUT1);
	} else {
		if (value <= B0111) {
			WriteRegister(CAP1114_LEDOUTPUT2, (value & HBMASK));
			st_74 = ReadRegister(CAP1114_LEDOUTPUT2);
		}
	}
}
void CAP1114_Driver::SetLED(LED b, State st) {
	if ((uint8_t) b <= 0xFF) {
		WriteRegBit(CAP1114_LEDOUTPUT1, (uint8_t) b, st);
		st_73 = ReadRegister(CAP1114_LEDOUTPUT1);
	} else {
		WriteRegBit(CAP1114_LEDOUTPUT2, ((uint8_t) b & HBMASK), st);
		st_74 = ReadRegister(CAP1114_LEDOUTPUT2);
	}
}

//Main Status Control Registers
void CAP1114_Driver::UpdateMSControl(uint8_t *status) {
	st_00 = ReadRegister(CAP1114_MAIN);

	if (status)
		*status = st_00;
}
State CAP1114_Driver::GetMSControl(MSControl b) const {
	return (st_00 & (uint8_t) b);
}
void CAP1114_Driver::SetMSControl(uint8_t value) {
	WriteRegister(CAP1114_MAIN, value);
	st_00 = ReadRegister(CAP1114_MAIN);
}
void CAP1114_Driver::SetMSControl(MSControl b, State st) {
	WriteRegBit(CAP1114_MAIN, (uint8_t) b, st);
	st_00 = ReadRegister(CAP1114_MAIN);
}

//Button Status Registers
void CAP1114_Driver::UpdateCS(uint8_t *status1, uint8_t *status2) {
	st_03 = ReadRegister(CAP1114_SENINPUTSTATUS1);
	st_04 = ReadRegister(CAP1114_SENINPUTSTATUS2);

	if (status1)
		*status1 = st_03;
	if (status2)
		*status2 = st_04;
}
uint16_t CAP1114_Driver::GetCS() const {
	return ((st_04 << 8) | st_03);
}

State CAP1114_Driver::GetCS(CS b) const {
	if ((uint8_t) b <= 0xFF)
		return (st_03 & (uint8_t) b);
	return st_04 & ((uint8_t) b & HBMASK);
}

//Group Status Registers
void CAP1114_Driver::UpdateSlide(uint8_t *status) {
	st_0F = ReadRegister(CAP1114_GROUPSTATUS);

	if (status)
		*status = st_0F;
}
uint8_t CAP1114_Driver::GetSlide() const {
	return st_0F;
}
State CAP1114_Driver::GetSlide(Slide b) const {
	return st_0F & (uint8_t) b;
}

//Slider Position / Volumetric Data Registers
uint8_t CAP1114_Driver::GetSliPos() {
	return ReadRegister(CAP1114_SLIDERPOSITION);
}
uint8_t CAP1114_Driver::GetVolData() {
	return ReadRegister(CAP1114_SLIDERPOSITION);
}
uint8_t CAP1114_Driver::SetVolData(uint8_t vol) {
	if ((vol >= 0) && (vol <= 100))
		for (int i = 0; i < 7; i++) {
			uint8_t tmp = vol;
			WriteRegBit(CAP1114_SLIDERPOSITION, 1 << i,
					(State) ((vol & (1 << i)) >> i));
		}
}

//Sensor Delta Count Registers
uint8_t CAP1114_Driver::GetSDelta(SDelta b) {
	return (ReadRegister((uint8_t) b));
}

uint8_t CAP1114_Driver::GetSDelta(int8_t b) {
	if ( (b>=0x10) && (b<=0x1D) )
		return (ReadRegister(b));
}

//
//Get
uint8_t CAP1114_Driver::GetDeltaSen() {
	return (ReadRegister(CAP1114_DATASENSITIVITY) & B1110000)>> 4;
}
uint8_t CAP1114_Driver::GetBaseShift() {
	return (ReadRegister(CAP1114_DATASENSITIVITY) & B1111);
}

//Set
void CAP1114_Driver::SetDeltaSen(uint8_t value) {
	if ((value >= 0) && (value <= 7)) {
		WriteRegBit(CAP1114_DATASENSITIVITY, B6, (value & B2) ? HI : LO);
		WriteRegBit(CAP1114_DATASENSITIVITY, B5, (value & B1) ? HI : LO);
		WriteRegBit(CAP1114_DATASENSITIVITY, B4, (value & B0) ? HI : LO);
	}
}

void CAP1114_Driver::SetBaseShift(uint8_t value) {
	if ((value >= 0) && (value <= 15)) {
		WriteRegBit(CAP1114_DATASENSITIVITY, B3, (value & B3) ? HI : LO);
		WriteRegBit(CAP1114_DATASENSITIVITY, B2, (value & B2) ? HI : LO);
		WriteRegBit(CAP1114_DATASENSITIVITY, B1, (value & B1) ? HI : LO);
		WriteRegBit(CAP1114_DATASENSITIVITY, B0, (value & B0) ? HI : LO);
	}
}


//Configuration Registers
//Get
bool CAP1114_Driver::GetTimeOutConfig() {
	return (ReadRegister(CAP1114_CONFIGREG) & B7);
}
bool CAP1114_Driver::GetPosVolConfig() {
	return (ReadRegister(CAP1114_CONFIGREG) & B6);
}
bool CAP1114_Driver::GetNoiseConfig(bool *th, bool *flag) {
	*th = ReadRegister(CAP1114_CONFIGREG) & B5;
	*flag = ReadRegister(CAP1114_CONFIGREG) & B4;
}
bool CAP1114_Driver::GetMaxDurCalConfig(bool *sg, bool *gp) {
	*sg = ReadRegister(CAP1114_CONFIGREG) & B3;
	*gp = ReadRegister(CAP1114_CONFIGREG) & B1;
}
bool CAP1114_Driver::GetRptRateConfig(bool *sg, bool *gp) {
	*sg = ReadRegister(CAP1114_CONFIGREG) & B2;
	*gp = ReadRegister(CAP1114_CONFIGREG) & B0;
}
//Set
void CAP1114_Driver::SetTimeOutConfig(State st) {
	WriteRegBit(CAP1114_CONFIGREG, B7, st);
}
void CAP1114_Driver::SetPosVolConfig(State st) {
	WriteRegBit(CAP1114_CONFIGREG, B6, st);
}
void CAP1114_Driver::SetNoiseConfig(State th, State flag) {
	WriteRegBit(CAP1114_CONFIGREG, B5, th);
	WriteRegBit(CAP1114_CONFIGREG, B4, flag);
}
void CAP1114_Driver::SetMaxDurCalConfig(State sg, State gp) {
	WriteRegBit(CAP1114_CONFIGREG, B3, sg);
	WriteRegBit(CAP1114_CONFIGREG, B1, gp);
}
void CAP1114_Driver::SetRptRateConfig(State sg, State gp) {
	WriteRegBit(CAP1114_CONFIGREG, B2, sg);
	WriteRegBit(CAP1114_CONFIGREG, B0, gp);
}

//Group Configuration Registers
void CAP1114_Driver::GetGroupConfig(uint8_t *rpt_ph, uint8_t *m_press,
		uint8_t *max_dur, uint8_t *rpt_sl) {
	uint8_t tmp;

	tmp = ReadRegister(CAP1114_GROUPCONFIG1);
	(rpt_ph) ? *rpt_ph = (tmp & B11110000) >> 4 : NULL;
	(m_press) ? *m_press = (tmp & B00001111) : NULL;
	tmp = ReadRegister(CAP1114_GROUPCONFIG2);
	(max_dur) ? *max_dur = (tmp & B11110000) >> 4 : NULL;
	(rpt_sl) ? *rpt_sl = (tmp & B00001111) : NULL;
}

//Interrupt Enable Registers
void CAP1114_Driver::UpdateIntEn(uint8_t *status1, uint8_t *status2) {
	st_27 = ReadRegister(CAP1114_INT_ENABLE_REG1);
	st_28 = ReadRegister(CAP1114_INT_ENABLE_REG2);

	if (status1)
		*status1 = st_27;
	if (status2)
		*status2 = st_28;
}
uint16_t CAP1114_Driver::GetIntEn() const {
	return ((st_28 << 8) | st_27);
}

State CAP1114_Driver::GetIntEn(IntEn b) const {
	if ((uint8_t) b <= 0xFF)
		return (st_27 & (uint8_t) b);
	return st_28 & ((uint8_t) b & HBMASK);
}
void CAP1114_Driver::SetIntEn(uint8_t value) {
	if (value <= 0xFF) {
		WriteRegister(CAP1114_INT_ENABLE_REG1, value);
		st_27 = ReadRegister(CAP1114_INT_ENABLE_REG1);
	} else {
		WriteRegister(CAP1114_INT_ENABLE_REG2, (value & HBMASK));
		st_28 = ReadRegister(CAP1114_INT_ENABLE_REG2);
	}
}
void CAP1114_Driver::SetIntEn(IntEn b, State st) {
	if ((uint8_t) b <= 0xFF) {
		WriteRegBit(CAP1114_INT_ENABLE_REG1, (uint8_t) b, st);
		st_27 = ReadRegister(CAP1114_INT_ENABLE_REG1);
	} else {
		WriteRegBit(CAP1114_INT_ENABLE_REG2, ((uint8_t) b & HBMASK), st);
		st_28 = ReadRegister(CAP1114_INT_ENABLE_REG2);
	}
}

//Proximity Control Register
//Get
bool CAP1114_Driver::GetProxEN() {
	return (ReadRegister(CAP1114_PROXIMITYCTRL) & B7) >> 7;
}
bool CAP1114_Driver::GetProxSum() {
	return (ReadRegister(CAP1114_PROXIMITYCTRL) & B6) >> 6;
}
uint8_t CAP1114_Driver::GetProxAvg() {
	return (ReadRegister(CAP1114_PROXIMITYCTRL) & B11000) >> 3;
}
uint8_t CAP1114_Driver::GetProxSen() {
	return (ReadRegister(CAP1114_PROXIMITYCTRL) & B111);
}

//Set
void CAP1114_Driver::SetProxEN(State st) {
	WriteRegBit(CAP1114_PROXIMITYCTRL, B7, st);
}
void CAP1114_Driver::SetProxSum(State st) {
	WriteRegBit(CAP1114_PROXIMITYCTRL, B6, st);
}
void CAP1114_Driver::SetProxAvg(uint8_t value) {
	if ((value >= 0) && (value <= 3)) {
		WriteRegBit(CAP1114_PROXIMITYCTRL, B4, (value & B1) ? HI : LO);
		WriteRegBit(CAP1114_PROXIMITYCTRL, B3, (value & B0) ? HI : LO);
	}
}
void CAP1114_Driver::SetProxSen(uint8_t value) {
	if ((value >= 0) && (value <= 7)) {
		WriteRegBit(CAP1114_PROXIMITYCTRL, B2, (value & B2) ? HI : LO);
		WriteRegBit(CAP1114_PROXIMITYCTRL, B1, (value & B1) ? HI : LO);
		WriteRegBit(CAP1114_PROXIMITYCTRL, B0, (value & B0) ? HI : LO);
	}
}

//Calibration Activate Registers
void CAP1114_Driver::SetCalAct(uint8_t xbits) {
	if ((xbits >= 0) && (xbits <= 0xFF))
		WriteRegister(CAP1114_CALIBRATIONACTIVATE, xbits);
}
void CAP1114_Driver::SetGroupCalAct(uint8_t xbits) {
	if ((xbits >= 0) && (xbits <= 0x7F))
		WriteRegister(CAP1114_GROUPCALIBRATIONACTIVATE, xbits);
}

//Multiple Touch Configuration Register
uint8_t CAP1114_Driver::GetMTConfig() {
	return (ReadRegister(CAP1114_MTBLK));
}
void CAP1114_Driver::SetMTConfig(uint8_t value) {
	WriteRegister(CAP1114_MTBLK, value);
}

//Sensor Threshold Registers
uint8_t CAP1114_Driver::GetThresh(uint8_t sensor) {
	if ( (sensor > 0x30) && (sensor < 0x37) )
		return ReadRegister(sensor);
}
void CAP1114_Driver::SetThresh(uint8_t sensor, uint8_t value){
	if ( (sensor > 0x30) && (sensor < 0x37) )
		WriteRegister(sensor, value);
}

//Slider Velocity Configuration Registers
void CAP1114_Driver::SetAccelEN(State st) {
	WriteRegBit(CAP1114_VELCONFIG, B7, st);
}
void CAP1114_Driver::SetIntNum(uint8_t value) {
	if ((value >= 0) && (value <= 7)) {
		WriteRegBit(CAP1114_PROXIMITYCTRL, B6, (value & B2) ? HI : LO);
		WriteRegBit(CAP1114_PROXIMITYCTRL, B5, (value & B1) ? HI : LO);
		WriteRegBit(CAP1114_PROXIMITYCTRL, B4, (value & B0) ? HI : LO);
	}
}
void CAP1114_Driver::SetSliTime(uint8_t value) {
	if ((value >= 0) && (value <= 3)) {
		WriteRegBit(CAP1114_PROXIMITYCTRL, B3, (value & B1) ? HI : LO);
		WriteRegBit(CAP1114_PROXIMITYCTRL, B2, (value & B0) ? HI : LO);
	}
}
void CAP1114_Driver::SetRptScale(uint8_t value) {
	if ((value >= 0) && (value <= 3)) {
		WriteRegBit(CAP1114_PROXIMITYCTRL, B1, (value & B1) ? HI : LO);
		WriteRegBit(CAP1114_PROXIMITYCTRL, B0, (value & B0) ? HI : LO);
	}
}

//LED/GPIO Direction Register
uint8_t CAP1114_Driver::GetGPIODir() {
	return (ReadRegister(CAP1114_LEDDIRECTION));
}
State CAP1114_Driver::GetGPIODir(GPIODir b) {
	uint8_t st_70 = ReadRegister(CAP1114_LEDDIRECTION);
	return (st_70 & (uint8_t) b);
}
void CAP1114_Driver::SetGPIODir(uint8_t value) {
	WriteRegister(CAP1114_LEDDIRECTION, value);
}
void CAP1114_Driver::SetGPIODir(GPIODir b, State st) {
	WriteRegBit(CAP1114_LEDDIRECTION, (uint8_t) b, st);
}

//LED/GPIO Output Type Register
void CAP1114_Driver::SetOutputType(uint8_t value) {
	WriteRegister(CAP1114_LEDOUTPUTTYPE, value);
}

//LED Polarity Registers
uint8_t CAP1114_Driver::SetLEDPol(uint8_t value) {
	WriteRegister(CAP1114_LEDPOL1, value);
}

//Sensor LED Linking Register
uint8_t CAP1114_Driver::GetLEDLinking() {
	return (ReadRegister(CAP1114_LEDLINK));
}
State CAP1114_Driver::GetLEDLinking(LEDLinking b) {
	uint8_t st_80 = ReadRegister(CAP1114_LEDLINK);
	return (st_80 & (uint8_t) b);
}
void CAP1114_Driver::SetLEDLinking(uint8_t value) {
	WriteRegister(CAP1114_LEDLINK, value);
}
void CAP1114_Driver::SetLEDLinking(LEDLinking b, State st) {
	WriteRegBit(CAP1114_LEDLINK, (uint8_t) b, st);
}


//LED Behavior Registers
uint8_t CAP1114_Driver::GetLEDBehavior() {
	
}
void CAP1114_Driver::SetLEDBehavior(unsigned int b, uint8_t value) {
	switch (b>>8) {
		case 0 :
			WriteRegBit(CAP1114_LEDDUTYCYCLE1, B3, (value & B3) ? HI : LO);
			WriteRegBit(CAP1114_LEDDUTYCYCLE1, B2, (value & B2) ? HI : LO);
			WriteRegBit(CAP1114_LEDDUTYCYCLE1, B1, (value & B1) ? HI : LO);
			WriteRegBit(CAP1114_LEDDUTYCYCLE1, B0, (value & B0) ? HI : LO);
		case 1 :
			WriteRegBit(CAP1114_LEDDUTYCYCLE2, B3, (value & B3) ? HI : LO);
			WriteRegBit(CAP1114_LEDDUTYCYCLE2, B2, (value & B2) ? HI : LO);
			WriteRegBit(CAP1114_LEDDUTYCYCLE2, B1, (value & B1) ? HI : LO);
			WriteRegBit(CAP1114_LEDDUTYCYCLE2, B0, (value & B0) ? HI : LO);
		case 2 :
			WriteRegBit(CAP1114_LEDDUTYCYCLE3, B3, (value & B3) ? HI : LO);
			WriteRegBit(CAP1114_LEDDUTYCYCLE3, B2, (value & B2) ? HI : LO);
			WriteRegBit(CAP1114_LEDDUTYCYCLE3, B1, (value & B1) ? HI : LO);
			WriteRegBit(CAP1114_LEDDUTYCYCLE3, B0, (value & B0) ? HI : LO);
		case 3 :
			WriteRegBit(CAP1114_LEDDUTYCYCLE4, B3, (value & B3) ? HI : LO);
			WriteRegBit(CAP1114_LEDDUTYCYCLE4, B2, (value & B2) ? HI : LO);
			WriteRegBit(CAP1114_LEDDUTYCYCLE4, B1, (value & B1) ? HI : LO);
			WriteRegBit(CAP1114_LEDDUTYCYCLE4, B0, (value & B0) ? HI : LO);
	}
}
	
//LED Pulse 1 Period Registers
void CAP1114_Driver::GetPulse1P() {
	
}
void CAP1114_Driver::GetPulse1Trig() {
	
}
void CAP1114_Driver::SetPulse1P() {
	
}
void CAP1114_Driver::SetPulse1Trig() {
	
}
	
//LED Pulse 2 Period Registers
void CAP1114_Driver::GetPulse2P() {
	
}
void CAP1114_Driver::SetPulse2P() {
	
}
	
//LED Breathe Period Registers
void CAP1114_Driver::GetBreatheP() {
	
}
void CAP1114_Driver::SetBreatheP() {
	
}

//LED Pulse and Breathe Duty Cycle Registers
uint8_t CAP1114_Driver::GetLEDDC() {
	
}
void CAP1114_Driver::SetLEDDC(unsigned int b, uint8_t value){
	switch (b>>8) {
		case 0 :
			WriteRegBit(CAP1114_LEDDUTYCYCLE1, B1, (value & B1) ? HI : LO);
			WriteRegBit(CAP1114_LEDDUTYCYCLE1, B0, (value & B0) ? HI : LO);
		case 1 :
			WriteRegBit(CAP1114_LEDDUTYCYCLE2, B1, (value & B1) ? HI : LO);
			WriteRegBit(CAP1114_LEDDUTYCYCLE2, B0, (value & B0) ? HI : LO);
		case 2 :
			WriteRegBit(CAP1114_LEDDUTYCYCLE3, B1, (value & B1) ? HI : LO);
			WriteRegBit(CAP1114_LEDDUTYCYCLE3, B0, (value & B0) ? HI : LO);
		case 3 :
			WriteRegBit(CAP1114_LEDDUTYCYCLE4, B1, (value & B1) ? HI : LO);
			WriteRegBit(CAP1114_LEDDUTYCYCLE4, B0, (value & B0) ? HI : LO);
	}
}

CAP1114_Driver::CAP1114_Driver() :
		I2CSensor(0x28) {
}

CAP1114_Driver::~CAP1114_Driver() {
}

}
