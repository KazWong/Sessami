#include "Si7020.h"

bool Si7020::initWireI2C() {
	/*Serial.print("Product ID: 0x");
	Serial.println(ReadRegister(CAP1114_PRODID), HEX);
	Serial.print("Manuf. ID: 0x");
	Serial.println(ReadRegister(CAP1114_MANUID), HEX);
	Serial.print("Revision: 0x");
	Serial.println(ReadRegister(CAP1114_REV), HEX);

	if ((ReadRegister(CAP1114_PRODID) != 0x3A)
			|| (ReadRegister(CAP1114_MANUID) != 0x5D)
			|| (ReadRegister(CAP1114_REV) != 0x10))
		return false;*/
	return true;
}

float Si7020::GetRH() {
  uint8_t *data = ReadRegister(0xF5, 3);
  uint16_t code = data[0] << 8;
  code += (data[1] & 0xFC);
  delete[] data;
  return ( (125.0*(float)code)/65536.0 ) - 6.0;
}

float Si7020::GetTp() {
  uint8_t *data = ReadRegister(0xF3, 2);
  uint16_t code = data[0] << 8;
  code += (data[1] &= 0xFC);
  delete[] data;
  return ( (175.72*(float)code)/65536.0 ) - 46.85;
}

uint8_t Si7020::ReadUserReg1() {
  return ReadRegister(0xE7);
}

uint8_t Si7020::ReadHCReg() {
  return ReadRegister(0x11);
}

Si7020::Si7020() : I2CSensor(0x40) {
}

Si7020::~Si7020() {
}
