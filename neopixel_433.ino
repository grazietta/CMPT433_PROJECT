/*code to manipulate LED neopixels given a string received through serial*/
//reference :
//https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/
//https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define MAX_PACE 50
#define MIN_PACE 0
#define PIN 8
#define NUM_PIXELS 60

int speedDelay = 10;

String incomingString;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

String mode[] = {"none", "fadeinout", "twinkle", "sparkle", "rainbow"};
String *ptr = mode;
String currentMode = "fadeinout";
byte rgb_r = 255;
byte rgb_g = 255;
byte rgb_b = 255;

void setup()
{
  strip.begin();

  Serial.begin(9600);
  Serial.setTimeout(5);
}

void loop()
{

  incomingString = "0";
  if (Serial.available() > 0)
  {

    incomingString = Serial.readString();
    Serial.print("I received: ");
    Serial.println(incomingString);
  }
  if (incomingString == "mode1")
  {
    defalut_para();
    currentMode = "none";
  }
  else if (incomingString == "mode2")
  {
    defalut_para();
    currentMode = "fadeinout";
  }
  else if (incomingString == "mode3")
  {
    defalut_para();
    currentMode = "twinkle";
  }
  else if (incomingString == "mode4")
  {
    defalut_para();
    currentMode = "bounce";
  }
  else if (incomingString == "mode5")
  {
    defalut_para();
    currentMode = "rainbow";
  }
  else if (incomingString == "bright1")
  {
    set_Brightness(0);
  }
  else if (incomingString == "bright2")
  {
    set_Brightness(10);
  }
  else if (incomingString == "bright3")
  {
    set_Brightness(120);
  }
  else if (incomingString == "bright4")
  {
    set_Brightness(180);
  }
  else if (incomingString == "bright5")
  {
    set_Brightness(255);
  }
  else if (incomingString == "clap")
  {
    set_Speed(1);
    currentMode = "rainbow";
  }
  else if (incomingString == "color1")
  {
    rgb_r = 255;
    rgb_g = 0;
    rgb_b = 0;
  }
  else if (incomingString == "color2")
  {
    rgb_r = 0;
    rgb_g = 255;
    rgb_b = 0;
  }
  else if (incomingString == "color3")
  {
    rgb_r = 0;
    rgb_g = 0;
    rgb_b = 255;
  }
  else if (incomingString == "color4")
  {
    rgb_r = 255;
    rgb_g = 255;
    rgb_b = 255;
  }
  else if (incomingString == "speed1")
  {
    set_Speed(0);
  }
  else if (incomingString == "speed2")
  {
    set_Speed(10);
  }
  else if (incomingString == "speed3")
  {
    set_Speed(20);
  }
  else if (incomingString == "smile")
  {
    defalut_para();
    currentMode = "smile";
  }
  else if (incomingString == "frown")
  {
    defalut_para();
    currentMode = "frown";
  }
  playCurrentMode();
}
void defalut_para()
{
  rgb_r = 255;
  rgb_g = 255;
  rgb_b = 255;
  set_Brightness(255);
  set_Speed(10);
}

void playCurrentMode()
{
  if (currentMode == "none")
  {
    clear();
  }
  else if (currentMode == "fadeinout")
  {
    fadeinout(rgb_r, rgb_g, rgb_b);
  }
  else if (currentMode == "twinkle")
  {
    twinkle(10);
  }
  else if (currentMode == "sparkle")
  {
    sparkle(rgb_r, rgb_g, rgb_b);
  }
  else if (currentMode == "rainbow")
  {
    rainbow();
  }
  else if (currentMode == "bounce")
  {
    bounce();
  }
  else if (currentMode == "smile")
  {
    green();
  }
  else if (currentMode == "frown")
  {
    pink();
  }
}

void pink()
{
  setAllPixels(255, 0, 0);
}
void green()
{
  setAllPixels(0, 255, 0);
}

int getSpeed()
{
  Serial.print("speed: ");
  Serial.println(speedDelay);
  return speedDelay;
}

void set_Speed(int s)
{
  if (s >= 0 && s <= 100)
  {
    speedDelay = s;
  }
}

void set_Brightness(int bright)
{
  strip.setBrightness(bright);
  Serial.println(bright);
}

void get_Brightness()
{
  Serial.print("brightness: ");
  Serial.println(strip.getBrightness());
}

void setPixelColor(uint16_t i, uint8_t r, uint8_t g, uint8_t b)
{
  if (i < 60 && i >= 0)
  {
    strip.setPixelColor(i, r, g, b);
  }
}

void setAllPixels(uint8_t r, uint8_t g, uint8_t b)
{
  for (int i = 0; i < NUM_PIXELS; i++)
  {
    setPixelColor(i, r, g, b);
  }
  show();
}

void show()
{
  strip.show();
}

void fadein(byte r, byte g, byte b)
{

  for (int k = 0; k < 256; k++)
  {
    for (int i = 0; i < NUM_PIXELS; i++)
    {
      setPixelColor(i, r * k / 255, g * k / 255, b * k / 255);
    }
    show();
    delay(0);
  }
}

void fadeout(byte r, byte g, byte b)
{
  for (int k = 255; k >= 0; k--)
  {
    for (int i = 0; i < NUM_PIXELS; i++)
    {
      setPixelColor(i, r * k / 255, g * k / 255, b * k / 255);
    }
    show();
    delay(0);
  }
}

void fadeinout(byte r, byte g, byte b)
{
  fadein(r, g, b);
  fadeout(r, g, b);
}

void twinkle(int count)
{
  clear();
  for (int i = 0; i < count; i++)
  {
    setPixelColor(random(NUM_PIXELS), random(255), random(255), random(255));
    show();
    delay(speedDelay);
  }
}

void clear()
{
  setAllPixels(0, 0, 0);
}

void sparkle(byte r, byte g, byte b)
{
  setPixelColor(random(NUM_PIXELS), r, g, b);
  delay(speedDelay);
  show();
  clear();
}

void bounce()
{

  for (int i = 0; i < NUM_PIXELS; i++)
  {
    clear();
    for (int j = i; j < i + 4; j++)
    {
      setPixelColor(j, rgb_r, rgb_g, rgb_b);
    }
    show();
    delay(speedDelay);
  }

  for (int i = 59; i >= 0; i--)
  {
    clear();
    for (int j = i; j > i - 4; j--)
    {
      setPixelColor(j, rgb_r, rgb_g, rgb_b);
    }
    show();
    delay(speedDelay);
  }
}

//https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/
//I did not code rainbow function
void rainbow()
{
  byte *color;
  uint16_t i, j;

  for (j = 0; j < 256; j++)
  { //
    for (i = 0; i < NUM_PIXELS; i++)
    {
      color = wheel(((i * 256 / NUM_PIXELS) + j) & 255);
      strip.setPixelColor(i, *color, *(color + 1), *(color + 2));
    }
    strip.show();
    delay(speedDelay);
  }
}
//https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/
//I did not code wheel function
byte *wheel(byte WheelPos)
{
  static byte color[3];

  if (WheelPos < 85)
  {
    color[0] = WheelPos * 3;
    color[1] = 255 - WheelPos * 3;
    color[2] = 0;
  }
  else if (WheelPos < 170)
  {
    WheelPos -= 85;
    color[0] = 255 - WheelPos * 3;
    color[1] = 0;
    color[2] = WheelPos * 3;
  }
  else
  {
    WheelPos -= 170;
    color[0] = 0;
    color[1] = WheelPos * 3;
    color[2] = 255 - WheelPos * 3;
  }

  return color;
}
