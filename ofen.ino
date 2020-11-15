#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <string.h>
#include "driver/adc.h"

#define MAX_CLIENTS 5

#define DISPLAY_WIDTH 84
#define DISPLAY_HEIGHT 20
#define DISPLAY_LENGTH (DISPLAY_WIDTH * DISPLAY_HEIGHT * 3) + DISPLAY_HEIGHT

#define GPIO_COIL_A 22
#define GPIO_COIL_B 23
#define ADC_SPEED_READING ADC1_GPIO34_CHANNEL
#define ADC_SPEED_REFERENCE ADC1_GPIO35_CHANNEL
#define POINT_50 400
#define ADJUST_temperature_THRESHHOLD 50

WiFiClient *clients[MAX_CLIENTS] = { NULL };

const char *ssid = "esp32";
const char *password = "duBrauchstKeinPasswort";

WiFiServer server(80);

int rotary = 0;

int temperature = 0;

struct Color{
  unsigned char red;
  unsigned char green;
  unsigned char blue;
};

void IRAM_ATTR coilAReset();
void IRAM_ATTR coilBReset();

int adjusttemperature(){
  int speed = adc1_get_raw(ADC_SPEED_READING) - adc1_get_raw(ADC_SPEED_REFERENCE);
  if(speed < ADJUST_temperature_THRESHHOLD){
    return 1;
  }
  return pow(((double)speed - ADJUST_temperature_THRESHHOLD) / (POINT_50 - ADJUST_temperature_THRESHHOLD), 2) * 49 + 1;
}

void IRAM_ATTR coilA(){
  if(rotary == 0){
    rotary += 1;
  }else{
    temperature += adjusttemperature();
  }
  attachInterrupt(GPIO_COIL_A, coilAReset, FALLING);
}

void IRAM_ATTR coilB(){
  if(rotary == 0){
    rotary += 1;
  }else{
    temperature -= adjusttemperature();
  }
  attachInterrupt(GPIO_COIL_B, coilBReset, FALLING);
}

void IRAM_ATTR coilAReset(){
  rotary = 0;
  attachInterrupt(GPIO_COIL_A, coilA, RISING);
}

void IRAM_ATTR coilBReset(){
  rotary = 0;
  attachInterrupt(GPIO_COIL_B, coilB, RISING);
}

void display(){
  char displayChars[DISPLAY_LENGTH] = {};
  initializeDisplayChars(displayChars);
  struct Color displayForegroundInvColor[DISPLAY_WIDTH * DISPLAY_HEIGHT] = {};
  struct Color displayBackgroundColor[DISPLAY_WIDTH * DISPLAY_HEIGHT] = {};
  drawSeparationLines(displayChars);
  char appendString[10] = {};
  sprintf(appendString, "%d", temperature);

  appendText(displayChars, "12:34:56h1105oC/1255oC1496W");
  finishDisplayChars(displayChars);
  sendDisplay(displayChars);
}

void * positionToPointer(char *displayChars, int x, int y){
  return displayChars + y * (DISPLAY_WIDTH * 3 + 1) + x * 3;
}

void initializeDisplayChars(char displayChars[DISPLAY_LENGTH]){
  for(int i = DISPLAY_WIDTH * 3; i < DISPLAY_LENGTH - 1; i += DISPLAY_WIDTH * 3 + 1){
    displayChars[i] = '\n';
  }
}

void finishDisplayChars(char displayChars[DISPLAY_LENGTH]){
  int i = 0;
  while(i < DISPLAY_LENGTH - 3) {
    if(!displayChars[i]){
      displayChars[i] = ' ';
      displayChars[i + 1] = '\u001A';
      displayChars[i + 2] = '\u001A';
      // Serial.print("filled in voids at: ");
      // Serial.print(i);
      // Serial.print(", ");
      // Serial.print(i + 1);
      // Serial.print(" and ");
      // Serial.println(i + 2);
      i += 3;
    }else{
      i += 1;
    }
  }
}

String getColorCode(int iterator){
  iterator = iterator%1024;

  int r;
  int g;
  int b;

  if(iterator >= 0 && iterator < 256){
    r = 255;
    g = iterator - 0;
    b = 0;
  }
  if(iterator >= 256 && iterator < 512){
    r = 511 - iterator;
    g = 255;
    b = iterator - 256;
  }
  if(iterator >= 512 && iterator < 768){
    r = 0;
    g = 767 - iterator;
    b = 255;
  }
  if(iterator >= 768 && iterator < 1024){
    r = iterator - 768;
    g = 0;
    b = 1023 - iterator;
  }

  char output[51] = "\u001b[48;2;";
  char colorVal[12];

  sprintf(colorVal, "%d", r);
  strcat(output, colorVal);
  strcat(output, ";");

  sprintf(colorVal, "%d", g);
  strcat(output, colorVal);
  strcat(output, ";");

  sprintf(colorVal, "%d", b);
  strcat(output, colorVal);
  strcat(output, "m");

  return output;
}

void appendText(char displayChars[DISPLAY_LENGTH], char *text){
  int displayIterator = 0;
  while(displayChars[displayIterator] && displayIterator < DISPLAY_LENGTH){
    displayIterator++;
  }
  int distanceFromLineEnding = DISPLAY_WIDTH * 3 - (displayIterator + 1) % (DISPLAY_WIDTH * 3 + 1);
  if(distanceFromLineEnding >= 0 && distanceFromLineEnding <= 8){
    displayIterator += distanceFromLineEnding + 1;
  }
  char pattern[28];
  memcpy(pattern, letterToPattern(text[0]), 28);
  for(int i = 0; i < 3; i++){
    int displayCharsPos = displayIterator + i * (DISPLAY_WIDTH * 3 + 1);
    void *displayCharsPnt = displayChars + displayCharsPos;

    int patternPos = i * 9;
    void *patternPnt = pattern + patternPos;

    memcpy(displayCharsPnt, patternPnt, 9);
  }
  int textLen = strlen(text) - 1;
  if(textLen >= 1){
    appendText(displayChars, ++text);
  }
}

void drawSeparationLines(char displayChars[DISPLAY_LENGTH]){
  for(int i = 0; i < 3; i++){
    char line[4] = "┃";
    memcpy(positionToPointer(displayChars, 27, i), line, 3);
    memcpy(positionToPointer(displayChars, 67, i), line, 3);
  }
  for(int i = 0; i < DISPLAY_WIDTH; i++){
    char line[4] = "━";
    memcpy(positionToPointer(displayChars, i, 3), line, 3);
  }
  char line[4] = "┻";
  memcpy(positionToPointer(displayChars, 27, 3), line, 3);
  memcpy(positionToPointer(displayChars, 67, 3), line, 3);
}

void sendDisplay(char displayChars[DISPLAY_LENGTH]){
  for (int i=0 ; i<MAX_CLIENTS ; i++) {
    if (clients[i] == NULL) {
      break;
    }
    if(!(clients[i] -> connected())){
      clients[i] = NULL;
    }else{
      initializeClient(*clients[i]);
      clients[i] -> print(displayChars);
       //Serial.println(sizeof(displayChars));
      //Serial.print(displayChars);
    }
  }
}

void initializeClient(WiFiClient client){
  client.print("\u001b[0m\u001b[?25l\u001b[2J\u001b[1;1H"); //reset all graphic settings (SGR parameters), hide cursor, erase display, go to top left corner of display
}

char* letterToPattern(char letter){ //x, y and 3 chars for unicode
  // '━': \u2501, '┃': \u2503, '╱': \u2571, '╲': \u2572,
  // '╸': \u2578, '╹': \u2579, '╺': \u257A, '╻': \u257B
  // '┏': \u250F, '┓': \u2513, '┗': \u2517, '┛': \u251B,
  // '┣': \u2523, '┫': \u252B, '┳': \u2533, '┻': \u253B,
  char *pattern;
  switch (letter) {
    case '0':
      pattern = "┏━┓┃ \u001A\u001A┃┗━┛";
      break;
    case '1':
      pattern = " \u001A\u001A \u001A\u001A╻ \u001A\u001A \u001A\u001A┃ \u001A\u001A \u001A\u001A╹";
      break;
    case '2':
      pattern = "╺━┓┏━┛┗━╸";
      break;
    case '3':
      pattern = "╺━┓ \u001A\u001A━┫╺━┛";
      break;
    case '4':
      pattern = "╻ \u001A\u001A╻┗━┫ \u001A\u001A \u001A\u001A╹";
      break;
    case '5':
      pattern = "┏━╸┗━┓╺━┛";
      break;
    case '6':
      pattern = "┏━╸┣━┓┗━┛";
      break;
    case '7':
      pattern = "╺━┓ \u001A\u001A \u001A\u001A┃ \u001A\u001A \u001A\u001A╹";
      break;
    case '8':
      pattern = "┏━┓┣━┫┗━┛";
      break;
    case '9':
      pattern = "┏━┓┗━┫╺━┛";
      break;
    case '/':
      pattern = " \u001A\u001A \u001A\u001A╱ \u001A\u001A╱ \u001A\u001A╱ \u001A\u001A \u001A\u001A";
      break;
    case 'o':
      pattern = "┏━┓┗━┛ \u001A\u001A \u001A\u001A \u001A\u001A";
      break;
    case 'C':
      pattern = "┏━╸┃ \u001A\u001A \u001A\u001A┗━╸";
      break;
    case 'h':
      pattern = "╻ \u001A\u001A \u001A\u001A┣━┓╹ \u001A\u001A╹";
      break;
    case ':':
      pattern = " \u001A\u001A╻ \u001A\u001A \u001A\u001A \u001A\u001A \u001A\u001A \u001A\u001A╹ \u001A\u001A";
      break;
    case 'W':
      pattern = "╻ \u001A\u001A╻┃┃┃┗┻┛";
      break;
    default:
      pattern = " \u001A\u001A \u001A\u001A \u001A\u001A \u001A\u001A \u001A\u001A \u001A\u001A \u001A\u001A \u001A\u001A \u001A\u001A";
  }
  return pattern;
}

void setup() {
  Serial.begin(115200);

  pinMode(GPIO_COIL_A, INPUT);
  pinMode(GPIO_COIL_B, INPUT);
  attachInterrupt(GPIO_COIL_A, coilA, RISING);
  attachInterrupt(GPIO_COIL_B, coilB, RISING);

  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC_SPEED_READING, ADC_ATTEN_DB_11);
  adc1_config_channel_atten(ADC_SPEED_REFERENCE, ADC_ATTEN_DB_11);

  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");
}

void loop() {
  WiFiClient newClient = server.available();   // listen for incoming clients

  if(newClient){
    for (int i=0 ; i<MAX_CLIENTS ; i++) {
      if (clients[i] == NULL) {
          clients[i] = new WiFiClient(newClient);
          initializeClient(newClient);
          break;
      }
    }
  }
  display();

  /*char displayChars[DISPLAY_LENGTH] = {};
  initializeDisplayChars(displayChars);
  displayChars[0] = 'a';
  sendDisplay(displayChars);*/
  delay(100);
}
