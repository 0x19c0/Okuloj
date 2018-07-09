#include "okuloj.h"

void Logger::add(uint8_t* mac) {
	if (this->size == kMaxDevicesNum) return;
	for (int i = 0; i < this->size; ++i) {
		if (!memcmp(&array[kMacLength*i], mac, kMacLength)) return;
	}
	memcpy(&array[size*kMacLength], mac, kMacLength);
	this->size++;
}

bool Logger::empty() {
	return size == 0;
}

uint8_t* Logger::top() {
	if (size == 0) return NULL;
	return &array[(size-1)*kMacLength];
}

void Logger::pop() {
	if (size > 0) size--;
}

int Logger::get_size() {
  return size;
}

void Logger::flush() {
  size = 0;
}

bool MacValid(uint8_t* mac) {
  for (uint8_t i = 0; i < 6; i++)
    if (mac[i] != 0x00 && mac[i] != 0xff) return true;
  return false;
}

bool MacMulticast(uint8_t* mac) {
  if (mac[0] == 0x33 && mac[1] == 0x33) return true;
  if (mac[0] == 0x01 && mac[1] == 0x80 && mac[2] == 0xC2) return true;
  if (mac[0] == 0x01 && mac[1] == 0x00 &&
		(mac[2] == 0x5E || mac[2] == 0x0C)) return true;
  if (mac[0] == 0x01 && mac[1] == 0x0C && mac[2] == 0xCD &&
		(mac[3] == 0x01 || mac[3] == 0x02 || mac[3] == 0x04) &&
		(mac[4] == 0x00 || mac[4] == 0x01)) return true;
  if (mac[0] == 0x01 && mac[1] == 0x00 && mac[2] == 0x0C && mac[3] == 0xCC &&
		mac[4] == 0xCC && (mac[5] == 0xCC || mac[5] == 0xCD)) return true;
  if (mac[0] == 0x01 && mac[1] == 0x1B && mac[2] == 0x19 && mac[3] == 0x00 &&
		mac[4] == 0x00 && mac[5] == 0x00) return true;
  return false;
}

bool FrameValid(uint8_t frame) {
	if (frame == 0x80 || frame == 0x40 || frame == 0x50 ||
		frame == 0xc0 || frame == 0xa0) return false;
	return true;
}
