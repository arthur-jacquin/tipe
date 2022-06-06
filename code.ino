// Libraries
#include <Wire.h>
#include <VL53L1X.h>

// Declare used pins
const int pinVibr = 3;
VL53L1X sensor;     // I2C protocol

// Declare variables
int dist;           // Distance measured (in mm)
int intensity;      // Command for vibration (from 0 to 255)
const int n = 3;    // Number of points for sliding average
int logs[n];        // Array to store last measurements
int k = 0;          // Sliding index in array
int avg = 0;        // Average value

void setup () {
  pinMode(pinVibr, OUTPUT); // Set vibration motor as output

  for (int i = 0; i < n; i++) { // Initialize array
    logs[i] = 0;
  }

  // Communication through I2C (for VL53L1X), set at 400 kHz
  Wire.begin();
  Wire.setClock(400000);

  // Configure VL53L1X in long distance mode
  // Read rate frequency set as 20Hz (<< 400 kHz)
  sensor.setTimeout(500);
  if (!sensor.init()) { while (1); } // No sensor detected
  sensor.setDistanceMode(VL53L1X::Long);
  sensor.setMeasurementTimingBudget(50000);
  sensor.startContinuous(50);
}

void loop () {
  // Reading the sensor measurement
  dist = sensor.read();

  // Computing sliding average
  k = (k + 1)%n;
  avg = avg + (dist - logs[k])/n;
  logs[k] = dist;

  // Modifying vibration intensity
  intensity = dist_to_intensity(avg);
  analogWrite(pinVibr, intensity);
}

int dist_to_intensity (int x) {
  // Takes dist in mm, return command between 0 and 255
  if (x > 2000) {
    return 0;
  } else {
    return -1.8*pow(10, -8)*x*x*x + 0.108*x*x/1000 - 0.234*x + 255;
  };
}
