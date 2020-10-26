#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "Joystick.h"
#include "Joystick_HTML.h"
#include "Configuraciones.h"
#include "FS.h"
#include <SD.h>
#include "SPI.h"
#include <EEPROM.h>

//Banderas
bool flagJoyTimer = false;
bool flagBoton = false;

//Variables para homing
bool HOMING = false;
bool CANVAS = false;
unsigned int homingState = 0;
unsigned int canvasState = 0;
float homingGain = 1.0;
float canvasGain = 1.0;
bool nextState = false;
bool hom_L1 = false;
bool hom_L2 = false;

//Webserver para utilizacion de Joystick HTML
AsyncWebServer server(80);
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

//Declaraciones de punteros para utilizar módulo timer
hw_timer_t *timer = NULL;
hw_timer_t *timerJoy = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE timerJoyMux = portMUX_INITIALIZER_UNLOCKED;

const char *PARAM_MESSAGE1 = "stateX";
const char *PARAM_MESSAGE2 = "stateY";
const char *PARAM_MESSAGE3 = "GPIO";
const char *PARAM_MESSAGE4 = "action";
const char *PARAM_MESSAGE5 = "output";
const char *PARAM_MESSAGE6 = "state";
const char *PARAM_MESSAGE7 = "HX";
const char *PARAM_MESSAGE8 = "HY";
const char *PARAM_MESSAGE9 = "PX";
const char *PARAM_MESSAGE10 = "PY";
const char *PARAM_MESSAGE11 = "CVW";
const char *PARAM_MESSAGE12 = "CVH";

Joystick JoyXY(POT_X, POT_Y, JOY_CENTER_X, JOY_CENTER_Y);
int joyLongPressCnt = 0;

//Configuraciones para utilizar periférico PWM - ignorar si se utiliza AccelStepper
const float MAX_FREQ_M1 = MAXRPM_M1 * PPR_M1 * MICROSTEPS / (60.0);
const float MAX_FREQ_M2 = MAXRPM_M2 * PPR_M2 * MICROSTEPS / (60.0);
float FREQ_M1 = 0, FREQ_M2 = 0;
float DUTY_M1 = 0, DUTY_M2 = 0;

//Posicion inicial del carro (Eje Y positivo hacia abajo)
float POS_X = INI_X, POS_Y = INI_Y;
float VEL_X = 0, VEL_Y = 0;
float VEL_L1 = 0, VEL_L2 = 0;
float VEL_L1_ANT = 0, VEL_L2_ANT = 0;
float VEL_L1_aux = 0, VEL_L2_aux = 0;

//Angulos de las cuerdas, paso actual y paso previo
float alpha_L2 = 0, gamma_L1 = 0;
float alphaAnt = 0, gammaAnt = 0;
//Banderas para rampa de aceleracion y frenado
bool rampUpL1 = false, rampDownL1 = false;
bool rampUpL2 = false, rampDownL2 = false;
bool flag_fm1 = false, flag_fm2 = false;
int state_fm1 = 0, state_fm2 = 0;

//Variables para joystick HTML
int cnt_clientes = 0;
bool joystickHTML = false;
float html_CX = 0, html_CY = 0;

//Variables para longitud de cuerda de los motores
float L1 = 250.0, L2 = 250.0;
signed long int M1_STEPS = L1 * PPR_M1 * MICROSTEPS / PI * D1;
signed long int M2_STEPS = L2 * PPR_M2 * MICROSTEPS / PI * D2;

//Variable de 'archivo' para lectura y escritura
File fileWrite;
File fileRead;

int cnt = 0;
int taskcnt = 0; //Utilizada en funcion moveXY como contador
int debounceCounter = 0;

//Variables para grabacion y reproducción de movimientos
bool SD_RECORDING = false;
bool SD_PLAYING = false;

bool SDOK = false;
bool fileOK = false;
bool filereadOK = false;

float SD_BUFFER[SD_BUFFER_SIZE][5];
float REPLAY_BUFFER[REPLAY_IT_LINES][5];
int SD_REC_CNT = 0;

int writeCycle = 0, readCycle = 0, readLines = 0, maxCycles = 0, replay_cnt = 0;

bool cycleStart = false;
bool readStart = false;

bool resumePlay = false;
int playState = 0;
bool replay_homing = false;
