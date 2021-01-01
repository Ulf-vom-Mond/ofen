#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <string.h>
#include "driver/adc.h"
#include "driver/timer.h"
#include "freertos/task.h"
#include <PID_v1.h>

#define MAX_CLIENTS 5

#define DISPLAY_WIDTH 84
#define DISPLAY_HEIGHT 20
#define DISPLAY_LENGTH (DISPLAY_WIDTH * DISPLAY_HEIGHT * 3) + 1

#define GPIO_COIL_A 22
#define GPIO_COIL_B 23
#define ADC_SPEED_READING ADC1_GPIO34_CHANNEL
#define ADC_SPEED_REFERENCE ADC1_GPIO35_CHANNEL
#define ADC_TEMP_READING ADC1_GPIO33_CHANNEL
#define POINT_50 400
#define ADJUST_TEMPERATURE_THRESHHOLD 50

#define GPIO_LEFT   25
#define GPIO_MIDDLE 26
#define GPIO_RIGHT  27
#define GPIO_SWITCH 12
#define GPIO_HEATER 13

#define SETPOINTS_COUNT 12
#define TIME_AXIS_LENGTH (DISPLAY_WIDTH - 4 - 1 - 1 - 12)
#define TEMP_AXIS_LENGTH (DISPLAY_HEIGHT - 4 - 1)
#define ORIGIN_X 4
#define ORIGIN_Y DISPLAY_HEIGHT - 2

#define LONG_PRESS 300

WiFiClient *clients[MAX_CLIENTS] = { NULL };

const char *ssid = "esp32";
const char *password = "duBrauchstKeinPasswort";

WiFiServer server(80);

int rotary = 0;

int selected_field = 0;

unsigned long lastMiddlePress = millis();

int setpoints[SETPOINTS_COUNT][2] = {
  {0,   0},
  {20,  100},
  {60,  800},
  {90,  800},
  {100, 600},
  {180, 1000},
  {200, 0},
  {-1, -1},
  {-1, -1},
  {-1, -1},
  {-1, -1},
  {-1, -1}
};

char staticDisplay[DISPLAY_LENGTH] = {};

uint8_t running = 0;
unsigned long runningSince = 0;

void IRAM_ATTR coilAReset();
void IRAM_ATTR coilBReset();

void IRAM_ATTR middle_down();
void IRAM_ATTR middle_up();

int adjustRotaryValue(){
  int speed = adc1_get_raw(ADC_SPEED_READING) - adc1_get_raw(ADC_SPEED_REFERENCE);
  if(speed < ADJUST_TEMPERATURE_THRESHHOLD){
    return 1;
  }
  return pow(((double)speed - ADJUST_TEMPERATURE_THRESHHOLD) / (POINT_50 - ADJUST_TEMPERATURE_THRESHHOLD), 2) * 49 + 1;
}

void adjustSetpoint(int adjustment){
  setpoints[(int)(selected_field / 2)][selected_field % 2] += adjustment;
  if(selected_field % 2 == 0){
    if (selected_field > 0 && setpoints[(int)(selected_field / 2)][selected_field % 2] < setpoints[(int)(selected_field / 2) - 1][selected_field % 2]) {
      setpoints[(int)(selected_field / 2)][selected_field % 2] = setpoints[(int)(selected_field / 2) - 1][selected_field % 2];
      return;
    } else if (selected_field < SETPOINTS_COUNT * 2 - 2 && setpoints[(int)(selected_field / 2) + 1][selected_field % 2] != -1 && setpoints[(int)(selected_field / 2)][selected_field % 2] > setpoints[(int)(selected_field / 2) + 1][selected_field % 2]) {
      setpoints[(int)(selected_field / 2)][selected_field % 2] = setpoints[(int)(selected_field / 2) + 1][selected_field % 2];
      return;
    } else if (setpoints[(int)(selected_field / 2)][selected_field % 2] > 99 * 60 + 59) {
      setpoints[(int)(selected_field / 2)][selected_field % 2] = 99 * 60 + 59;
      return;
    }
  }
  if(setpoints[(int)(selected_field / 2)][selected_field % 2] < 0){
    setpoints[(int)(selected_field / 2)][selected_field % 2] = 0;
    return;
  }
  if (selected_field % 2 != 0 && setpoints[(int)(selected_field / 2)][selected_field % 2] > 1300) {
    setpoints[(int)(selected_field / 2)][selected_field % 2] = 1300;
  }
}

void IRAM_ATTR coilA(){
  if(rotary == 0){
    rotary += 1;
  }else{
    adjustSetpoint(adjustRotaryValue());
  }
  attachInterrupt(GPIO_COIL_A, coilAReset, FALLING);
}

void IRAM_ATTR coilB(){
  if(rotary == 0){
    rotary += 1;
  }else{
    adjustSetpoint(-adjustRotaryValue());
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

void IRAM_ATTR left(){
  if(selected_field > 0){
    selected_field--;
  }
}

void IRAM_ATTR middle_down(){
  lastMiddlePress = millis();
  attachInterrupt(GPIO_MIDDLE, middle_up, RISING);
}

void insertSetpoint(int position, int lastMiddlePress, int temp){
  if(setpoints[position][0] != -1){
    insertSetpoint(position + 1, setpoints[position][0], setpoints[position][1]);
  }
  setpoints[position][0] = lastMiddlePress;
  setpoints[position][1] = temp;
}

void deleteSetpoint(int position){
  if(position + 1 < SETPOINTS_COUNT){
    if(setpoints[position + 1][0] == -1){
      setpoints[position][0] = -1;
      setpoints[position][1] = -1;
    }else{
      setpoints[position][0] = setpoints[position + 1][0];
      setpoints[position][1] = setpoints[position + 1][1];
      deleteSetpoint(position + 1);
    }
  }
}

void IRAM_ATTR middle_up(){
  if(millis() - lastMiddlePress < LONG_PRESS && setpoints[SETPOINTS_COUNT - 1][0] == -1){
    int position = (int)(selected_field / 2);
    insertSetpoint(position, setpoints[position][0], setpoints[position][1]);
  }
  if(millis() - lastMiddlePress > LONG_PRESS && setpoints[2][0] != -1) {
    deleteSetpoint((int)(selected_field / 2));
  }
  attachInterrupt(GPIO_MIDDLE, middle_down, FALLING);
}

void IRAM_ATTR right(){
  if (setpoints[(int)((selected_field + 1) / 2)][0] != -1) {
    selected_field++;
  }
}

double getElapsedMinutes(){
  if(running == 1){
    return (millis() - runningSince) / 1000.0 / 60.0;
  }else{
    return 0;
  }
}

int getCurrentSetpoint(){
  if(running == 1){
    double minutesRunning = getElapsedMinutes();
    int lastSetpoint = 0;
    while(setpoints[lastSetpoint + 1][0] < minutesRunning && lastSetpoint < SETPOINTS_COUNT - 1) {
      lastSetpoint++;
    }
    if(setpoints[lastSetpoint + 1][0] != setpoints[lastSetpoint][0]){
      double slope = (setpoints[lastSetpoint + 1][1] - setpoints[lastSetpoint][1]) / (setpoints[lastSetpoint + 1][0] - setpoints[lastSetpoint][0]);
      return ((minutesRunning - setpoints[lastSetpoint][0]) * slope + setpoints[lastSetpoint][1]);
    }else{
      return setpoints[lastSetpoint + 1][1];
    }
  }else{
    return 0;
  }
}

int positionToInt(uint8_t x, uint8_t y){
  return y * DISPLAY_WIDTH * 3 + x * 3;
}

void * positionToPointer(char *displayChars, uint8_t x, uint8_t y){
  return displayChars + positionToInt(x, y);
}

uint8_t getBit(unsigned char invertDisplay[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8], int pos){
  return (uint8_t)(*(invertDisplay + (int)(pos / 8)) >> (pos % 8)) & 0x01;
}

uint8_t setBit(unsigned char invertDisplay[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8], int pos, uint8_t bit){
  *(invertDisplay + (int)(pos / 8)) = (*(invertDisplay + (int)(pos / 8)) & ~(0x01 << (pos % 8))) | bit << (pos % 8);
}

void appendLargeText(char displayChars[DISPLAY_LENGTH], char *text, uint8_t xPos, uint8_t yPos){

  char pattern[28];
  memcpy(pattern, letterToPattern(text[0]), 28);
  for(int i = 0; i < 3; i++){
    void *displayCharsPnt = positionToPointer(displayChars, xPos, yPos + i);

    int patternPos = i * 9;
    void *patternPnt = pattern + patternPos;

    memcpy(displayCharsPnt, patternPnt, 9);
  }
  int textLen = strlen(text) - 1;
  if(textLen >= 1 && xPos + 3 < DISPLAY_WIDTH){
    appendLargeText(displayChars, ++text, xPos + 3, yPos);
  }
}

void appendText(char displayChars[DISPLAY_LENGTH], char *text, uint8_t xPos, uint8_t yPos){
  strncpy((char*)positionToPointer(displayChars, xPos, yPos), text, 3);
  if(strlen(text + 3) > 0){
    appendText(displayChars, text + 3, xPos + 1, yPos);
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

          strncpy(str, "┼", 4);

          if(current_x_pos == lines[i].start_x && current_y_pos == lines[i].start_y){
            if(lines[i].direction){ // y
              strncpy(str, "┬", 4);
            }else{ // x
              strncpy(str, "├", 4);
            }
          }
          if(current_x_pos == lines[k].start_x && current_y_pos == lines[k].start_y){
            if(lines[k].direction){ // y
              strncpy(str, "┬", 4);
            }else{ // x
              strncpy(str, "├", 4);
            }
          }
          if(current_x_pos == end_x_i && current_y_pos == end_y_i){
            if(lines[i].direction){ // y
              strncpy(str, "┴", 4);
            }else{ // x
              strncpy(str, "┤", 4);
            }
          }
          if(current_x_pos == end_x_k && current_y_pos == end_y_k){
            if(lines[k].direction){ // y
              strncpy(str, "┴", 4);
            }else{ // x
              strncpy(str, "┤", 4);
            }
          }

          if(current_x_pos == lines[i].start_x && current_y_pos == lines[i].start_y && current_x_pos == lines[k].start_x && current_y_pos == lines[k].start_y){
            strncpy(str, "┌", 4);
          }
          if(current_x_pos == lines[i].start_x && current_y_pos == lines[i].start_y && current_x_pos == end_x_k && current_y_pos == end_y_k){
            if(lines[k].direction){ // y
              strncpy(str, "└", 4);
            }else{ // x
              strncpy(str, "┐", 4);
            }
          }
          if(current_x_pos == end_x_i && current_y_pos == end_y_i && current_x_pos == lines[k].start_x && current_y_pos == lines[k].start_y){
            if(lines[k].direction){ // y
              strncpy(str, "┐", 4);
            }else{ // x
              strncpy(str, "└", 4);
            }
          }
          if(current_x_pos == end_x_i && current_y_pos == end_y_i && current_x_pos == end_x_k && current_y_pos == end_y_k){
            strncpy(str, "┘", 4);
          }
        }
      }

      if(str[0] == '\0'){
        if(lines[i].direction){
          strncpy(str, "│", 4);
        }else{
          strncpy(str, "─", 4);
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

void printInt(char *str, int x, int y, int num){
  int rest = num / 10;
  int digit = ((num / 10.0) - rest) * 10;
  char numString[1] = {};
  sprintf(numString, "%d", (int)digit);
  memcpy(positionToPointer(str, x, y), numString, 1);
  if(rest != 0){
    printInt(str, x - 1, y, rest);
  }
}

void addTimeToDisplay(char *dest, int minutes){
  int hours = (minutes - minutes % 60) / 60;
  minutes = minutes % 60;

  size_t i;
  for (i = 0; i < 2 - getDigitCount(hours); i++) {
    strncpy(dest + i * 3, "0", 1);
  }
  printInt(dest, 1, 0, hours);
  strncpy(dest + 6, ":", 1);
  for (i = 0; i < 2 - getDigitCount(minutes); i++) {
    strncpy(dest + 9 + i * 3 , "0", 1);
  }
  printInt(dest, 4, 0, minutes);
}

void formatTime(char *dest, double minutes){
  Serial.print("input: ");
  Serial.print(minutes);
  int seconds = minutes * 60.0 - (int)(minutes) * 60;
  Serial.print(", seconds: ");
  Serial.print(seconds);
  int hours = ((int)minutes - (int)minutes % 60) / 60;
  Serial.print(", hours: ");
  Serial.print(hours);
  minutes = (int)minutes % 60;
  Serial.print(", minutes: ");
  Serial.println(minutes);

  sprintf(dest, "%02d:%02d:%02dh", hours, (int)minutes, seconds);
}

void drawCoordinateSystem(char displayChars[DISPLAY_LENGTH], int maxTime, int maxTemp){
  appendText(displayChars, "T\0\0/\0\0°\0C\0\0^\0\0", 0, 4);
  appendText(displayChars, ">\0\0", 4 + TIME_AXIS_LENGTH, DISPLAY_HEIGHT - 2);
  appendText(displayChars, "t\0\0/\0\0h\0\0", 4 + TIME_AXIS_LENGTH - 2, DISPLAY_HEIGHT - 1);
  appendText(displayChars, "┼", ORIGIN_X, ORIGIN_Y);
  appendText(displayChars, "0\0\0", ORIGIN_X - 1, ORIGIN_Y + 1);

  for (int i = 2; i < TEMP_AXIS_LENGTH - 1; i += 2) {
    int tempVal = maxTemp * i / (TEMP_AXIS_LENGTH - 1);
    printInt(displayChars, 3, 18 - i, tempVal);
    strncpy((char*)positionToPointer(displayChars, 4, 18 - i), "┼", 3);
  }

  for (int i = 10; i < TIME_AXIS_LENGTH - 1; i += 10) {
    int timeVal = maxTime * i / (TIME_AXIS_LENGTH - 1);
    addTimeToDisplay((char*)positionToPointer(displayChars, 2 + i, 19), timeVal);
    strncpy((char*)positionToPointer(displayChars, 4 + i, 18), "┼", 3);
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
      if(scaledTempDecimals < 0.33){
        strncpy((char*)positionToPointer(displayChars, ORIGIN_X + i, ORIGIN_Y - scaledTemp), ".", 3);
      }else if(scaledTempDecimals >= 0.67){
        strncpy((char*)positionToPointer(displayChars, ORIGIN_X + i, ORIGIN_Y - scaledTemp), "'", 3);
      }else{
        strncpy((char*)positionToPointer(displayChars, ORIGIN_X + i, ORIGIN_Y - scaledTemp), "·", 3);
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
    if(setpoints[i][0] > 0){
      maxTime = setpoints[i][0];
      break;
    }
  }

  drawCoordinateSystem(displayChars, maxTime, maxTemp);
  drawGraph(displayChars, maxTime, maxTemp);
}

void fillTable(char displayChars[DISPLAY_LENGTH], unsigned char invertDisplay[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8]){
  appendText(displayChars, "t\0\0/\0\0h\0\0", DISPLAY_WIDTH - 10, 5);
  appendText(displayChars, "T\0\0/\0\0°\0C\0\0", DISPLAY_WIDTH - 5, 5);

  for (size_t i = 0; i < SETPOINTS_COUNT; i++) {
    if(setpoints[i][0] == 0 && i != 0){
      break;
    }
    if(setpoints[i][0] >= 0){
      addTimeToDisplay((char*)positionToPointer(displayChars, DISPLAY_WIDTH - 11, 7 + i), setpoints[i][0]);
    }
    if(setpoints[i][1] >= 0){
      printInt(displayChars, DISPLAY_WIDTH - 2, 7 + i, setpoints[i][1]);
    }
    if((int)(selected_field / 2) == i){
      if (selected_field % 2 == 0) {
        for (size_t j = 0; j < 5; j++) {
          setBit(invertDisplay, j + DISPLAY_WIDTH - 11 + (7 + i) * DISPLAY_WIDTH, 1);
        }
      } else {
        for (size_t j = 0; j < 4; j++) {
          setBit(invertDisplay, j + DISPLAY_WIDTH - 5 + (7 + i) * DISPLAY_WIDTH, 1);
        }
      }
    }
  }
}

void sendDisplay(char *displayChars){
  for (int i=0 ; i<MAX_CLIENTS ; i++) {
    if (clients[i] == NULL) {
      break;
    }
    if(!(clients[i] -> connected())){
      clients[i] = NULL;
    }else{
      clients[i] -> print(displayChars);
    }
  }
}

void initializeClient(WiFiClient client){
  client.print("\u001b[0m\u001b[?25l\u001b[3J\u001b[1;1H"); //reset all graphic settings (SGR parameters), hide cursor, erase display, go to top left corner of display
}

char* letterToPattern(char letter){ //x, y and 3 chars for unicode
  // '━': \u2501, '┃': \u2503, '╱': \u2571, '╲': \u2572,
  // '╸': \u2578, '╹': \u2579, '╺': \u257A, '╻': \u257B
  // '┏': \u250F, '┓': \u2513, '┗': \u2517, '┛': \u251B,
  // '┣': \u2523, '┫': \u252B, '┳': \u2533, '┻': \u253B,
  char *pattern;
  switch (letter) {
    case '0':
      pattern = "┏━┓┃ \0\0┃┗━┛";
      break;
    case '1':
      pattern = " \0\0 \0\0╻ \0\0 \0\0┃ \0\0 \0\0╹";
      break;
    case '2':
      pattern = "╺━┓┏━┛┗━╸";
      break;
    case '3':
      pattern = "╺━┓ \0\0━┫╺━┛";
      break;
    case '4':
      pattern = "╻ \0\0╻┗━┫ \0\0 \0\0╹";
      break;
    case '5':
      pattern = "┏━╸┗━┓╺━┛";
      break;
    case '6':
      pattern = "┏━╸┣━┓┗━┛";
      break;
    case '7':
      pattern = "╺━┓ \0\0 \0\0┃ \0\0 \0\0╹";
      break;
    case '8':
      pattern = "┏━┓┣━┫┗━┛";
      break;
    case '9':
      pattern = "┏━┓┗━┫╺━┛";
      break;
    case '/':
      pattern = " \0\0 \0\0╱ \0\0╱ \0\0╱ \0\0 \0\0";
      break;
    case 'o':
      pattern = " \0\0 \0\0o\0\0 \0\0 \0\0 \0\0 \0\0 \0\0 \0\0";
      break;
    case 'C':
      pattern = "┏━╸┃ \0\0 \0\0┗━╸";
      break;
    case 'h':
      pattern = "╻ \0\0 \0\0┣━┓╹ \0\0╹";
      break;
    case ':':
      pattern = " \0\0╻ \0\0 \0\0 \0\0 \0\0 \0\0╹ \0\0";
      break;
    case 'W':
      pattern = "╻ \0\0╻┃┃┃┗┻┛";
      break;
    default:
      pattern = " \0\0 \0\0 \0\0 \0\0 \0\0 \0\0 \0\0 \0\0 \0\0";
  }
  return pattern;
}

void generateStaticDisplay(char staticDisplay[DISPLAY_LENGTH]){
  drawSeparationLines(staticDisplay);
}

void generateDynamicDisplay(char dynamicDisplay[DISPLAY_LENGTH], unsigned char invertDisplay[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8]){
  char timeString[10] = {};
  formatTime(timeString, getElapsedMinutes());
  appendLargeText(dynamicDisplay, timeString, 0, 0);
  char tempString[14] = {};
  sprintf(tempString, "%4doC/%4doC", voltageToTemp(adc1_get_raw(ADC_TEMP_READING)), getCurrentSetpoint());
  appendLargeText(dynamicDisplay, tempString, 28, 0);
  appendLargeText(dynamicDisplay, "1496W", 68, 0);

  drawDiagram(dynamicDisplay);
  fillTable(dynamicDisplay, invertDisplay);
}

void processDisplay(char staticDisplay[DISPLAY_LENGTH], char dynamicDisplay[DISPLAY_LENGTH], unsigned char invertDisplay[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8]){
  char invert[] = "\u001b[7m";
  char reset[] = "\u001b[27m";

  int invertedRegionCounter = 0;                                 // checking how large the
  uint8_t lastBit = 0;                                           // final displayChars array
  for (size_t i = 0; i < sizeof(invertDisplay) * 8; i++) {  // has to be to contain the
    uint8_t currentBit = getBit(invertDisplay, i);       // invert/reset video commands
    if(currentBit == 1 && lastBit == 0){
      invertedRegionCounter++;
    }
    lastBit = currentBit;
  }

  int displayCharsLength = DISPLAY_LENGTH + DISPLAY_HEIGHT - 1 + invertedRegionCounter * (sizeof(invert) + sizeof(reset));
  char *displayChars = (char*)malloc(displayCharsLength + 20);
  memset(displayChars, '\0', displayCharsLength);
  strcpy(displayChars, "\u001b[0m\u001b[?25l\u001b[3J\u001b[1;1H");
  int displayCharIterator = 20;                                   // merging staticDisplay and dynamicDisplay
  lastBit = 0;                                                   // and inserting the invert/reset video commands
  for (size_t i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
    if(i != 0 && i % DISPLAY_WIDTH == 0){
      strcpy(displayChars + displayCharIterator, "\n");
      displayCharIterator++;
    }

    uint8_t currentBit = getBit(invertDisplay, i);
    if(currentBit == 1 && lastBit == 0){
      strcpy(displayChars + displayCharIterator, invert);
      displayCharIterator += 4;
    }
    if(currentBit == 0 && lastBit == 1){
      strcpy(displayChars + displayCharIterator, reset);
      displayCharIterator += 5;
    }
    lastBit = currentBit;

    if (dynamicDisplay[3 * i] != '\0') {
      strncpy(displayChars + displayCharIterator, dynamicDisplay + i * 3, 3);
    } else if (staticDisplay[3 * i] != '\0') {
      strncpy(displayChars + displayCharIterator, staticDisplay + i * 3, 3);
    } else {
      strcpy(displayChars + displayCharIterator, " ");
    }
    while(*(displayChars + displayCharIterator) != '\0'){
      displayCharIterator++;
    }

  }
  sendDisplay(displayChars);
  free(displayChars);
}

uint8_t dutycycleCounter = 0;

double input = 0;
double output = 0;
double setpoint = 0;
PID pid(&input, &output, &setpoint, 2, 5, 1, DIRECT);

void IRAM_ATTR pwmISR(void *param){
  TIMERG0.int_clr_timers.t0 = 1;
  if(dutycycleCounter >= 100){
    digitalWrite(GPIO_HEATER, HIGH);
    dutycycleCounter -= 100;
  }else{
    digitalWrite(GPIO_HEATER, LOW);
  }
  dutycycleCounter += output;
  timer_set_alarm(TIMER_GROUP_0, TIMER_0, TIMER_ALARM_EN);
  TIMERG0.int_clr_timers.t0 = 0;
}

void IRAM_ATTR pidISR(void *param){
  TIMERG0.int_clr_timers.t1 = 1;

  if(running == 1){
    input = voltageToTemp(adcToVoltage(adc1_get_raw(ADC_TEMP_READING)));
    setpoint = getCurrentSetpoint();
  }

  pid.Compute();
  timer_set_alarm(TIMER_GROUP_0, TIMER_1, TIMER_ALARM_EN);
  TIMERG0.int_clr_timers.t1 = 0;
}

void IRAM_ATTR turnOff();

void IRAM_ATTR turnOn(){
  running = 1;
  runningSince = millis();
  pid.SetMode(AUTOMATIC);
  timer_start(TIMER_GROUP_0, TIMER_0);
  attachInterrupt(GPIO_SWITCH, turnOff, RISING);
}

void IRAM_ATTR turnOff(){
  running = 0;
  pid.SetMode(MANUAL);
  timer_pause(TIMER_GROUP_0, TIMER_0);
  attachInterrupt(GPIO_SWITCH, turnOn, FALLING);
}

double adcToVoltage(int adc){
  int adcMax = 4095;
  double UMax = 1.1;
  return UMax / adcMax * adc;
}

int voltageToTemp(double voltage){
  int TMin = 0;
  int TMax = 1300;
  double UMin = 0;
  double UMax = 52.410;
  return (TMax - TMin) / (UMax - UMin) * voltage;
}

void core2(){
  pinMode(GPIO_HEATER, OUTPUT);
  const timer_config_t pwmTimer = {
    .alarm_en = TIMER_ALARM_EN,
    .counter_en = TIMER_START,
    .intr_type = TIMER_INTR_LEVEL,
    .counter_dir = TIMER_COUNT_UP,
    .auto_reload = TIMER_AUTORELOAD_EN,
    .divider = 50000
  };
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 16);
  timer_isr_register(TIMER_GROUP_0, TIMER_0, pwmISR, (void*)0, ESP_INTR_FLAG_IRAM, NULL);
  timer_enable_intr(TIMER_GROUP_0, TIMER_0);
  timer_init(TIMER_GROUP_0, TIMER_0, &pwmTimer);

  const timer_config_t pidTimer = {
    .alarm_en = TIMER_ALARM_EN,
    .counter_en = TIMER_START,
    .intr_type = TIMER_INTR_LEVEL,
    .counter_dir = TIMER_COUNT_UP,
    .auto_reload = TIMER_AUTORELOAD_EN,
    .divider = 64000
  };
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_1, 1250);
  timer_isr_register(TIMER_GROUP_0, TIMER_1, pidISR, (void*)0, ESP_INTR_FLAG_IRAM, NULL);
  timer_enable_intr(TIMER_GROUP_0, TIMER_1);
  timer_init(TIMER_GROUP_0, TIMER_1, &pidTimer);

  pinMode(GPIO_SWITCH, INPUT_PULLUP);
  attachInterrupt(GPIO_SWITCH, turnOn, FALLING);

  adc1_config_channel_atten(ADC_TEMP_READING, ADC_ATTEN_DB_0);

  pid.SetSampleTime(1000);
  pid.SetOutputLimits(0, 100);

  while(true){
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(GPIO_COIL_A, INPUT);
  pinMode(GPIO_COIL_B, INPUT);
  attachInterrupt(GPIO_COIL_A, coilA, RISING);
  attachInterrupt(GPIO_COIL_B, coilB, RISING);

  pinMode(GPIO_LEFT,   INPUT_PULLUP);
  pinMode(GPIO_MIDDLE, INPUT_PULLUP);
  pinMode(GPIO_RIGHT,  INPUT_PULLUP);
  attachInterrupt(GPIO_LEFT,   left,   FALLING);
  attachInterrupt(GPIO_MIDDLE, middle_down, FALLING);
  attachInterrupt(GPIO_RIGHT,  right,  FALLING);

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

  xTaskCreatePinnedToCore((TaskFunction_t)core2, "core2Task", 20000, NULL, 0, NULL, 0);

  generateStaticDisplay(staticDisplay);
}

void loop(){
  unsigned long loopTime = micros();
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
  char dynamicDisplay[DISPLAY_LENGTH] = {};
  unsigned char invertDisplay[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8] = {};

  generateDynamicDisplay(dynamicDisplay, invertDisplay);
  processDisplay(staticDisplay, dynamicDisplay, invertDisplay);

  delay(50);
}
