#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include "Joystick.h"
#include "SD_MMC.h"
#include "Joystick_HTML.h"
#include <AccelStepper.h>
#include <MultiStepper.h>
#include "Configuraciones.h"

//Banderas
bool flag_EX = false;
bool flag_EY = false;
bool flagJoyTimer = false;
bool flagMoveTimer = false;
bool flagBoton = false;

//Variables para homing
bool HOMING = false;
unsigned int homingState = 0;

//Variables para grabación
bool SD_RECORDING = false;
bool SD_PLAYING = false;

//Webserver para utilizacion de Joystick HTML
AsyncWebServer server(80);

//Declaraciones de punteros para utilizar módulo timer
hw_timer_t *timer = NULL;
hw_timer_t *timerJoy = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE timerJoyMux = portMUX_INITIALIZER_UNLOCKED;

const char *ssid = "Felicitas";
const char *password = "felicitas2097.,.,";

const char *PARAM_INPUT_1 = "joy1PosX";
const char *PARAM_INPUT_2 = "joy1PosY";

Joystick JoyXY(POT_X, POT_Y, JOY_CENTER_X, JOY_CENTER_Y);
AccelStepper motor1(AccelStepper::DRIVER, STEP_M1, DIRPIN_M1);
AccelStepper motor2(AccelStepper::DRIVER, STEP_M2, DIRPIN_M2);
MultiStepper motores;
long positions[2]; //Arreglo que contiene las posiciones (en pasos) de los motores

//Configuraciones para utilizar periférico PWM - ignorar si se utiliza AccelStepper
const float MAX_FREQ_M1 = MAXRPM_M1 * PPR_M1 / (60.0);
const float MAX_FREQ_M2 = MAXRPM_M2 * PPR_M2 / (60.0);
float FREQ_M1 = 0, FREQ_M2 = 0;
float DUTY_M1 = 0, DUTY_M2 = 0;

//Posicion inicial del carro (Eje Y positivo hacia abajo)
float POS_X = 250.0, POS_Y = 10.0;
float posX_prev = 250.0, posY_prev = 10.0;

//Variables para longitud de cuerda de los motores
float L1 = 250.0, L2 = 250.0;
float L1_PREV = 250.0, L2_PREV = 250.0;
int L1_STEPS = 0, L2_STEPS = 0;

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}
/*
void display_offset()
{
  Serial.print("X: ");
  Serial.print(2048 - Xread);
  Serial.print(" - Y: ");
  Serial.println(2048 - Yread);
}
*/

//Recordad NO utilizar variables de tipo flotante
//dentro de rutinas de servicio de interrupcion!! (actualmente no soportado por ESP32)
void IRAM_ATTR endstop_x()
{
  bool statex = digitalRead(ENDSTOP_X);
  if (statex == HIGH && flag_EX == false)
  {
    flag_EX = true;
  }
  if (statex == LOW)
  {
    flag_EX = false;
  }
}
void IRAM_ATTR endstop_y()
{
  bool statey = digitalRead(ENDSTOP_Y);
  if (statey == HIGH && flag_EY == false)
  {
    flag_EY = true;
  }
  if (statey == LOW)
  {
    flag_EY = false;
  }
}
void IRAM_ATTR botonJoy()
{
  portENTER_CRITICAL_ISR(&timerMux);
  if (flagBoton == false)
  {
    flagBoton = true;
    //JoyXY.boton.counter = millis();
  }
  Serial.println("pressed");
  portEXIT_CRITICAL_ISR(&timerMux);
}
void IRAM_ATTR feedback_m1()
{
  portENTER_CRITICAL_ISR(&timerMux);
  int DIR_M1 = 0;
  if (digitalRead(DIRPIN_M1) == HIGH)
  {
    DIR_M1 = 1;
  }
  else
  {
    DIR_M1 = -1;
  }
  L1_STEPS += DIR_M1;
  portEXIT_CRITICAL_ISR(&timerMux);
}
void IRAM_ATTR feedback_m2()
{
  portENTER_CRITICAL_ISR(&timerMux);
  int DIR_M2 = 0;
  if (digitalRead(DIRPIN_M2) == HIGH)
  {
    DIR_M2 = 1;
  }
  else
  {
    DIR_M2 = -1;
  }
  L2_STEPS += DIR_M2;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void IRAM_ATTR onTimer()
{
  portENTER_CRITICAL_ISR(&timerMux);
  flagMoveTimer = true;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void IRAM_ATTR onTimerJoy()
{
  portENTER_CRITICAL_ISR(&timerMux);
  flagJoyTimer = true;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void moveM1()
{
  float mappedX = JoyXY.getMapped_X();
  positions[0] += mappedX / RES_M1;
}

void moveM2()
{
  float mappedX = JoyXY.getMapped_X();
  positions[1] += mappedX / RES_M2;
}

void configInitialMotorsPositions()
{
  //Cálculo y configuración longitud de cuerda inicial
  L1 = sqrt(pow(POS_X, 2) + pow(POS_Y, 2));
  L2 = sqrt(pow(POSX_M2 - POS_X, 2) + pow(POS_Y, 2));

  //Conversión de mm a pasos
  positions[0] = L1 / RES_M1;
  positions[1] = L2 / RES_M2;

  //Cargar posicion actual de los motores (aún cuando no sea verdadera, para evitar movimiento de los motores en un reinicio)
  motor1.setCurrentPosition(positions[0]);
  motor2.setCurrentPosition(positions[1]);
}
int taskcnt = 0;

void moveXY()
{
  //Guardar longitudes de cuerda previos
  L1_PREV = L1;
  L2_PREV = L2;

  //longitud de cuerda en milímetros
  L1 = sqrt(pow(POS_X, 2) + pow(POS_Y, 2));
  L2 = sqrt(pow(POSX_M2 - POS_X, 2) + pow(POS_Y, 2));

  //Conversion a pasos (posicion de motor)
  float STEPS_L1 = L1 / RES_M1;
  float STEPS_L2 = L2 / RES_M2;

  positions[0] = STEPS_L1;
  positions[1] = STEPS_L2;

  if (COMMANDS_DISPLAY)
  {
    taskcnt++;
    if (taskcnt > 50)
    {
      taskcnt = 0;
      Serial.println("L1 " + String(L1) + " - L2 " + String(L2) + " P1 " + String(positions[0]) + " - P2 " + String(positions[1]) + " SL1 " + String(STEPS_L1) + " SL2 " + String(STEPS_L2));
    }
  }
}

void handleShortPress()
{
  Serial.print("pressed s");
  if (HOMING == false)
  {
    //Toggle servo
    if (digitalRead(LED_SERVO) == LOW)
    {
      digitalWrite(LED_SERVO, HIGH);
    }
    else
    {
      digitalWrite(LED_SERVO, LOW);
    }
    Serial.println(" a");
  }
  else if (HOMING == true)
  {
    homingState++;
    Serial.println(" b " + String(homingState));
  }
}
void handleMediumPress()
{
  Serial.println("pressed m");
}
void handleLongPress()
{

  if (HOMING == false && (SD_RECORDING == false && SD_PLAYING == false))
  {
    HOMING = true;
    digitalWrite(LED_HOMING, HIGH);
    homingState = 1;
    Serial.println("aaa");
  }
  else if (HOMING == true)
  {
    HOMING = false;
    digitalWrite(LED_HOMING, LOW);
    homingState = 0;
    POS_X = HOMING_X;
    POS_Y = HOMING_Y;
    configInitialMotorsPositions();
    Serial.println("bbb");
  }
}
void buttonHandler()
{

  JoyXY.boton.prevState = JoyXY.boton.currentState;
  JoyXY.boton.currentState = digitalRead(BOTON_JOY);

  if (JoyXY.boton.currentState != JoyXY.boton.prevState)
  {
    delay(JoyXY.boton.debounce);
    // update status in case of bounce
    JoyXY.boton.currentState = digitalRead(BOTON_JOY);
    if (JoyXY.boton.currentState == BTN_PRESSED)
    {
      JoyXY.boton.counter = millis();
    }

    if (JoyXY.boton.currentState == BTN_NOT_PRESSED)
    {
      // but no longer pressed, how long was it down?
      unsigned long currentMillis = millis();

      if ((currentMillis - JoyXY.boton.counter >= SHORT_PRESS) && !(currentMillis - JoyXY.boton.counter >= MEDIUM_PRESS))
      {
        //SHORT PRESS
        handleShortPress();
      }
      if ((currentMillis - JoyXY.boton.counter >= MEDIUM_PRESS) && !(currentMillis - JoyXY.boton.counter >= LONG_PRESS))
      {
        //MEDIUM PRESS
        handleMediumPress();
      }
      if ((currentMillis - JoyXY.boton.counter >= LONG_PRESS))
      {
        //LONG PRESS
        handleLongPress();
      }
      JoyXY.boton.done = true;
    }
  }
}

void joystickTask(void *parameter)
{
  //Configuración de Timer 1 para ejecutar ciclos periódicos en tarea de joystick
  timerJoy = timerBegin(1, 80, true); //Timer 1, prescaler 80 -> 1MHz
  timerAttachInterrupt(timerJoy, &onTimerJoy, true);
  timerAlarmWrite(timerJoy, 50000, true); //Desborde cada 50000 cuentas, interrupción cada 50 ms
  timerAlarmEnable(timerJoy);             //Funcion 'timerJoy' asociada a interrupción de timer1

  int cyclecnt = 0;
  while (1)
  {
    if (flagJoyTimer)
    {
      digitalWrite(22, HIGH); //Utilizado para medir tiempo de ciclo con osciloscopio

      flagJoyTimer = false;

      float mappedX = JoyXY.getMapped_X();
      if (mappedX > DEADBAND_X || mappedX < -DEADBAND_X)
      {
        posX_prev = POS_X;
        POS_X += mappedX*JOY_GAIN;
      }
      float mappedY = JoyXY.getMapped_Y();
      if (mappedY > DEADBAND_Y || mappedY < -DEADBAND_Y)
      {
        posY_prev = POS_Y;
        POS_Y += mappedY*JOY_GAIN;
      }

      if (COMMANDS_DISPLAY)
      {
        cyclecnt++;
        if (cyclecnt == 10)
        {
          cyclecnt = 0;
          Serial.println("PX: " + String(POS_X) + " PY: " + String(POS_Y));
        }
      }
      if (flagBoton)
      {
        buttonHandler();
        if (JoyXY.boton.done)
        {
          JoyXY.boton.done = false;
          flagBoton = false;
        }
      }
      digitalWrite(22, LOW); //Utilizado para medir tiempo de ciclo con osciloscopio
    }
    vTaskDelay(10);
  }
}

void setup()
{

  pinMode(22, OUTPUT); //Para depuración - Borrar luego

  pinMode(POT_X, INPUT);
  pinMode(POT_Y, INPUT);
  pinMode(FEEDBACK_M1, INPUT);
  pinMode(FEEDBACK_M2, INPUT);
  pinMode(ENDSTOP_X, INPUT);
  pinMode(ENDSTOP_Y, INPUT);
  pinMode(BOTON_JOY, INPUT);
  pinMode(DIRPIN_M1, OUTPUT);
  pinMode(DIRPIN_M2, OUTPUT);
  pinMode(LED_HOMING, OUTPUT);
  pinMode(LED_SERVO, OUTPUT);

  digitalWrite(LED_HOMING, LOW);
  digitalWrite(LED_SERVO, LOW);

  //Cnfiguración velocidad máxima de los motores
  motor1.setMaxSpeed(MAXRPM_M1);
  motor2.setMaxSpeed(MAXRPM_M2);

  configInitialMotorsPositions();

  //Configuración velocidad nominal de motores
  motor1.setSpeed(250);
  motor2.setSpeed(250);

  //Añadir motores a multistepper
  motores.addStepper(motor1);
  motores.addStepper(motor2);

  //Configuracion de pines de fin de carrera como interrupción
  attachInterrupt(digitalPinToInterrupt(ENDSTOP_X), endstop_x, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENDSTOP_Y), endstop_y, FALLING);

  //Configuracion de pines de cuenta de pasos de motor como interrupción
  attachInterrupt(digitalPinToInterrupt(FEEDBACK_M1), feedback_m1, FALLING);
  attachInterrupt(digitalPinToInterrupt(FEEDBACK_M2), feedback_m2, FALLING);

  attachInterrupt(digitalPinToInterrupt(BOTON_JOY), botonJoy, FALLING);

  //Configuracion de interrupciones de Timer 0
  timer = timerBegin(0, 80, true); //Timer 0, prescaler = 80 -> 1MHz
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 20000, true); //Desborde cada 20000 cuentas -> interrupción cada 20ms
  timerAlarmEnable(timer);

  /*
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
*/

  /*if (!SD_MMC.begin())
  {
    Serial.println("Failed to mount card");
    return;
  }*/

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1))
    {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
    }
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_2))
    {
      inputMessage = request->getParam(PARAM_INPUT_2)->value();
      inputParam = PARAM_INPUT_2;
    }

    else
    {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" + inputParam + ") with value: " + inputMessage + "<br><a href=\"/\">Return to Home Page</a>");
  });
  server.onNotFound(notFound);
  server.begin();

  //Tarea para lectura de valores de joystick
  xTaskCreate(
      joystickTask,    //Función asociada a la area
      "Joystick Task", //Nombre de la tarea
      20000,           // Stack size (bytes)
      NULL,            //Parametro de entrada de la tarea
      1,               //Prioridad de la tarea
      NULL);           //Handle de la tarea
}

void loop()
{
  //En loop solo se ejecuta el calculo de las posiciones y el movimiento de los motores dado que
  //la funcion 'runSpeedToPosition()' bloquea el hilo hasta cumplir con la posicion requerida
  if (flagMoveTimer)
  {
    flagMoveTimer = false;
    if (HOMING == true)
    {
      switch (homingState)
      {
      case 1:
        moveM1();
        break;
      case 2:
        moveM2();
        break;
      case 3:
        moveM1();
        break;
      };
    }
    else if (SD_RECORDING || SD_PLAYING)
    {
    }
    else
    {
      //Modo manual
      moveXY();
    }

    motores.moveTo(positions);
    motores.runSpeedToPosition();
  }
  delay(1);
}
