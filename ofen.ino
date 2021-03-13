#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <string.h>
#include "driver/adc.h"
#include "driver/timer.h"
#include "freertos/task.h"
#include <PID_v1.h>
#include <PID_AutoTune_v0.h>
#include "esp_task_wdt.h"
#include "soc/rtc_wdt.h"
#include "nvs_flash.h"
#include "nvs.h"

#define MAX_CLIENTS 5

#define DISPLAY_WIDTH 84
#define DISPLAY_HEIGHT 20
#define DISPLAY_LENGTH (DISPLAY_WIDTH * DISPLAY_HEIGHT * 3) + 1

#define GPIO_COIL_A 22
#define GPIO_COIL_B 23
#define ADC_SPEED_READING ADC1_GPIO39_CHANNEL
#define ADC_SPEED_REFERENCE ADC1_GPIO36_CHANNEL
#define ADC_TEMP_READING ADC1_GPIO34_CHANNEL
#define ADC_POWER_REFERNECE ADC1_GPIO35_CHANNEL
#define ADC_VOLTAGE_READING ADC1_GPIO33_CHANNEL
#define ADC_CURRENT_READING ADC1_GPIO32_CHANNEL
#define POINT_50 400
#define ADJUST_TEMPERATURE_THRESHHOLD 50

#define GPIO_LEFT   21
#define GPIO_MIDDLE 19
#define GPIO_RIGHT  18
#define GPIO_SWITCH 13
#define GPIO_HEATER 12

#define SETPOINTS_COUNT 12
#define SETPOINT_SAVE_INTERVAL 10000
#define TIME_AXIS_LENGTH (DISPLAY_WIDTH - 4 - 1 - 1 - 12)
#define TEMP_AXIS_LENGTH (DISPLAY_HEIGHT - 4 - 1)
#define ORIGIN_X 4
#define ORIGIN_Y DISPLAY_HEIGHT - 2

#define LONG_PRESS 400

#define VOLTAGE_MULTIPLIER 935
#define CURRENT_MULTIPLIER 50
#define ADC_CAL_M_ATTEN_0 0.00023437099
#define ADC_CAL_N_ATTEN_0 0.0780530051
#define ADC_CAL_M_ATTEN_11 0.00080334
#define ADC_CAL_N_ATTEN_11 0.13154864

#define TYPE_K_M 24458.3257
#define TYPE_K_N -3.4684894

#define OPV_GAIN 18.407
#define OPV_OFFSET_VOLTAGE 0.1
#define AMBIENT_TEMP 22.0

WiFiClient *clients[MAX_CLIENTS] = { NULL };

const char *ssid = "esp32";
const char *password = "duBrauchstKeinPasswort";

WiFiServer server(80);

volatile int rotary = 0;

int selected_field = 0;

unsigned long lastMiddlePress = millis();
int deletedSetpoints = 0;

unsigned long lastSetpointsChange = millis();
uint8_t setpointsSaved = 1;

int16_t setpoints[SETPOINTS_COUNT][2] = {
  {0,   0},
  {1,  100},
  {2,  50},
  {-1, -1},
  {-1, -1},
  {-1, -1},
  {-1, -1},
  {-1, -1},
  {-1, -1},
  {-1, -1},
  {-1, -1},
  {-1, -1}
};

char staticDisplay[DISPLAY_LENGTH] = {};

unsigned long runningSince = 0;

struct buttonStates{
  unsigned int left        : 1;
  unsigned int middle      : 1;
  unsigned int right       : 1;
  unsigned int onOffSwitch : 1;
} buttonStates = {
  .left   = 0,
  .middle = 0,
  .right  = 0,
  .onOffSwitch = 0
};

double power = 0;

double adcAvg = 0;

volatile uint8_t accessingFlash = 0;

void IRAM_ATTR coilAReset();
void IRAM_ATTR coilBReset();

void adjustSetpoint(int *multiplier){
  int speed = adc1_get_raw(ADC_SPEED_READING) - adc1_get_raw(ADC_SPEED_REFERENCE);
  int adjustment = 1;
  if(speed >= ADJUST_TEMPERATURE_THRESHHOLD){
    adjustment = *multiplier * (pow(((double)speed - ADJUST_TEMPERATURE_THRESHHOLD) / (POINT_50 - ADJUST_TEMPERATURE_THRESHHOLD), 2) * 49 + 1);
  }

  setpoints[(int)(selected_field / 2)][selected_field % 2] += adjustment;
  if(selected_field % 2 == 0){
    if (selected_field > 0 && setpoints[(int)(selected_field / 2)][selected_field % 2] < setpoints[(int)(selected_field / 2) - 1][selected_field % 2]) {
      setpoints[(int)(selected_field / 2)][selected_field % 2] = setpoints[(int)(selected_field / 2) - 1][selected_field % 2];
    } else if (selected_field < SETPOINTS_COUNT * 2 - 2 && setpoints[(int)(selected_field / 2) + 1][selected_field % 2] != -1 && setpoints[(int)(selected_field / 2)][selected_field % 2] > setpoints[(int)(selected_field / 2) + 1][selected_field % 2]) {
      setpoints[(int)(selected_field / 2)][selected_field % 2] = setpoints[(int)(selected_field / 2) + 1][selected_field % 2];
    } else if (setpoints[(int)(selected_field / 2)][selected_field % 2] > 99 * 60 + 59) {
      setpoints[(int)(selected_field / 2)][selected_field % 2] = 99 * 60 + 59;
    }
  }else{
    if (selected_field % 2 != 0 && setpoints[(int)(selected_field / 2)][selected_field % 2] > 1300) {
      setpoints[(int)(selected_field / 2)][selected_field % 2] = 1300;
    }
  }

  if(setpoints[(int)(selected_field / 2)][selected_field % 2] < 0){
    setpoints[(int)(selected_field / 2)][selected_field % 2] = 0;
  }

  lastSetpointsChange = millis();
  setpointsSaved = 0;

  vTaskDelete(NULL);
}

void IRAM_ATTR coilA(){
  if(rotary == 0){
    rotary += 1;
  }else{
    int multiplier = 1;
    xTaskCreate((TaskFunction_t)adjustSetpoint, "adjustSetpoint", 10000, &multiplier, 5, NULL);
  }
  attachInterrupt(GPIO_COIL_A, coilAReset, FALLING);
}

void IRAM_ATTR coilB(){
  if(rotary == 0){
    rotary += 1;
  }else{
    int multiplier = -1;
    xTaskCreate((TaskFunction_t)adjustSetpoint, "adjustSetpoint", 10000, &multiplier, 5, NULL);
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
  while (setpoints[(int)(selected_field / 2)][selected_field % 2] == -1) {
    selected_field--;
  }
}

double getElapsedMinutes(){
  if(buttonStates.onOffSwitch == 1){
    return (millis() - runningSince) / 1000.0 / 60.0;
  }else{
    return 0;
  }
}

int getCurrentSetpoint(double minutesRunning){
  if(buttonStates.onOffSwitch == 1){
    int lastSetpoint = 0;
    while(lastSetpoint <= SETPOINTS_COUNT - 1 && setpoints[lastSetpoint + 1][0] != -1 && setpoints[lastSetpoint + 1][0] < minutesRunning) {
      lastSetpoint++;
    }
    if(setpoints[lastSetpoint + 1][0] == -1){
      return setpoints[lastSetpoint][1];
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
  double digit = ((num / 10.0) - rest) * 10.0;
  char numString[2] = {};
  sprintf(numString, "%f", digit);
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
  int seconds = minutes * 60.0 - (int)(minutes) * 60;
  int hours = ((int)minutes - (int)minutes % 60) / 60;
  minutes = (int)minutes % 60;

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
    float currentTime = i * (float)maxTime / (float)(TIME_AXIS_LENGTH - 1);
    while(setpoints[lastSetpoint + 1][0] < currentTime && setpoints[lastSetpoint + 1][0] != -1 && lastSetpoint < SETPOINTS_COUNT - 1) {
      lastSetpoint++;
    }
    if(setpoints[lastSetpoint + 1][0] != setpoints[lastSetpoint][0]){
      float slope = (float)(setpoints[lastSetpoint + 1][1] - setpoints[lastSetpoint][1]) / (float)(setpoints[lastSetpoint + 1][0] - setpoints[lastSetpoint][0]);
      if(setpoints[lastSetpoint + 1][0] == -1){
        slope = 0;
      }
      float temp = ((currentTime - setpoints[lastSetpoint][0]) * slope + setpoints[lastSetpoint][1]);
      float scaledTemp = temp * (TEMP_AXIS_LENGTH - 1) / (float)maxTemp;
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

  for (size_t i = 0; i < SETPOINTS_COUNT; i++){
    if(setpoints[i][0] == -1){
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

char* letterToPattern(char letter){
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
    case '-':
      pattern = " \0\0 \0\0 \0\0╺━╸ \0\0 \0\0 \0\0";
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
  fillTable(dynamicDisplay, invertDisplay);
  drawDiagram(dynamicDisplay);
  char timeString[10] = {};
  formatTime(timeString, getElapsedMinutes());
  appendLargeText(dynamicDisplay, timeString, 0, 0);
  char tempString[15] = {};
  int temp = voltageToTemp(adcToVoltage(adcAvg, 0));
  if(temp > 9999){
    sprintf(tempString, "----oC/%4doC", getCurrentSetpoint(getElapsedMinutes()));
  }else {
    sprintf(tempString, "%4doC/%4doC", temp, getCurrentSetpoint(getElapsedMinutes()));
  }
  appendLargeText(dynamicDisplay, tempString, 28, 0);
  char powerString[6] = {};
  sprintf(powerString, "%4dW", (int)power);
  appendLargeText(dynamicDisplay, powerString, 68, 0);

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
double Kp = 1;
double Ki = 1;
double Kd = 1;
PID pid(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

PID_ATune PIDAutotune(&input, &output);
uint8_t PIDAutotuning = 0;

void IRAM_ATTR pwmISR(void *param){
  TIMERG0.int_clr_timers.t0 = 1;
  //Serial.println(accessingFlash);
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

double adcToVoltage(int adcReading, float attenDB){
  if (attenDB == 0) {
    return ADC_CAL_M_ATTEN_0 * adcReading + ADC_CAL_N_ATTEN_0;
  } else if (attenDB == 11) {
    return ADC_CAL_M_ATTEN_11 * adcReading + ADC_CAL_N_ATTEN_11;
  }
  return 0;
}

int voltageToTemp(double voltage){
  double temp = TYPE_K_M * (voltage - OPV_OFFSET_VOLTAGE) / OPV_GAIN + TYPE_K_N + AMBIENT_TEMP;
  return temp;
}

void turnOn(){
  runningSince = millis();
  pid.SetMode(AUTOMATIC);
  timer_start(TIMER_GROUP_0, TIMER_0);
}

void turnOff(){
  pid.SetMode(MANUAL);
  timer_pause(TIMER_GROUP_0, TIMER_0);
}

void left(){
  if(selected_field > 0){
    selected_field--;
  }
}

void middle_down(){
  lastMiddlePress = millis();
  deletedSetpoints = 0;
}

void middle_up(){
  if(millis() - lastMiddlePress < LONG_PRESS && setpoints[SETPOINTS_COUNT - 1][0] == -1){
    int position = (int)(selected_field / 2);
    insertSetpoint(position, setpoints[position][0], setpoints[position][1]);
  }
}

void middle_pressed(){
  if((millis() - lastMiddlePress) / LONG_PRESS > deletedSetpoints && setpoints[1][0] != -1){
    deletedSetpoints++;
    deleteSetpoint((int)(selected_field / 2));
  }
}

void right(){
  if (setpoints[(int)((selected_field + 1) / 2)][0] != -1) {
    selected_field++;
  }
}

void buttonHandler(){

  if (buttonStates.left == 1 && digitalRead(GPIO_LEFT) == HIGH) {
    buttonStates.left = 0;
  } else if (buttonStates.left == 0 && digitalRead(GPIO_LEFT) == LOW) {
    left();
    buttonStates.left = 1;
  }

  if (buttonStates.middle == 1 && digitalRead(GPIO_MIDDLE) == HIGH) {
    middle_up();
    buttonStates.middle = 0;
  } else if (buttonStates.middle == 0 && digitalRead(GPIO_MIDDLE) == LOW) {
    middle_down();
    buttonStates.middle = 1;
  }

  if(digitalRead(GPIO_MIDDLE) == LOW){
    middle_pressed();
  }

  if (buttonStates.right == 1 && digitalRead(GPIO_RIGHT) == HIGH) {
    buttonStates.right = 0;
  } else if (buttonStates.right == 0 && digitalRead(GPIO_RIGHT) == LOW) {
    right();
    buttonStates.right = 1;
  }

  if (buttonStates.onOffSwitch == 1 && digitalRead(GPIO_SWITCH) == LOW) {
    turnOff();
    buttonStates.onOffSwitch = 0;
  } else if (buttonStates.onOffSwitch == 0 && digitalRead(GPIO_SWITCH) == HIGH) {
    turnOn();
    buttonStates.onOffSwitch = 1;
  }

}

void readPIDTunings(){
  nvs_handle nvsHandle;
  nvs_open("nvs", NVS_READWRITE, &nvsHandle);

  uint64_t intKp;
  uint64_t intKi;
  uint64_t intKd;

  nvs_get_u64(nvsHandle, "Kp", &intKp);
  nvs_get_u64(nvsHandle, "Ki", &intKi);
  nvs_get_u64(nvsHandle, "Kd", &intKd);

  Kp = intKp / 1000000.0;
  Ki = intKi / 1000000.0;
  Kd = intKd / 1000000.0;

  nvs_commit(nvsHandle);
  nvs_close(nvsHandle);
}

void updatePIDTunings(){
  nvs_handle nvsHandle;
  nvs_open("nvs", NVS_READWRITE, &nvsHandle);

  uint64_t intKp;
  uint64_t intKi;
  uint64_t intKd;

  uint64_t timerValue = 17;
  timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &timerValue);

  while (timerValue >= 15) {
    timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &timerValue);
  }

  nvs_get_u64(nvsHandle, "Kp", &intKp);
  nvs_get_u64(nvsHandle, "Ki", &intKi);
  nvs_get_u64(nvsHandle, "Kd", &intKd);

  if (intKp / 1000000.0 != Kp) {
    nvs_set_u64(nvsHandle, "Kp", (int)(Kp * 1000000));
  }
  if (intKi / 1000000.0 != Ki) {
    nvs_set_u64(nvsHandle, "Ki", (int)(Ki * 1000000));
  }
  if (intKd / 1000000.0 != Kd) {
    nvs_set_u64(nvsHandle, "Kd", (int)(Kd * 1000000));
  }

  nvs_commit(nvsHandle);
  nvs_close(nvsHandle);
}

void printPIDTunings(){
  Serial.print("Kp = ");
  Serial.println(Kp);
  Serial.print("Ki = ");
  Serial.println(Ki);
  Serial.print("Kd = ");
  Serial.println(Kd);
  Serial.println("");
}

void commandHandler(char command[]){
  if (strcmp(command, "help") == 0) {
    Serial.println("Available commands:");
    Serial.println("help              - displays this help dialog");
    Serial.println("echoPIDTunings    - print the Kp, Ki and Kd values");
    Serial.println("PIDAutotune       - conducts a PID autotune. Oven should be heated to a stable temperature before doing that. To store the returned Kp, Ki and Kd values permanently, use 'savePIDTunings'");
    Serial.println("cancelPIDAutotune - aborts the PID autotune");
    Serial.println("savePIDTunings    - stores the Kp, Ki and Kp from the latest autotune in non-volatile storage");
    Serial.println("");
  } else if (strcmp(command, "echoPIDTunings") == 0) {
    printPIDTunings();
  } else if (strcmp(command, "PIDAutotune") == 0) {
    Serial.println("Starting PID autotune");
    pid.SetMode(MANUAL);
    PIDAutotune.SetOutputStep(20);
    PIDAutotune.SetControlType(1);
    PIDAutotune.SetNoiseBand(5);
    PIDAutotune.SetLookbackSec(20);
    PIDAutotuning = 1;
    Serial.println("");
  } else if (strcmp(command, "cancelPIDAutotune") == 0) {
    Serial.println("");
    Serial.println("Canceling PID autotune");
    pid.SetMode(AUTOMATIC);
    PIDAutotuning = 0;
    PIDAutotune.Cancel();
    Serial.println("");
  } else if (strcmp(command, "savePIDTunings") == 0) {
    updatePIDTunings();
    Serial.println("saved PID tunings!");
    Serial.println("");
  } else if (strcmp(command, "readTemp") == 0){
    float adcAvg = 0;
    for(int i = 0; i < 100; i++){
      adcAvg += adc1_get_raw(ADC_TEMP_READING) / 100.0;
      delay(20);
    }
    Serial.print("ADC raw: ");
    Serial.print(adcAvg, 3);
    Serial.print(", voltage: ");
    Serial.print(adcToVoltage(adcAvg, 0), 8);
    Serial.print(", temp: ");
    Serial.println(voltageToTemp(adcToVoltage(adcAvg, 0)));
  }
}

void readSetpointSave(){
  nvs_handle nvsHandle;
  nvs_open("nvs", NVS_READWRITE, &nvsHandle);

  for (size_t i = 0; i < SETPOINTS_COUNT * 2; i++) {
    char key[13] = {};
    sprintf(key, "setpoint_%d", i);

    if (nvs_get_i16(nvsHandle, key, &setpoints[(int)(i / 2)][i % 2]) != ESP_OK) {
      break;
    }
  }

  nvs_commit(nvsHandle);
  nvs_close(nvsHandle);
}

void updateSetpointsSave(){
  if(setpointsSaved == 0){
    nvs_handle nvsHandle;
    nvs_open("nvs", NVS_READWRITE, &nvsHandle);

    for (size_t i = 0; i < SETPOINTS_COUNT * 2; i++) {
      if(setpoints[(int)(i / 2)][i % 2] == -1){
        break;
      }

      char key[13] = {};
      sprintf(key, "setpoint_%d", i);

      int16_t currentValue = -1;

      uint64_t timerValue = 17;
      timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &timerValue);
      while (timerValue >= 15) {
        timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &timerValue);
      }

      nvs_get_i16(nvsHandle, key, &currentValue);

      if (currentValue != setpoints[(int)(i / 2)][i % 2]) {
        nvs_set_i16(nvsHandle, key, setpoints[(int)(i / 2)][i % 2]);
      }

    }
    setpointsSaved = 1;
    nvs_commit(nvsHandle);
    nvs_close(nvsHandle);
  }
}

void core1(){
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

  pinMode(GPIO_SWITCH, INPUT_PULLUP);

  adc1_config_channel_atten(ADC_TEMP_READING, ADC_ATTEN_DB_0);
  adc1_config_channel_atten(ADC_VOLTAGE_READING, ADC_ATTEN_DB_0);
  adc1_config_channel_atten(ADC_CURRENT_READING, ADC_ATTEN_DB_0);
  adc1_config_channel_atten(ADC_POWER_REFERNECE, ADC_ATTEN_DB_0);

  readSetpointSave();

  pid.SetSampleTime(1000);
  pid.SetOutputLimits(0, 40);

  uint16_t voltages[100] = {};
  uint16_t currents[100] = {};

  while(true){
    double aggregatedPower = 0;

    uint32_t adcCollection = 0;

    for (size_t i = 0; i < 1000; i++) {
      buttonHandler();

      double voltage = adcToVoltage(adc1_get_raw(ADC_VOLTAGE_READING), 0);
      double current = adcToVoltage(adc1_get_raw(ADC_CURRENT_READING), 0);
      double reference = adcToVoltage(adc1_get_raw(ADC_POWER_REFERNECE), 0);

      aggregatedPower += (voltage - reference) * (current - reference);

      adcCollection += adc1_get_raw(ADC_TEMP_READING);
    }

    adcAvg = adcCollection / 1000.0;

    Serial.println((adcToVoltage(adcAvg, 0) - OPV_OFFSET_VOLTAGE) / OPV_GAIN, 8);

    power = aggregatedPower / 1000 * CURRENT_MULTIPLIER * VOLTAGE_MULTIPLIER;

    if(buttonStates.onOffSwitch == 1 && pid.Compute()){
      input = voltageToTemp(adcToVoltage(adcAvg, 0));
      setpoint = getCurrentSetpoint(getElapsedMinutes());
    }

    if (millis() - lastSetpointsChange > SETPOINT_SAVE_INTERVAL) {
      updateSetpointsSave();
    }

    int availableBytes = Serial.available();
    if (availableBytes > 0) {
      char serialIn[availableBytes + 1] = {};
      Serial.readBytes(serialIn, availableBytes);
      commandHandler(serialIn);
    }

    if(PIDAutotuning && Serial.print(".") && PIDAutotune.Runtime()){
      Serial.println("");
      PIDAutotuning = 0;
      Kp = PIDAutotune.GetKp();
      Ki = PIDAutotune.GetKi();
      Kd = PIDAutotune.GetKd();
      printPIDTunings();
      pid.SetMode(AUTOMATIC);
    }
  }
}

void clientManager(){
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
}

void core0() {
  generateStaticDisplay(staticDisplay);

  while (true) {
    clientManager();

    char dynamicDisplay[DISPLAY_LENGTH] = {};
    unsigned char invertDisplay[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8] = {};
    generateDynamicDisplay(dynamicDisplay, invertDisplay);
    processDisplay(staticDisplay, dynamicDisplay, invertDisplay);
    delay(50);
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

  adc1_config_width(ADC_WIDTH_BIT_12);
  adc1_config_channel_atten(ADC_SPEED_READING, ADC_ATTEN_DB_11);
  adc1_config_channel_atten(ADC_SPEED_REFERENCE, ADC_ATTEN_DB_11);

  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
  Serial.println("Server started");

  if(nvs_flash_init() == ESP_OK){
    Serial.println("NVS initialized successfully");
  }else{
    Serial.println("problem during NVS initialization");
  }

  xTaskCreatePinnedToCore((TaskFunction_t)core0, "core0Task", 50000, NULL, 10, NULL, 0);
  xTaskCreatePinnedToCore((TaskFunction_t)core1, "core1Task", 20000, NULL, 0, NULL, 1);
}

void loop(){
  vTaskDelete(NULL);
}
