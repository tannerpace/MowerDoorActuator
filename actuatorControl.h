#ifndef ACTUATORCONTROL_H
#define ACTUATORCONTROL_H

#include <Arduino.h>

#define RELAY1_PIN 18
#define RELAY2_PIN 19


void setupActuator();


void extendActuator();
void retractActuator();
void stopActuator();

#endif 
