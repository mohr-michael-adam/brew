#include "HX711.h"
#include "LiquidCrystal_I2C.h"

const int SCALE_DATA_A = 2;
const int SCALE_CLOCK_A = 3;
const int SCALE_DATA_B = 5;
const int SCALE_CLOCK_B = 6;

const int NUM_SCALES = 2;

const int DATA_PIN_INDEX = 0;
const int CLOCK_PIN_INDEX = 1;

const int SCALE_PINS[NUM_SCALES][2] = { {SCALE_DATA_A, SCALE_CLOCK_A}, {SCALE_DATA_B, SCALE_CLOCK_B} };

const long SCALE_OFFSETS[NUM_SCALES] = { -33435, 136890 };
const float SCALE_CALIBRAIONS[NUM_SCALES] = { 22.59, 22.81 };

const int KEG_WEIGHTS[NUM_SCALES] = { 3900, 3900 }; // This needs to be reviewed after kegs are empty

const int NUM_WAIT_CYCLES = 4;

const int LCD_SPACES[NUM_SCALES] = { 12, 7 };

const float L_PER_PINT = 0.473176;

HX711 scales[NUM_SCALES];

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
    resetFunc();
  }
}

void setup() 
{
  Serial.begin(9600);

  lcd.init();      
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("  Mohr Brew House");
  lcd.setCursor(0, 1);
  lcd.print("      kg pints");
  lcd.setCursor(0, 2);
  lcd.print("Blonde Lager|Red Ale");

  for (int i = 0; i < NUM_SCALES; i++)
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
  lcd.setCursor(0, 3);

  for (int i = 0; i < NUM_SCALES; i++)
  {
    Serial.print("Scale ");
    Serial.println(i);

    checkScale(scales[i], i);

    float reading = scales[i].get_units() - KEG_WEIGHTS[i];

    Serial.print("reading: ");
    Serial.println(reading);

    float kgs = reading / 1000;

    Serial.print("kgs: ");
    Serial.println(String(kgs, 1));

    int pints = round(kgs / L_PER_PINT); // assumption 1kg = 1L of beer (slightly off but close enough)

    Serial.print("Pints: ");
    Serial.println(pints);

    int numChars = 7; // 12.2 45

    if (kgs < 10)
    {
      numChars--;
    }

    if (pints < 10)
    {
      numChars--;
    }

    // center the text
    int leadingSpaces = ceil((float(LCD_SPACES[i]) - numChars) / 2);
    int trailingSpaces = floor((float(LCD_SPACES[i]) - numChars) / 2);

    Serial.print("Num chars: ");
    Serial.println(numChars);

    Serial.print("Num leading spaces: ");
    Serial.println(leadingSpaces);

    Serial.print("Num trailing spaces: ");
    Serial.println(trailingSpaces);

    for (int j = 0; j < leadingSpaces; j++)
    {
      lcd.print(" ");
    }

    lcd.print(String(kgs, 1));
    lcd.print(" ");
    lcd.print(pints);

    for (int j = 0; j < trailingSpaces; j++)
    {
      lcd.print(" ");
    }

    if (i == 0)
    {
      lcd.print("|");
    }
  }

  delay(1000);
}
