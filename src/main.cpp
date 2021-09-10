#include "Wire.h"
#include "RTClib.h"
#include <LiquidCrystal_SR_LCD3.h>
#include <Arduino.h>

// Tipo canal
// 0 = LEDS curva de gamma aplicada
// 1 = On/Off

                  //{ CH#1, CH#2, CH#3, CH#4, CH#5, CH#6, CH#7, CH#8, CH#9, CH#10 }
int tipoCanal[10] = {    0,    0,    0,    0,    1,    0,    0,    0,    0,    0};

int entry[112][13] = {
    /*
   * CH#1 = R
   * CH#2 = G
   * CH#3 = B
   * CH#4 = W
   * CH#5 = CO2
   */
//      /       R     G     B     W    CO2
//  {  H,  M, CH#1, CH#2, CH#3, CH#4, CH#5, CH#6, CH#7, CH#8, CH#9, CH#10}
    {  0,  0,    0,    0,    5,    0,    0,    0,    0,    0,    0,    0 },
    {  4, 30,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
    {  5,  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
    { 10,  0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
    { 11,  0,    0,    0,    0,    0,    1,    0,    0,    0,    0,    0 },// Liga o CO2
    { 12,  0,    0,    0,    0,    0,    1,    0,    0,    0,    0,    0 },
    { 12, 30,   90,   45,   25,  100,    1,    0,    0,    0,    0,    0 },
    { 19,  0,   90,   45,   25,  100,    0,    0,    0,    0,    0,    0 },// Desliga CO2
    { 20,  0,   90,   45,   25,  100,    0,    0,    0,    0,    0,    0 },
    { 20, 30,    0,    0,    5,    0,    0,    0,    0,    0,    0,    0 },
    { 23, 59,    0,    0,    5,    0,    0,    0,    0,    0,    0,    0 },
};

const unsigned char cie[256] = {
    0,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    2,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    3,
    4,
    4,
    4,
    4,
    4,
    4,
    5,
    5,
    5,
    5,
    5,
    6,
    6,
    6,
    6,
    6,
    7,
    7,
    7,
    7,
    8,
    8,
    8,
    8,
    9,
    9,
    9,
    10,
    10,
    10,
    10,
    11,
    11,
    11,
    12,
    12,
    12,
    13,
    13,
    13,
    14,
    14,
    15,
    15,
    15,
    16,
    16,
    17,
    17,
    17,
    18,
    18,
    19,
    19,
    20,
    20,
    21,
    21,
    22,
    22,
    23,
    23,
    24,
    24,
    25,
    25,
    26,
    26,
    27,
    28,
    28,
    29,
    29,
    30,
    31,
    31,
    32,
    32,
    33,
    34,
    34,
    35,
    36,
    37,
    37,
    38,
    39,
    39,
    40,
    41,
    42,
    43,
    43,
    44,
    45,
    46,
    47,
    47,
    48,
    49,
    50,
    51,
    52,
    53,
    54,
    54,
    55,
    56,
    57,
    58,
    59,
    60,
    61,
    62,
    63,
    64,
    65,
    66,
    67,
    68,
    70,
    71,
    72,
    73,
    74,
    75,
    76,
    77,
    79,
    80,
    81,
    82,
    83,
    85,
    86,
    87,
    88,
    90,
    91,
    92,
    94,
    95,
    96,
    98,
    99,
    100,
    102,
    103,
    105,
    106,
    108,
    109,
    110,
    112,
    113,
    115,
    116,
    118,
    120,
    121,
    123,
    124,
    126,
    128,
    129,
    131,
    132,
    134,
    136,
    138,
    139,
    141,
    143,
    145,
    146,
    148,
    150,
    152,
    154,
    155,
    157,
    159,
    161,
    163,
    165,
    167,
    169,
    171,
    173,
    175,
    177,
    179,
    181,
    183,
    185,
    187,
    189,
    191,
    193,
    196,
    198,
    200,
    202,
    204,
    207,
    209,
    211,
    214,
    216,
    218,
    220,
    223,
    225,
    228,
    230,
    232,
    235,
    237,
    240,
    242,
    245,
    247,
    250,
    252,
    255,
    255,
    255,
    255,
    255,
};

// 4094
// pin 1 = STROBE
// pin 2 = DATA
// pin 3 = CLOCK

const int PIN_LCD_STROBE = 22;    // Out: LCD IC4094 shift-register strobe
const int PIN_LCD_DATA = 24;      // Out: LCD IC4094 shift-register data
const int PIN_LCD_CLOCK = 26;     // Out: LCD IC4094 shift-register clock
const int PIN_LCD_BACKLIGHT = 28; // Out: LCD backlight (PWM)

#define sensorTemp A0
int numSamples = 16; // Numero de amostras antes de mostrar no LCD

// Inicializacao das bibliotecas

RTC_DS1307 rtc;
DateTime now;
LiquidCrystal_SR_LCD3 lcd(PIN_LCD_DATA, PIN_LCD_CLOCK, PIN_LCD_STROBE);

//Array que desenha o simbolo de grau
byte a[8] = {
    B00110,
    B01001,
    B00110,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
};

// Array para precisão na medição de temperatura
int contaSamples = 0;         // Contagem das amostras
unsigned long sumSamples = 0; // Soma

long ultimaInteracao = 0;
int sofEntry;
int nowH;
int nowM;
int nowS;

int updateCounter = 3; // quantas rotinas a serem atualizadas

long unsigned updateInterval[5] = { // milisegundos
    1000,                           // relogio();
    1000,                           // updateBrightness();
    1000,                           // exibeRelogio();
    60,                             // exibeTemp();
    0};
long unsigned lastRun[5] = {0,  0, 0,  0, 0};

void relogio();
void updateBrightness();
void exibeRelogio();
void exibeTemp();

void setup()
{
  sofEntry = sizeof(entry) / sizeof(entry[0]);

  pinMode(PIN_LCD_BACKLIGHT, OUTPUT);
  digitalWrite(PIN_LCD_BACKLIGHT, 80);
  lcd.begin(16, 2);
  lcd.createChar(1, a);

  Serial.begin(115200);
  delay(1000);
  analogReference(INTERNAL1V1);

  pinMode(13, OUTPUT);
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    while (1)
      ;
  }
  if (!rtc.isrunning())
  {
    Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  for (int z = 2; z <= 11; z++)
  {
    // Saida padrao (PWM/ONOFF)
    if (tipoCanal[z - 2] == 0)
    {
      pinMode(z, OUTPUT);
      digitalWrite(z, 0);
      Serial.print("Pin ");
      Serial.print(z);
      Serial.println(" PWM output");
    }
    if (tipoCanal[z - 2] == 1)
    {
      pinMode(z, OUTPUT);
      digitalWrite(z, 0);
      Serial.print("Pin ");
      Serial.print(z);
      Serial.println(" ON/OFF output");
    }
  }
}

void loop()
{
  long unsigned actMillis = millis();

  for (int x = 0; x <= updateCounter; x++)
  {
    if ((actMillis - lastRun[x]) > updateInterval[x])
    {
      lastRun[x] = actMillis;
      switch (x)
      {
      case 0:
        relogio();
        break;
      case 1:
        updateBrightness();
        break;
      case 2:
        exibeRelogio();
        break;
      case 3:
        exibeTemp();
        break;
      }
    }
  }
}
void relogio()
{
  now = rtc.now();
  nowH = now.hour();
  nowM = now.minute();
  nowS = now.second();

  while ((nowH > 24) || (nowM > 60) || (nowS > 60))
  {
    Serial.print("\nRTC ERROR: ");
    if (nowH < 10)
    {
      Serial.print("0");
    }
    Serial.print(nowH);
    Serial.print(":");
    if (nowM < 10)
    {
      Serial.print("0");
    }
    Serial.print(nowM);
    Serial.print(":");
    if (nowS < 10)
    {
      Serial.print("0");
    }
    Serial.print(nowS);
    Serial.print("\t");
    if (!rtc.begin())
    {
      Serial.println("Couldn't find RTC");
      while (1)
        ;
    }
    if (!rtc.isrunning())
    {
      Serial.println("RTC is NOT running!");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }
    now = rtc.now();
    nowH = now.hour();
    nowM = now.minute();
    nowS = now.second();
    Serial.print("Retry... : ");
    delay(1000);
  }
  if (nowH < 10)
  {
    Serial.print("0"); 
  }
  Serial.print(nowH);
  Serial.print(":");
  if (nowM < 10)
  {
    Serial.print("0");
  }
  Serial.print(nowM);
  Serial.print(":");
  if (nowS < 10)
  {
    Serial.print("0");
  }
  Serial.print(nowS);
  Serial.print("\t");
}

void exibeRelogio()
{
  if (nowH < 10)
  {
    lcd.setCursor(0, 0);
    lcd.print(0, DEC);
    lcd.setCursor(1, 0);
    lcd.print(nowH, DEC);
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print(nowH, DEC);
  }

  lcd.setCursor(2, 0);
  lcd.print(":");

  if (nowM < 10)
  {
    lcd.setCursor(3, 0);
    lcd.print(0, DEC);
    lcd.setCursor(4, 0);
    lcd.print(nowM, DEC);
  }
  else
  {
    lcd.setCursor(3, 0);
    lcd.print(nowM, DEC);
  }

  lcd.setCursor(5, 0);
  lcd.print(":");

  if (nowS < 10)
  {
    lcd.setCursor(6, 0);
    lcd.print(0, DEC);
    lcd.setCursor(7, 0);
    lcd.print(nowS, DEC);
  }
  else
  {
    lcd.setCursor(6, 0);
    lcd.print(nowS, DEC);
  }
}
void exibeTemp()
{
  if (contaSamples <= numSamples)
  {
    sumSamples += analogRead(sensorTemp);
    contaSamples++;
  }
  else
  {

    float tempC = (sumSamples / numSamples) / 9.62;
    contaSamples = 1;
    sumSamples = 0;
    lcd.setCursor(10, 0);
    lcd.print(tempC, 2);
    lcd.setCursor(15, 0); //Coloca o cursor na coluna 7, linha 1
    lcd.write(1);         //Escreve o simbolo de grau
  }
}

int findCurrentEntryIdx(int nowH, int nowM)
{
  int foundIdx = sofEntry - 1;

  int entryH, entryM;
  for (int i = 0; i < sofEntry; i++)
  {
    entryH = entry[i][0];
    entryM = entry[i][1];
    if (nowH > entryH)
    {
      foundIdx = i;
      continue;
    }
    if (nowH == entryH && nowM >= entryM)
    {
      foundIdx = i;
      continue;
    }
    break;
  }
  return foundIdx;
}

int getNextEntryIdx(int curEntryIdx)
{
  int nextIdx;
  int foundIdx = sofEntry - 1;
  if (curEntryIdx + 1 > foundIdx)
  {
    nextIdx = 0; // Volta pra cima.
  }
  else
  {
    nextIdx = curEntryIdx + 1;
  }
  return nextIdx;
}

long unsigned int getAsSeconds(int h, int m, int s)
{
  return (h * 60 * 60L) + (m * 60L) + s;
}

long floatMap(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (float)(x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
void updateBrightness()
{

  int curEntryIdx = findCurrentEntryIdx(nowH, nowM);
  int nextEntryIdx = getNextEntryIdx(curEntryIdx);

  long unsigned int initTime = getAsSeconds(entry[curEntryIdx][0], entry[curEntryIdx][1], 0);
  long unsigned int finalTime = getAsSeconds(entry[nextEntryIdx][0], entry[nextEntryIdx][1], 0);
  long unsigned int curTime = getAsSeconds(nowH, nowM, nowS);

  if (finalTime < initTime)
  {
    finalTime += getAsSeconds(24,  0, 0) + 0L;
  }
  if (curTime < initTime)
  {
    curTime += getAsSeconds(24,  0, 0) + 0L;
  }

  float pcElapsed = ((float)curTime - (float)initTime) / ((float)finalTime - (float)initTime);

  for (int z = 2; z <= 11; z++)
  {
    int initBrightness = entry[curEntryIdx][z];
    int finalBrightness = entry[nextEntryIdx][z];
    float setBrightness = initBrightness + pcElapsed * (finalBrightness - initBrightness);

    // CANAL PWM
    if (tipoCanal[z - 2] == 0)
    {
      if (setBrightness == 1.0)
      {
        analogWrite(z, 1);
      }
      else
      {
        analogWrite(z, cie[(int)floatMap(setBrightness, 0.0, 100.0, 0.0, 255.0)]);
      }
      Serial.print("(");
      Serial.print(cie[(int)floatMap(setBrightness, 0.0, 100.0, 0.0, 255.0)]);
      Serial.print(")");
      Serial.print(setBrightness, 0);
      Serial.print("%\t");
    }
    // Canal ON/OFF
    else if (tipoCanal[z - 2] == 1)
    {
      if (initBrightness > 0)
      {
        digitalWrite(z, HIGH);
      }
      else if (initBrightness == 0)
      {
        digitalWrite(z, LOW);
      }
      if (digitalRead(z) == 1)
      {
        Serial.print("ON \t");
      }
      else
      {
        Serial.print("OFF \t");
      }
    }
  }
  Serial.println("");
}