//
// Empty Keg Weight = 3452 grams
//

// 
// Scale Settings
//
// Scale | Value
// -------------
//   A   | 22.59
//   B   | TBD
//
// Scale -> Beer Details
//
// Scale | Beer          | Starting Gross Weight | Starting Net Weight
// -------------------------------------------------------------------
//   A   | Blonde Lager  | 22459                 | 19007
//   B   | Red Ale       | TBD                   | TBD

#include "Arduino.h"
#include "HX711.h"

const int NUM_SCALES = 1;
const int NUM_READINGS = 4;

const int LOADCELL_DOUT_PIN_A = 2;
const int LOADCELL_SCK_PIN_A = 3;

const int SCALE_PINS[NUM_SCALES][2] = { { LOADCELL_DOUT_PIN_A, LOADCELL_SCK_PIN_A } };
const float SCALE_INIT_VALUES[NUM_SCALES] = { 22.59 };
const int NET_WEIGHTS[NUM_SCALES] = { 19007 };

const float READING_DEVIATION = 0.05; // percentage

const int MAX_READING = 2500; // grams

const int MAX_INT = 32767;
const int MIN_INT = -32768;

HX711 scales[NUM_SCALES];

int counter = 0;

int offsets[NUM_SCALES] = { 0 };

float readings[NUM_SCALES][NUM_READINGS];

void setup() {
  Serial.begin(9600);
  Serial.println("Beer Scale");
  

  for (int i = 0; i < NUM_SCALES; i++)
  {
    Serial.print("Initializing the scale: ");
    Serial.println(i);
    scales[i].begin(SCALE_PINS[i][0], SCALE_PINS[i][1]);
    scales[i].set_scale(SCALE_INIT_VALUES[i]);
    scales[i].tare();
    scales[i].wait_ready_retry(5);

     // TODO - read last value from FRAM
  }
}

void loop() {

  Serial.print("Counter: ");
  Serial.println(counter);

  for (int i = 0; i < NUM_SCALES; i++) // 
  {
    Serial.print("Working on scale: ");
    Serial.println(i);

    Serial.println("Gathering reading: ");
    float reading = scales[i].get_units(2);
    readings[i][counter] = reading;
  }

  if (counter == (NUM_READINGS - 1))
  {
    for (int i = 0; i < NUM_SCALES; i++)
    {
      Serial.println("examining readings...");
      
      bool validReadings = true;

      // check for valid readings
      for (int j = 0; j < NUM_READINGS; j++)
      {
        float reading_f = readings[i][j];

        Serial.print("reading: ");
        Serial.println(reading_f, 5);

        if (reading_f > MAX_INT)
        {
          Serial.println("reading larger than max int!");
          validReadings = false;
        }

        if (reading_f < MIN_INT)
        {
          Serial.println("reading smaller than min int!");
          validReadings = false;
        }

        if (validReadings)
        {
          int reading_i = int(reading_f);

          if (abs(reading_i) > MAX_READING)
          {
            Serial.println("reading too large (+/-)");
            validReadings = false;
          }
        }
      }

      if (!validReadings)
      {
        Serial.println("...invalid reading(s) detected aborting processing");
        break;
      }
      else
      {
        Serial.println("...valid reading(s) continuing to process");
      }

      int first = 0;
      int deviation = 0;
      bool comparableReadings = true;

      for (int j = 0; j < NUM_READINGS; j++)
      {
        float reading_f = readings[i][j];

        Serial.print("reading: ");
        Serial.println(reading_f, 5);

        // we are within the bounds of an int so flip to integers
        int reading_i = int(reading_f);

        int modReading = reading_i - (reading_i % 10);
        Serial.print("Adjusted to 10s: ");
        Serial.println(modReading);

        if (j == 0)
        {
          first = modReading;
          deviation = int(abs(first * READING_DEVIATION));

          Serial.print("Max deviation: ");
          Serial.println(deviation);
        }
        else
        {
          int difference = abs(first - modReading);
          Serial.print("Difference: ");
          Serial.println(difference);

          if (difference > deviation)
          {
            comparableReadings = false;
            Serial.println("Too much deviation");
          }
          else
          {
            Serial.println("Deviation within tolerance");
          }
        }
      }

      if (comparableReadings)
      {
        Serial.println("Readings are comparable - computing new offset");

        int combinedReadings = 0;
        for (int j = 0; j < NUM_READINGS; j++)
        {
          float reading_f = readings[i][j];
          int reading_i = int(reading_f);
          int modReading = reading_i - (reading_i % 10);

          combinedReadings = combinedReadings + modReading;
        }

        Serial.print("Combined readings: ");
        Serial.println(combinedReadings);

        int averageReading = int(combinedReadings / NUM_READINGS);

        Serial.print("average reading: ");
        Serial.println(averageReading);

        int averagedModReading = averageReading - (averageReading % 10);
        Serial.print("Adjusted to 10s: ");
        Serial.println(averagedModReading);

        Serial.print("average mod reading + offset: ");
        int offsetReading = averagedModReading + offsets[i];
        Serial.println(offsetReading);

        Serial.print("Grams of beer to start: ");
        Serial.println(NET_WEIGHTS[i]);

        int displayedReading = NET_WEIGHTS[i] + offsetReading;
        if (displayedReading > NET_WEIGHTS[i]) 
        {
          displayedReading = NET_WEIGHTS[i];
        }
        Serial.print("Grams of beer left: ");
        Serial.println(displayedReading);

        float percentage = ((float) displayedReading / (float) NET_WEIGHTS[i]) * (float) 100;

        if (percentage > 100)
        {
          percentage = 100;
        }
        if (percentage < 0)
        {
          percentage = 0;
        }

        Serial.print("Percentage of beer left: ");
        Serial.println(round(percentage));

        offsets[i] = offsetReading;

        Serial.print("Previous offset: ");
        Serial.println(offsets[i]);

        Serial.print("New offset: ");
        Serial.println(offsets[i]);

        // TODO - write to FRAM

        Serial.println("taring");
        scales[i].tare();
        scales[i].wait_ready_retry(5);
        Serial.println("taring done");
      }
      else
      {
        Serial.println("Reading(s) aren't comparable");
      }
      
    }
    counter = 0;  
  }
  else
  {
    counter++;
  }

  delay(500);
}