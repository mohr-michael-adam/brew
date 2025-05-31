#include "HX711.h"
#include "LiquidCrystal_I2C.h"

const int SCALE_DATA_A = 2;
const int SCALE_CLOCK_A = 3;
const int SCALE_DATA_B = 5;
const int SCALE_CLOCK_B = 6;

const int NUM_SCALES = 2;
const int NUM_ACTIVE_SCALES = 2;

const int DATA_PIN_INDEX = 0;
const int CLOCK_PIN_INDEX = 1;

const int SCALE_PINS[NUM_SCALES][2] = { {SCALE_DATA_A, SCALE_CLOCK_A}, {SCALE_DATA_B, SCALE_CLOCK_B} };

const long SCALE_OFFSETS[NUM_SCALES] = { -261817, 134080 };
const float SCALE_CALIBRAIONS[NUM_SCALES] = { 22.59, 22.59 };

const int KEG_WEIGHTS[NUM_SCALES] = { 5050, 3650 }; // This needs to be reviewed after kegs are empty

const int NUM_WAIT_CYCLES = 4;

const int LCD_SPACES[NUM_SCALES] = { 9, 10 };

const float L_PER_PINT = 0.473176;

HX711 scales[NUM_SCALES];

float last_reading[NUM_SCALES] = {-1000, -1000};

LiquidCrystal_I2C lcd(0x27,20,4);     

void(* resetFunc) (void) = 0; // create a standard reset function

void checkScale(HX711 scale, int i)
{
  int retries = 0;

  while (!scale.is_ready() && retries < NUM_WAIT_CYCLES)
  {
    delay(250);
    retries++;
  }
    
  if (retries == NUM_WAIT_CYCLES)
  {
    Serial.print("Scale ");
    Serial.print(i);
    Serial.println(" in a bad state - rebooting");
    //resetFunc();
  }
}

void setup() 
{
  Serial.begin(9600);

  lcd.init();      
  lcd.backlight();
  lcd.clear();

  for (int i = 0; i < NUM_ACTIVE_SCALES; i++)
  {
    Serial.print("Initializing the scale: ");
    Serial.println(i);

    scales[i].begin(SCALE_PINS[i][DATA_PIN_INDEX], SCALE_PINS[i][CLOCK_PIN_INDEX]); 
    scales[i].set_scale(SCALE_CALIBRAIONS[i]);                               
    scales[i].set_offset(SCALE_OFFSETS[i]);   

    checkScale(scales[i], i);
  }
}

void loop()
{
  lcd.setCursor(0,0);
  lcd.print("  MOHR BREW HOUSE   ");
  lcd.setCursor(0, 2);
  lcd.print(" Left: ");
  lcd.setCursor(0, 3);
  lcd.print("Right: ");

  for (int i = 0; i < NUM_ACTIVE_SCALES; i++)
  {
    Serial.print("Scale ");
    Serial.println(i);

    checkScale(scales[i], i);

    float reading = scales[i].get_units() - KEG_WEIGHTS[i];

    // FAKE DATA FOR NOW
    //reading = random(5000, 20000);

    Serial.print("reading: ");
    Serial.println(reading);

    float kgs = reading / 1000;

    Serial.print("kgs: ");
    Serial.println(String(kgs, 1));

    Serial.print("last kgs: ");
    Serial.println(String(last_reading[i, 1]));

    Serial.print("Delta: ");
    Serial.println(abs(kgs - last_reading[i]));

    if (abs(kgs - last_reading[i]) <= 0.1)
    {
      continue;
    }

    last_reading[i] = kgs;

    int pints = round(kgs / L_PER_PINT); // assumption 1kg = 1L of beer (slightly off but close enough)

    Serial.print("Pints: ");
    Serial.println(pints);

    lcd.setCursor(7, i+2);

    // String scale;

    // if (i == 0)
    // {
    //   scale = " Left:";
    // }
    // else
    // {
    //   scale = "Right:";
    // }

    // lcd.print(scale);
    lcd.print(String(kgs, 1));
    lcd.print("kg ");
    lcd.print(pints);
    lcd.print("p   ");
  }

  delay(1000);
}
