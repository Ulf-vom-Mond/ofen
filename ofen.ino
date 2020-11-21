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
#define ADJUST_TEMPERATURE_THRESHHOLD 50

#define SETPOINTS_COUNT 12
#define TIME_AXIS_LENGTH (DISPLAY_WIDTH - 4 - 1 - 1 - 12)
#define TEMP_AXIS_LENGTH (DISPLAY_HEIGHT - 4 - 1)
#define ORIGIN_X 4
#define ORIGIN_Y DISPLAY_HEIGHT - 2

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
  if(speed < ADJUST_TEMPERATURE_THRESHHOLD){
    return 1;
  }
  return pow(((double)speed - ADJUST_TEMPERATURE_THRESHHOLD) / (POINT_50 - ADJUST_TEMPERATURE_THRESHHOLD), 2) * 49 + 1;
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

  drawDiagram(displayChars);
  fillTable(displayChars);

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

struct line{
  int start_x;
  int start_y;
  int len;
  unsigned int direction : 1; //0 means x, 1 means y
};

void drawSeparationLines(char displayChars[DISPLAY_LENGTH]){
  struct line lines[] = {
    { .start_x = DISPLAY_WIDTH - 12, .start_y = 4,                               .len = 12,                 .direction = 0},
    { .start_x = DISPLAY_WIDTH - 12, .start_y = 6,                               .len = 12,                 .direction = 0},
    { .start_x = DISPLAY_WIDTH - 12, .start_y = DISPLAY_HEIGHT - 1,              .len = 12,                 .direction = 0},
    { .start_x = DISPLAY_WIDTH - 12, .start_y = 4,                               .len = DISPLAY_HEIGHT - 4, .direction = 1},
    { .start_x = DISPLAY_WIDTH - 6,  .start_y = 4,                               .len = DISPLAY_HEIGHT - 4, .direction = 1},
    { .start_x = DISPLAY_WIDTH - 1,  .start_y = 4,                               .len = DISPLAY_HEIGHT - 4, .direction = 1},
    { .start_x = 0,                  .start_y = 3,                               .len = DISPLAY_WIDTH,      .direction = 0},
    { .start_x = 27,                 .start_y = 0,                               .len = 4,                  .direction = 1},
    { .start_x = 67,                 .start_y = 0,                               .len = 4,                  .direction = 1},
    { .start_x = ORIGIN_X,           .start_y = ORIGIN_Y - TEMP_AXIS_LENGTH + 1, .len = TEMP_AXIS_LENGTH,   .direction = 1},
    { .start_x = ORIGIN_X,           .start_y = ORIGIN_Y,                        .len = TIME_AXIS_LENGTH,   .direction = 0}
  };

  int lines_len = sizeof(lines) / sizeof(line);

  for (size_t i = 0; i < lines_len; i++) {
    int end_x_i = lines[i].start_x + (lines[i].len - 1) * (1 - lines[i].direction);
    int end_y_i = lines[i].start_y + (lines[i].len - 1) * lines[i].direction;

    for (size_t j = 0; j < lines[i].len; j++) {
      int current_x_pos = lines[i].start_x + (1 - lines[i].direction) * j;
      int current_y_pos = lines[i].start_y + lines[i].direction * j;

      char *str = (char*)calloc(4, sizeof(char));
      str[0] = '\0';

      for (size_t k = 0; k < lines_len; k++) {
        // we dont care about: the same line, parallel lines, lines which arent crossed
        if(k == i){
          k++;
        }
        if((lines[k].direction != lines[i].direction) && //checks if lines arent parallel
          (((current_x_pos == lines[k].start_x) && !lines[i].direction) || ((current_y_pos == lines[k].start_y) && lines[i].direction)) && // check if the current pos is a intersection with the current line
          (current_x_pos >= lines[k].start_x && current_x_pos <= lines[k].start_x + lines[k].len && current_y_pos >= lines[k].start_y && current_y_pos <= lines[k].start_y + lines[k].len)){ // check if the intersection lays within the start and end of the current line

          int end_x_k = lines[k].start_x + (lines[k].len - 1) * (1 - lines[k].direction);
          int end_y_k = lines[k].start_y + (lines[k].len - 1) * lines[k].direction;

          strncpy(str, "╋", 4);

          if(current_x_pos == lines[i].start_x && current_y_pos == lines[i].start_y){
            if(lines[i].direction){ // y
              strncpy(str, "┳", 4);
            }else{ // x
              strncpy(str, "┣", 4);
            }
          }
          if(current_x_pos == lines[k].start_x && current_y_pos == lines[k].start_y){
            if(lines[k].direction){ // y
              strncpy(str, "┳", 4);
            }else{ // x
              strncpy(str, "┣", 4);
            }
          }
          if(current_x_pos == end_x_i && current_y_pos == end_y_i){
            if(lines[i].direction){ // y
              strncpy(str, "┻", 4);
            }else{ // x
              strncpy(str, "┫", 4);
            }
          }
          if(current_x_pos == end_x_k && current_y_pos == end_y_k){
            if(lines[k].direction){ // y
              strncpy(str, "┻", 4);
            }else{ // x
              strncpy(str, "┫", 4);
            }
          }

          if(current_x_pos == lines[i].start_x && current_y_pos == lines[i].start_y && current_x_pos == lines[k].start_x && current_y_pos == lines[k].start_y){
            strncpy(str, "┏", 4);
          }
          if(current_x_pos == lines[i].start_x && current_y_pos == lines[i].start_y && current_x_pos == end_x_k && current_y_pos == end_y_k){
            if(lines[k].direction){ // y
              strncpy(str, "┗", 4);
            }else{ // x
              strncpy(str, "┓", 4);
            }          }
          if(current_x_pos == end_x_i && current_y_pos == end_y_i && current_x_pos == lines[k].start_x && current_y_pos == lines[k].start_y){
            if(lines[k].direction){ // y
              strncpy(str, "┓", 4);
            }else{ // x
              strncpy(str, "┗", 4);
            }          }
          if(current_x_pos == end_x_i && current_y_pos == end_y_i && current_x_pos == end_x_k && current_y_pos == end_y_k){
            strncpy(str, "┛", 4);
          }
        }
      }

      if(str[0] == '\0'){
        if(lines[i].direction){
          strncpy(str, "┃", 4);
        }else{
          strncpy(str, "━", 4);
        }
      }

      memcpy(positionToPointer(displayChars, current_x_pos, current_y_pos), str, 3);
      free(str);
    }

  }
}

int getDigitCount(int num){
  if(num < 10){
    return 1;
  }else{
    return getDigitCount(num / 10) + 1;
  }
}

void addIntToDisplay(char displayChars[DISPLAY_LENGTH], int x, int y, int num){
  int numLen = getDigitCount(num);
  char numString[numLen * 3 + 1] = {};
  for (size_t i = 0; i < numLen * 2; i++) {
    numString[i] = '\u001A';
  }
  sprintf(&numString[numLen * 2], "%d", (int)num);
  memcpy(positionToPointer(displayChars, x - numLen + 1, y), numString, numLen * 3);
}

void formatTime(char *dest, int minutes){
  int hours = (minutes - minutes % 60) / 60;
  minutes = minutes % 60;

  size_t i;
  for (i = 0; i < 2 - getDigitCount(hours); i++) {
    strncpy(dest + i, "0", 1);
  }
  sprintf(dest + i, "%d", hours);
  strncpy(dest + 2, ":", 1);
  for (i = 0; i < 2 - getDigitCount(minutes); i++) {
    strncpy(dest + 3 + i, "0", 1);
  }
  sprintf(dest + 3 + i, "%d", minutes);
  strncpy(dest + 5, "\u001A\u001A\u001A\u001A\u001A\u001A\u001A\u001A\u001A\u001A", 10);
}

int setpoints[SETPOINTS_COUNT][2] = {
  {0, 0},
  {20, 100},
  {60, 800},
  {90, 800},
  {100, 600},
  {180, 1000},
  {200, 0}
};

void drawCoordinateSystem(char displayChars[DISPLAY_LENGTH], int maxTime, int maxTemp){
  strncpy((char*)positionToPointer(displayChars, 0, 4), "T\u001A\u001A/\u001A\u001A°\u001AC\u001A\u001A^\u001A\u001A", 5 * 3);
  strncpy((char*)positionToPointer(displayChars, 4 + TIME_AXIS_LENGTH, DISPLAY_HEIGHT - 2), ">\u001A\u001A", 3);
  strncpy((char*)positionToPointer(displayChars, 4 + TIME_AXIS_LENGTH - 2, DISPLAY_HEIGHT - 1), "t\u001A\u001A/\u001A\u001Ah\u001A\u001A", 3 * 3);
  strncpy((char*)positionToPointer(displayChars, ORIGIN_X, ORIGIN_Y), "╋", 3);
  strncpy((char*)positionToPointer(displayChars, ORIGIN_X - 1, ORIGIN_Y + 1), "0\u001A\u001A", 3);

  for (int i = 2; i < TEMP_AXIS_LENGTH - 1; i += 2) {
    int tempVal = maxTemp * i / (TEMP_AXIS_LENGTH - 1);
    addIntToDisplay(displayChars, 3, 18 - i, tempVal);
    strncpy((char*)positionToPointer(displayChars, 4, 18 - i), "╋", 3);
  }

  for (int i = 10; i < TIME_AXIS_LENGTH - 1; i += 10) {
    int timeVal = maxTime * i / (TIME_AXIS_LENGTH - 1);
    formatTime((char*)positionToPointer(displayChars, 2 + i, 19), timeVal);
    strncpy((char*)positionToPointer(displayChars, 4 + i, 18), "╋", 3);
  }
}

void drawGraph(char displayChars[DISPLAY_LENGTH], int maxTime, int maxTemp){
  for (int i = 0; i < TIME_AXIS_LENGTH - 1; i++) {
    int lastSetpoint = 0;
    int currentTime = i * maxTime / (TIME_AXIS_LENGTH - 1);
    while(setpoints[lastSetpoint + 1][0] < currentTime && lastSetpoint < SETPOINTS_COUNT - 1) {
      lastSetpoint++;
    }
    if(setpoints[lastSetpoint + 1][0] != setpoints[lastSetpoint][0]){
      float slope = (setpoints[lastSetpoint + 1][1] - setpoints[lastSetpoint][1]) / (setpoints[lastSetpoint + 1][0] - setpoints[lastSetpoint][0]);
      float temp = ((currentTime - setpoints[lastSetpoint][0]) * slope + setpoints[lastSetpoint][1]);
      float scaledTemp = temp * (TEMP_AXIS_LENGTH - 1) / maxTemp;
      float scaledTempDecimals = scaledTemp - (int)scaledTemp;
      Serial.print(scaledTemp);
      Serial.print(", ");
      Serial.print((int)scaledTemp);
      Serial.print(", ");
      Serial.print(scaledTempDecimals);
      Serial.print(", ");
      if(scaledTempDecimals < 0.33){
        strncpy((char*)positionToPointer(displayChars, ORIGIN_X + i, ORIGIN_Y - scaledTemp), ".\u001A\u001A", 3);
        Serial.println("down");
      }else if(scaledTempDecimals >= 0.67){
        strncpy((char*)positionToPointer(displayChars, ORIGIN_X + i, ORIGIN_Y - scaledTemp), "'\u001A\u001A", 3);
        Serial.println("up");
      }else{
        strncpy((char*)positionToPointer(displayChars, ORIGIN_X + i, ORIGIN_Y - scaledTemp), "·\u001A", 3);
        Serial.println("middle");
      }
    }
  }

}

void drawDiagram(char displayChars[DISPLAY_LENGTH]){
  int maxTime = 0;
  int maxTemp = 0;

  for (int i = 0; i < SETPOINTS_COUNT; i++) {
    if(setpoints[i][1] > maxTemp){
      maxTemp = setpoints[i][1];
    }
  }

  for (int i = SETPOINTS_COUNT - 1; i >= 0; i--) {
    if(setpoints[i][0] != 0){
      maxTime = setpoints[i][0];
      break;
    }
  }

  drawCoordinateSystem(displayChars, maxTime, maxTemp);
  drawGraph(displayChars, maxTime, maxTemp);
}

void fillTable(char displayChars[DISPLAY_LENGTH]){
  strncpy((char*)positionToPointer(displayChars, DISPLAY_WIDTH - 10, 5), "t/h\u001A\u001A\u001A\u001A\u001A\u001A", 3*3);
  strncpy((char*)positionToPointer(displayChars, DISPLAY_WIDTH - 5, 5), "T/°C\u001A\u001A\u001A\u001A\u001A\u001A\u001A", 4*3);

  for (size_t i = 0; i < SETPOINTS_COUNT; i++) {
    if(setpoints[i][0] == 0 && i != 0){
      break;
    }
    formatTime((char*)positionToPointer(displayChars, DISPLAY_WIDTH - 11, 7 + i), setpoints[i][0]);
    addIntToDisplay(displayChars, DISPLAY_WIDTH - 2, 7 + i, setpoints[i][1]);
  }
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
  delay(500);
}
