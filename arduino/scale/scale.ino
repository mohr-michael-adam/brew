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
//   B   | 22.81
//
// Scale -> Beer Details
//
// Scale | Beer          | Starting Gross Weight | Starting Net Weight
// -------------------------------------------------------------------
//   A   | Blonde Lager  | 22459                 | 19007
//   B   | Red Ale       | 22870                 | 19418

#include "Arduino.h"
#include "HX711.h"
#include "Adafruit_FRAM_I2C.h"
#include "LiquidCrystal_I2C.h"

const int NUM_SCALES = 2;
const int NUM_READINGS = 4;

const int LOADCELL_DOUT_PIN_A = 2;
const int LOADCELL_SCK_PIN_A = 3;
const int LOADCELL_DOUT_PIN_B = 5;
const int LOADCELL_SCK_PIN_B = 6;

const int SCALE_PINS[NUM_SCALES][2] = { { LOADCELL_DOUT_PIN_A, LOADCELL_SCK_PIN_A },{ LOADCELL_DOUT_PIN_B, LOADCELL_SCK_PIN_B }  };
const float SCALE_INIT_VALUES[NUM_SCALES] = { 22.59, 22.81 };
const int NET_WEIGHTS[NUM_SCALES] = { 19007, 19418 };

const int MAX_READING = 2000; // grams

const int MAX_INT = 32767;
const int MIN_INT = -32768;

const int NUM_WAIT_CYCLES = 4;

HX711 scales[NUM_SCALES];

int counter = 0;

int offsets[NUM_SCALES] = { 0, 0 };

float readings[NUM_SCALES][NUM_READINGS];

Adafruit_FRAM_I2C fram = Adafruit_FRAM_I2C();

LiquidCrystal_I2C lcd(0x27,20,4); 

const int LCD_OFFSET[NUM_SCALES] = { 1, 3 };

void(* resetFunc) (void) = 0; // create a standard reset function

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

  lcd.setCursor(0,0);
  lcd.print("Blonde Lager: ");

  lcd.setCursor(0,2);
  lcd.print("Red Ale: "); 

  for (int i = 0; i < NUM_SCALES; i++)
  {
    int retries = 0;

    Serial.print("Initializing the scale: ");
    Serial.println(i);
    scales[i].begin(SCALE_PINS[i][0], SCALE_PINS[i][1]);
    scales[i].set_scale(SCALE_INIT_VALUES[i]);
    scales[i].power_up();
    while (!scales[i].is_ready() && retries < NUM_WAIT_CYCLES)
    {
      delay(250);
      retries++;
    }
    
    if (retries == NUM_WAIT_CYCLES)
    {
      Serial.println("Waited enough time rebooting");
      resetFunc();
    }

    scales[i].tare();
    scales[i].power_down();

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

    scales[i].power_up();

    int retries = 0;

    while (!scales[i].is_ready() && retries < NUM_WAIT_CYCLES)
    {
      delay(250);
      retries++;
    }
    
    if (retries == NUM_WAIT_CYCLES)
    {
      Serial.println("Waited enough time rebooting");
      resetFunc();
    }

    float reading = scales[i].get_units(25);
    scales[i].power_down();
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
        Serial.println("...invalid reading(s) resetting");
        resetFunc();
      }
      else
      {
        Serial.println("...valid reading(s) continuing to process");
      }

      int first = 0;
      int deviation = 0;
      bool comparableReadings = true;

      if (comparableReadings)
      {
        Serial.println("Readings are comparable - computing new offset");

        int combinedReadings = 0;
        for (int j = 0; j < NUM_READINGS; j++)
        {
          float reading_f = readings[i][j];
          int reading_i = int(reading_f);
          combinedReadings = combinedReadings + reading_i;
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
        scales[i].power_up();
        scales[i].tare();
        scales[i].wait_ready_retry(5);
        scales[i].power_down();
        Serial.println("taring done");
      }
      else
      {
        Serial.println("Reading(s) aren't comparable");
      }

      Serial.print("Grams of beer to start: ");
      Serial.println(NET_WEIGHTS[i]);

      int displayedReading = NET_WEIGHTS[i] + offsets[i];
      Serial.print("Grams of beer left: ");
      Serial.println(displayedReading);

      float percentage = ((float) displayedReading / (float) NET_WEIGHTS[i]) * (float) 100;
      Serial.print("Percentage of beer left: ");
      Serial.println(round(percentage)); 

      lcd.setCursor(0, LCD_OFFSET[i]);
      lcd.print(displayedReading);
      lcd.print("g ");

      int original = displayedReading;
      int digitCount = 0;

      while ( original > 0 )
      {
        digitCount++;
        original = original/10;
      }

      for (int space = 0; space < (5 - digitCount); space++)
      {
        lcd.print(" ");
      }

      lcd.print(round(percentage));
      lcd.print("%  ");
    }
    counter = 0;  
  }
  else
  {
    counter++;
  }

  delay(2500);
}