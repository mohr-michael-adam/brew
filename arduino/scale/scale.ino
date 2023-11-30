// 
// Required libraries
//
// https://github.com/bogde/HX711/archive/refs/tags/0.7.5.zip
// https://github.com/adafruit/Adafruit_FRAM_I2C/archive/refs/tags/2.0.3.zip
// https://github.com/johnrickman/LiquidCrystal_I2C/archive/refs/tags/1.1.3.zip
//

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
#include "Adafruit_FRAM_I2C.h"
#include "LiquidCrystal_I2C.h"

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

Adafruit_FRAM_I2C fram = Adafruit_FRAM_I2C();

LiquidCrystal_I2C lcd(0x27,20,4); 

const int LCD_OFFSET[NUM_SCALES] = { 14 };

void setup() {
  Serial.begin(9600);
  Serial.println("Beer Scale");
  
  if (fram.begin()) { 
    Serial.println("Found I2C FRAM");
  } else {
    Serial.println("I2C FRAM not identified ... check your connections?");
    while (1);
  }

  lcd.init();      
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0,1);
  lcd.print("Blonde Lager: ");

  lcd.setCursor(0,2);
  lcd.print("Red Ale: N/A"); 

  for (int i = 0; i < NUM_SCALES; i++)
  {
    Serial.print("Initializing the scale: ");
    Serial.println(i);
    scales[i].begin(SCALE_PINS[i][0], SCALE_PINS[i][1]);
    scales[i].set_scale(SCALE_INIT_VALUES[i]);
    scales[i].tare();
    scales[i].wait_ready_retry(5);

    fram.readObject(i * sizeof(offsets[i]), offsets[i]);

    Serial.print("Found initial offset: ");
    Serial.println(offsets[i]);
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

        int modReading = reading_i - (reading_i % 10) + int(round(float(reading_i % 10) / 10)) ;
        Serial.print("Adjusted to 10s: ");
        Serial.println(modReading);

        if (j == 0)
        {
          first = modReading;
          first = reading_i;
          deviation = int(abs(first * READING_DEVIATION));

          Serial.print("Max deviation: ");
          Serial.println(deviation);
        }
        else
        {
          //int difference = abs(first - modReading);
          int difference = abs(first - reading_i);
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

        Serial.print("average reading + offset: ");
        int offsetReading = averageReading + offsets[i];
        Serial.println(offsetReading);

        Serial.print("Previous offset: ");
        Serial.println(offsets[i]);

        offsets[i] = offsetReading;

        Serial.print("New offset: ");
        Serial.println(offsets[i]);

        fram.writeObject(i * sizeof(offsets[i]), offsets[i]);

        Serial.println("taring");
        scales[i].tare();
        scales[i].wait_ready_retry(5);
        Serial.println("taring done");
      }
      else
      {
        Serial.println("Reading(s) aren't comparable");
      }

      Serial.print("Grams of beer to start: ");
      Serial.println(NET_WEIGHTS[i]);

      int displayedReading = NET_WEIGHTS[i] + offsets[i];
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

      lcd.setCursor(LCD_OFFSET[i],i+1);
      lcd.print(round(percentage));
      lcd.print("% ");
    }
    counter = 0;  
  }
  else
  {
    counter++;
  }

  delay(500);
}