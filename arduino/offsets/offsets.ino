#include "HX711.h" 

const int NUM_SCALES = 2;

const int SCALE_DATA_A = 2;
const int SCALE_CLOCK_A = 3;
const int SCALE_DATA_B = 5;
const int SCALE_CLOCK_B = 6;

const int DATA_PIN_INDEX = 0;
const int CLOCK_PIN_INDEX = 1;

const int SCALE_PINS[NUM_SCALES][2] = { {SCALE_DATA_A, SCALE_CLOCK_A}, {SCALE_DATA_B, SCALE_CLOCK_B} };
const float SCALE_CALIBRATIONS[NUM_SCALES] = { 22.59, 22.81 }; // Use scale_calibration to get these values

HX711 scales[NUM_SCALES];

void setup() 
{
  Serial.begin(9600);

  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("Record Zero factors");

  for (int i = 0; i < NUM_SCALES; i++)
  {
    scales[i].begin(SCALE_PINS[DATA_PIN_INDEX], SCALE_PINS[CLOCK_PIN_INDEX]);
    scales[i].set_scale(); // no units - raw values
    scales[i].tare(); // get an average reading and set as offset

    long zero_factor = scales[i].read_average(5); // get the reading and store and remove the need to tare the scale
    Serial.print("Zero factor for scale "); 
    Serial.print(i);
    Serial.print(": ");
    Serial.println(zero_factor);
  } 
}

void loop() 
{
  for (int i = 0; i < NUM_SCALES; i++)
  {
    scales[i].set_scale(SCALE_CALIBRATIONS[i]);

    Serial.print("Scale ");
    Serial.print(i);
    Serial.print(" offset value: ");
    Serial.print(scales[i].get_units(), 2);
    Serial.println();
  } 

  Serial.println();
  delay(5000);
}
