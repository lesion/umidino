/*
  umidino 1.0 - keep a place at selected temperature
  by lesion
  Sept 2014
  This code is in the public domain.
  Documentation: http://lesion.github.io/2014/09/25/umidino/
*/

// we are going to use an LCD and the cheap DHT11 sensor
#include <LiquidCrystal.h>
#include <dht11.h>

// sensor on A0
dht11 DHT11;
#define SENSOR_PIN A0

// pin connected to relay
#define RELAY_PIN 2

// pin connected to potentiometer
// we use this to select wanted temperature
#define POT_PIN A1

// wait minimum this ms until each status
// change to avoid toggle relay too fast
#define CHANGE_DELAY 10000

#define MIN_TEMP 20
#define MAX_TEMP 30

// initialize the library with the numbers of the interface pins
// LiquidCrystal(rs, enable, d4, d5, d6, d7)
// my lcd datasheet: http://www.mikrocontroller.net/attachment/2612/LCD.pdf
LiquidCrystal lcd(9, 10, 8, 7, 6, 5);

// store last change status time
long last_change;
boolean changing_temperature=false;

// store the temperature to reach
int goal_temperature=0;

// print normal information to lcd
void print_data(int chk){
  // clear lcd
  lcd.clear();

  switch (chk)
  {
    case DHTLIB_OK:
      // print readed value to lcd
      lcd.print("H:");
      lcd.print(DHT11.humidity);
      lcd.print("% T:");
      lcd.setCursor(0,1);
      lcd.print(DHT11.temperature);
      lcd.print("C");
      break;

    case DHTLIB_ERROR_CHECKSUM:
    case DHTLIB_ERROR_TIMEOUT:
    default:
      lcd.print("SENSOR E");
      lcd.setCursor(0,1);
      lcd.print("RROR!");

    break;
  }
}

void setup() {

  pinMode(RELAY_PIN,OUTPUT);
  pinMode(POT_PIN,INPUT);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // Print a welcome blinking message to the LCD.
  for(int i=0;i<3;i++)
  {
    lcd.clear();
    delay(300);
    lcd.setCursor(0,0);
    lcd.print("Umidino");
    lcd.setCursor(0,1);
    lcd.print("1.0");
    delay(500);
  }

  last_change=millis();
}

int status = false;

void loop() {

  /* read pot value to see if it's changed */
  int tmp_temp = map(analogRead(POT_PIN),0,1023,MIN_TEMP,MAX_TEMP);
  if(tmp_temp!=goal_temperature)
  {
    lcd.clear();
    goal_temperature=tmp_temp;
    changing_temperature=true;
    last_change = millis();
    lcd.print("New T:");
    lcd.setCursor(0,1);
    lcd.print(goal_temperature);
    lcd.print("C");
  }

  // after change the goal temperature wait 2 second!
  if(changing_temperature)
  {
    if(millis()-last_change<2000)
      return;
    changing_temperature=false;
  }


  // read the value of sensor
  int chk = DHT11.read(SENSOR_PIN);
  print_data(chk);
  if(chk!=DHTLIB_OK)
    return;
  if(status)
    lcd.print("  On");
  else
    lcd.print("  Off");

  if(millis()-last_change>CHANGE_DELAY)
  {
    last_change = millis();
    if(DHT11.temperature<goal_temperature)
    {
      digitalWrite(RELAY_PIN,LOW);
      status=true;
    }
    else
    {
      digitalWrite(RELAY_PIN,HIGH);
      status=false;
    }
  }

  delay(500);
}

