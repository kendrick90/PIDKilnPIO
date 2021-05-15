#include "mini12864.h"

/* Constructors */
//U8G2_UC1701_MINI12864_F_4W_SW_SPI u8g2(U8G2_R0, SPI_CLK_PIN, SPI_MOSI_PIN, SPI_LCD_CS_PIN, LCD_A0_DC_PIN, LCD_RESET_PIN);
U8G2_UC1701_MINI12864_F_4W_HW_SPI u8g2(ROTATION, SPI_LCD_CS_PIN, LCD_A0_DC_PIN, LCD_RESET_PIN);
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_RGB + NEO_KHZ800);
File myFile;
ESP32Encoder encoder;
Switch pushButton1 = Switch(BTN_ENC);
Switch pushButton2 = Switch(EXTRA_BUTTON_PIN);

void configure_serial(void)
{
  Serial.begin(115200);
  Serial.println("Serial setup.");
}

void configure_lcd(void)
{
  u8g2.begin();
  u8g2.setContrast(LCD_CONTRAST);
  u8g2.firstPage();
  do
  {
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0, 20, "Hello World!");
  } while (u8g2.nextPage());
}

void configure_leds(void)
{
  strip.begin();                   // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();                    // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void configure_sd(void)
{
  Serial.print("Initializing SD card...");

  if (!SD.begin(SPI_SD_CS_PIN))
  {
    Serial.println("initialization failed!");
    while (1)
      ;
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("//test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile)
  {
    Serial.print("Writing to /test.txt...");
    myFile.println("testing 1, 2, 3.");
    // close the file:
    myFile.close();
    Serial.println("done.");
  }
  else
  {
    // if the file didn't open, print an error:
    Serial.println("error opening /test.txt");
  }

  // re-open the file for reading:
  myFile = SD.open("/test.txt");
  if (myFile)
  {
    Serial.println("Reading from /test.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available())
    {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
    Serial.println("Test Passed!");
  }
  else
  {
    // if the file didn't open, print an error:
    Serial.println("error opening /test.txt");
  }
}

void configure_encoder(void)
{
  // Enable the weak pull down resistors
  ESP32Encoder::useInternalWeakPullResistors=DOWN;

  // Enable the weak pull up resistors
  // ESP32Encoder::useInternalWeakPullResistors = UP;

  // use pin BTN_EN1 and BTN_EN2 for the first encoder
  encoder.attachSingleEdge(BTN_EN1, BTN_EN2);
  // set starting count value after attaching
  encoder.setCount(0);

  // clear the encoder's raw count and set the tracked count to zero
  Serial.print("Encoder Start = ");
  Serial.println((int32_t)encoder.getCount());
}

void testBuzzer(void)
{
  tone(BUZZER_PIN, NOTE_C4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_D4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_E4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_F4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_G4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_A4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
  tone(BUZZER_PIN, NOTE_B4, 500, BUZZER_CHANNEL);
  noTone(BUZZER_PIN, BUZZER_CHANNEL);
}

uint32_t Wheel(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3, 0);
  }
  if (WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0, 0);
}

int j = 0;
uint32_t k = 0;
void rainbowCycle(uint32_t LED_SLOWDOWN)
{
  uint16_t i;
  if (k == LED_SLOWDOWN)
  {
    for (i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j)));
    }
    strip.show();
    j++;
    if (j > 256 || j < 0)
    {        //if it's out of range
      j = 0; //reset j
    }
  }
  k++;
  if (k > LED_SLOWDOWN || k < 0)
  {        //if it's out of range
    k = 0; //reset it
  }
}

uint64_t lastEncoderCount = 0;
void doEncoder(void)
{
  if (lastEncoderCount != encoder.getCount())
  {
    Serial.print("Encoder Count = ");
    Serial.println((int32_t)encoder.getCount());
  }
  lastEncoderCount = encoder.getCount();
}

int p, q;
void doPushButtons(void)
{
  pushButton1.poll();
  if (pushButton1.switched())
    Serial.print("pushButton1 switched ");
  if (pushButton1.pushed())
  {
    Serial.print("pushButton1 pushed ");
    Serial.print(++p);
    Serial.println(" times");
  }
  if (pushButton1.released())
    Serial.println("pushButton1 released");
  pushButton2.poll();
  if (pushButton2.switched())
    Serial.print("pushButton2 switched ");
  if (pushButton2.pushed())
  {
    Serial.print("pushButton2 pushed ");
    Serial.print(++q);
    Serial.println(" times");
  }
  if (pushButton2.released())
    Serial.println("pushButton2 released");
}