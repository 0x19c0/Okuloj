#pragma once

#include <Arduino.h>

// Maximum number of saved devices
const int kMaxDevicesNum = 40;

// Length of mac address
const int kMacLength = 6;

// Logger wraps the array of logged macs
// There is no reason not to use std::set instead, except there is no STL on Arduino
class Logger {
private:
  uint8_t array[kMaxDevicesNum*kMacLength];
  int size;
public:
	Logger() : size(0) {}
  void add(uint8_t* mac);
	bool empty();
	uint8_t* top();
	void pop();
  int get_size();
  void flush();
};

// Checks whether a given mac address is non-empty
bool MacValid(uint8_t* mac);
// Checks whether a given mac address is a multicast
bool MacMulticast(uint8_t* mac);
// Checks whether frame may contain mac address
bool FrameValid(uint8_t frame);
