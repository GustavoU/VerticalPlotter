#include <Arduino.h>
#include "Includes.h"

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

//Recordad NO utilizar variables de tipo flotante
//dentro de rutinas de servicio de interrupcion!! (actualmente no soportado por ESP32)
void IRAM_ATTR botonJoy()
{
  portENTER_CRITICAL_ISR(&timerMux);
  if (flagBoton == false)
  {
    flagBoton = true;
  }
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

  state_fm1 = digitalRead(FEEDBACK_M1);
  if (state_fm1 == HIGH && flag_fm1 == false)
  {
    flag_fm1 = true;
    M1_STEPS += DIR_M1;
  }
  if (state_fm1 == LOW)
  {
    flag_fm1 = false;
  }
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
  state_fm2 = digitalRead(FEEDBACK_M2);
  if (state_fm2 == HIGH && flag_fm2 == false)
  {
    flag_fm2 = true;
    M2_STEPS += DIR_M2;
  }
  if (state_fm2 == LOW)
  {
    flag_fm2 = false;
  }

  portEXIT_CRITICAL_ISR(&timerMux);
}

void IRAM_ATTR onTimerJoy()
{
  portENTER_CRITICAL_ISR(&timerMux);
  flagJoyTimer = true;
  portEXIT_CRITICAL_ISR(&timerMux);
}

void moveM1(float mapped1)
{
  VEL_L1 = mapped1 * homingGain * GAIN;
}

void moveM2(float mapped2)
{
  VEL_L2 = mapped2 * homingGain * GAIN;
}

void configInitialMotorsPositions()
{
  //Cálculo y configuración longitud de cuerda inicial (en pulsos/pasos de motor)
  M1_STEPS = sqrt(pow(INI_X, 2) + pow(INI_Y, 2)) / RES_M1;
  M2_STEPS = sqrt(pow(POSX_M2 - INI_X, 2) + pow(INI_Y, 2)) / RES_M2;
}

void endSDWrite()
{
  //Cerrar archivo de escritura y resetear contadores y banderas de grabación
  fileWrite.close();
  writeCycle = 0;
  SD_REC_CNT = 0;
  fileOK = false;
}

void startSDWrite()
{
  /*Si la tarjeta SD se ha inicializado correctamente intentar abrir el archivo e informar por puerto serie en caso de error*/
  if (SDOK)
  {
    fileWrite = SD.open("/grabacion.txt", FILE_WRITE);
    if (!fileWrite)
    {
      Serial.println("Error apertura archivo SD");
      fileOK = false;
    }
    else
    {
      fileWrite.close();
      SD.remove("/grabacion.txt");
      fileWrite = SD.open("/grabacion.txt", FILE_WRITE);
      fileOK = true;
    }
  }
  else
  {
    Serial.println("Error lectura tarjeta SD");
  }
}

void startSDRead()
{
  /*Al inicio del ciclo de lectura verificar que el módulo SD se haya inicializado correctamente e informar en caso de error
    Resetear banderas de posición de homing
    Contar cantidad de lineas que posee el archivo, cada linea representa una consigna
  */
  hom_L1 = false;
  hom_L2 = false;
  readStart = true;
  if (SDOK)
  {
    readCycle = 0;
    maxCycles = 0;
    fileRead = SD.open("/grabacion.txt", FILE_READ);
    if (!fileRead)
    {
      Serial.println("Error apertura archivo SD");
      filereadOK = false;
    }
    else
    {
      filereadOK = true;
      int linecount = 0;

      while (fileRead.available())
      {
        fileRead.readStringUntil('\n');
        linecount++;
      }
      fileRead.close();
      readLines = linecount;
      maxCycles = (readLines / REPLAY_IT_LINES) + 1;
      Serial.println("Filas leidas: " + String(linecount));
    }
  }
  else
  {
    Serial.println("Error lectura tarjeta SD");
  }
}

void endSDRead()
{
  //Cerrar el archivo de lectura y resetear contadores y banderas
  fileRead.close();
  readCycle = 0;
  readLines = 0;
  filereadOK = false;
  readStart = false;
}

void writeToSD()
{
  if (!fileOK)
  {
    startSDWrite();
  }
  else if (fileOK)
  {
    Serial.println(writeCycle);
    while (cnt < SD_BUFFER_SIZE)
    {
      fileWrite.println(String(cnt + (SD_BUFFER_SIZE * writeCycle)) + "," + String(SD_BUFFER[cnt][0]) + "," + String(SD_BUFFER[cnt][1]) + "," + String(SD_BUFFER[cnt][2]) + "," + String(SD_BUFFER[cnt][3]) + "," + String(SD_BUFFER[cnt][4]));
      cnt++;
    }
    cnt = 0;
    if (writeCycle >= RECORD_MAX_LENGTH)
    {
      endSDWrite();
    }
  }
}

void iniciarReproduccion()
{
  SD_PLAYING = true;
  cycleStart = true;
  playState = 1;
  startSDRead();
}
void detenerReproduccion()
{
  SD_PLAYING = false;
  playState = 0;
  resumePlay = false;
  endSDRead();
}
void resumirReproduccion()
{
  resumePlay = true;
  playState = 2;
}
void iniciarGrabacion()
{
  SD_RECORDING = true;
  startSDWrite();
}
void detenerGrabacion()
{
  endSDWrite();
  SD_RECORDING = false;
}

void whileSDRead()
{

  if (readLines != 0)
  {
    if (readCycle == 0)
    {
      fileRead = SD.open("/grabacion.txt", FILE_READ);
    }
    if (readCycle < maxCycles)
    {
      int counterline = 0;
      for (int i = 0; i < REPLAY_IT_LINES; i++)
      {
        for (int j = 0; j < 5; j++)
        {
          REPLAY_BUFFER[i][j] = 0;
        }
      }
      while (fileRead.available() && counterline < REPLAY_IT_LINES)
      {
        fileRead.readStringUntil(',').toFloat();
        REPLAY_BUFFER[counterline][0] = fileRead.readStringUntil(',').toFloat();
        REPLAY_BUFFER[counterline][1] = fileRead.readStringUntil(',').toFloat();
        REPLAY_BUFFER[counterline][2] = fileRead.readStringUntil(',').toFloat();
        REPLAY_BUFFER[counterline][3] = fileRead.readStringUntil(',').toFloat();
        REPLAY_BUFFER[counterline][4] = fileRead.readStringUntil('\n').toFloat();
        counterline++;
      }
      readCycle++;
    }
    else
    {
      Serial.println("End of file");
      fileRead.close();
      detenerReproduccion();

      return;
    }
  }
}

void moveXY()
{
  //Longitud de cuerda en milímetros
  L1 = PI * D1 * (M1_STEPS / (PPR_M1 * MICROSTEPS));
  L2 = PI * D2 * (M2_STEPS / (PPR_M2 * MICROSTEPS));

  //Angulos calculados en paso previo
  alphaAnt = alpha_L2;
  gammaAnt = gamma_L1;
  //Angulos actuales
  alpha_L2 = acosf(-((L1 * L1) - (POSX_M2 * POSX_M2) - (L2 * L2)) / (2 * POSX_M2 * L2));
  gamma_L1 = acosf(-((L2 * L2) - (POSX_M2 * POSX_M2) - (L1 * L1)) / (2 * POSX_M2 * L1));

  //Verificar que no haya error de cálculo para no perder el posicionamiento
  if (isnan(alpha_L2))
  {
    alpha_L2 = alphaAnt;
  }
  if (isnan(gamma_L1))
  {
    gamma_L1 = gammaAnt;
  }

  //Posicion en X Y de acuerdo a angulo y longitud de cuerda
  POS_Y = L1 * sinf(gamma_L1);
  POS_X = L1 * cosf(gamma_L1);

  float posy2 = L2 * sinf(alpha_L2);
  float posx2 = POSX_M2 - (L2 * cosf(alpha_L2));

  VEL_L1_ANT = VEL_L1;
  VEL_L2_ANT = VEL_L2;

  //Se verifica que el carro se encuentre dentro de los límites establecidos del canvas
  if (POS_X <= LIM_INF_X)
  {
    if (VEL_X < 0)
    {
      VEL_X = 0;
    }
  }
  if (POS_X >= LIM_SUP_X)
  {
    if (VEL_X > 0)
    {
      VEL_X = 0;
    }
  }
  if (POS_Y <= LIM_INF_Y)
  {
    if (VEL_Y < 0)
    {
      VEL_Y = 0;
    }
  }
  if (POS_Y >= LIM_SUP_Y)
  {
    if (VEL_Y > 0)
    {
      VEL_Y = 0;
    }
  }

  //Calculo auxiliar de velocidades de cuerda/motor
  VEL_L1_aux = (VEL_X * cosf(gamma_L1)) + (VEL_Y * cosf((PI / 2) - gamma_L1));
  VEL_L2_aux = (-VEL_X * cosf(alpha_L2)) + (VEL_Y * cosf((PI / 2) - alpha_L2));

  /*
  //Calculo auxiliar de angulo de velocidad consigna
  float alpha_xy = atanf(VEL_Y / VEL_X);
  if (isnan(alpha_xy))
  {
    alpha_xy = 0;
  }
  Serial.println(alpha_xy);
  */

  //Implementacion de rampa de acelecaración y frenado
  if (VEL_L1_ANT == 0 && VEL_L1_aux != 0)
  {
    //Rampa de aceleracion si la velocidad anterior es cero y la consigna actual no lo es
    rampUpL1 = true;
    rampDownL1 = false;
  }
  else if (VEL_L1_ANT != 0 && VEL_L1_aux == 0)
  {
    //Rampa de frenado si la velocidad consigna es 0 y la anterior no lo es
    rampDownL1 = true;
    rampUpL1 = false;
  }
  else if (!rampDownL1 && !rampUpL1)
  {
    VEL_L1 = VEL_L1_aux;
  }

  if (rampUpL1)
  {
    //En rampa de aceleración se incrementa la velocidad consigna
    VEL_L1 += 0.05 * VEL_L1_aux;
    if (VEL_L1 >= VEL_L1_aux)
    {
      //Si la velocidad actual es igual a la deseada se finaliza rampa de aceleracion
      VEL_L1 = VEL_L1_aux;
      rampUpL1 = false;
    }
  }
  if (rampDownL1)
  {
    //en rampa de frenado se disminuye constantemente la velocidad en cada paso
    VEL_L1 = VEL_L1_ANT * 0.9;
    if (abs(VEL_L1) < 0.08)
    {
      //Si la velocidad es lo suficientemente baja se detiene por completo
      VEL_L1 = 0;
      rampDownL1 = false;
    }
  }

  if (VEL_L2_ANT == 0 && VEL_L2_aux != 0)
  {
    rampUpL2 = true;
    rampDownL2 = false;
  }
  else if (VEL_L2_ANT != 0 && VEL_L2_aux == 0)
  {
    rampDownL2 = true;
    rampUpL2 = false;
  }
  else if (!rampDownL2 && !rampUpL2)
  {
    VEL_L2 = VEL_L2_aux;
  }
  if (rampUpL2)
  {
    VEL_L2 += 0.05 * VEL_L2_aux;
    if (VEL_L2 >= VEL_L2_aux)
    {
      VEL_L2 = VEL_L2_aux;
      rampUpL2 = false;
    }
  }
  if (rampDownL2)
  {
    VEL_L2 = VEL_L2_ANT * 0.9;
    if (abs(VEL_L2) < 0.08)
    {
      VEL_L2 = 0;
      rampDownL2 = false;
    }
  }

  if (COMMANDS_DISPLAY)
  {
    taskcnt++;
    if (taskcnt > 50)
    {
      taskcnt = 0;
      Serial.println("Total " + String(L1) + " " + String(L2) + " " + String(posy2) + " " + String(posx2) + " " + String(gamma_L1) + " " + String(alpha_L2) + " " + String(VEL_L1_aux) + " " + String(VEL_L2_aux));
    }
  }
}

void activarHoming()
{
  HOMING = true;
  digitalWrite(LED_HOMING, HIGH);
  digitalWrite(SERVO, LOW); //Desactivar Servo
  Serial.println("Homing");
}
void desactivarHoming()
{
  HOMING = false;
  digitalWrite(LED_HOMING, LOW);
  homingState = 0;
  Serial.println("End homing");
}

void activarCanvasConfig()
{
  CANVAS = true;
  digitalWrite(LED_HOMING, HIGH);
  digitalWrite(SERVO, LOW); //Desactivar Servo
  Serial.println("Canvasing");
}
void desactivarCanvasConfig()
{
  CANVAS = false;
  digitalWrite(LED_HOMING, LOW);
  canvasState = 0;
  Serial.println("End canvasing");
}

void handleShortPress()
{
  Serial.print("Short press");
  if (HOMING == false)
  {
    //Toggle servo
    if (digitalRead(SERVO) == LOW)
    {
      digitalWrite(SERVO, HIGH);
      Serial.println("Servo ON");
    }
    else
    {
      digitalWrite(SERVO, LOW);
      Serial.println("Servo OFF");
    }
  }
  else if (HOMING == true)
  {
    if (homingState < 1)
    {
      homingState++;
    }
    Serial.println("Homing state: " + String(homingState));
  }
  else if (CANVAS == true)
  {
    if (canvasState < 4)
    {
      canvasState++;
    }
    Serial.println("Canvas state: " + String(canvasState));
  }
}

void handleMediumPress()
{
  Serial.println("Medium press");
}

void handleLongPress()
{
  Serial.println("Long press");
  joyLongPressCnt++;
  switch (joyLongPressCnt)
  {
  case 1:
    activarHoming();
    break;
  case 2:
    desactivarHoming();
    break;
  case 3:
    activarCanvasConfig();
    break;
  case 4:
    desactivarCanvasConfig();
    joyLongPressCnt = 0;
    break;
  }
}

void buttonHandler()
{
  JoyXY.boton.prevState = JoyXY.boton.currentState;
  JoyXY.boton.currentState = digitalRead(BOTON_JOY);

  if (JoyXY.boton.currentState != JoyXY.boton.prevState)
  {
    delay(JoyXY.boton.debounce);
    JoyXY.boton.currentState = digitalRead(BOTON_JOY);
    if (JoyXY.boton.currentState == BTN_PRESSED)
    {
      JoyXY.boton.counter = millis();
    }

    if (JoyXY.boton.currentState == BTN_NOT_PRESSED)
    {
      unsigned long currentMillis = millis(); //Tiempo transcurrido desde boton presionado

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

void updateXY(float mappedX, float mappedY)
{
  //Implementación de banda muerta en lectura de valores del joystick
  VEL_X = 0;
  VEL_Y = 0;

  if (mappedX > DEADBAND_X || mappedX < -DEADBAND_X)
  {
    VEL_X = mappedX * JOY_GAIN;
  }
  if (mappedY > DEADBAND_Y || mappedY < -DEADBAND_Y)
  {
    VEL_Y = mappedY * JOY_GAIN;
  }
}

void joystickTask(void *parameter)
{
  //Configuración de Timer 1 para ejecutar ciclos periódicos en tarea de joystick
  timerJoy = timerBegin(1, 80, true); //Timer 1, prescaler 80 -> 1MHz
  timerAttachInterrupt(timerJoy, &onTimerJoy, true);
  timerAlarmWrite(timerJoy, 25000, true); //Desborde cada 50000 cuentas, interrupción cada 50 ms
  timerAlarmEnable(timerJoy);             //Funcion 'timerJoy' asociada a interrupción de timer1
  int ledCnt = 0;
  int cyclecnt = 0;
  while (1)
  {
    if (flagJoyTimer)
    {
      flagJoyTimer = false;
      digitalWrite(27, HIGH);

      if (debounceCounter < 10)
      {
        debounceCounter++;
      }
      else if (debounceCounter >= 10)
      {
        debounceCounter = 0;
      }

      if (CANVAS == true) //Rutina de configuración de tamaño del canvas
      {
        ledCnt++;
        if (ledCnt > 25)
        {
          ledCnt = 0;
          if (digitalRead(LED_HOMING))
          {
            digitalWrite(LED_HOMING, LOW);
          }
          else
          {
            digitalWrite(LED_HOMING, HIGH);
          }
        }
        L1 = PI * D1 * (M1_STEPS / (PPR_M1 * MICROSTEPS));
        L2 = PI * D2 * (M2_STEPS / (PPR_M2 * MICROSTEPS));
        switch (canvasState)
        {
        case 0:
          if (nextState)
          {
            //Esperar llegada a primera esquina
            nextState = false;
            canvasState = 1;
          }
        case 1:
          hom_L1 = false;
          hom_L2 = false;
          if (nextState)
          {
            //Una vez en la primera esquina se considera longitud de cuerda L1 = 0 y se avanza hacia la segunda esquina
            M1_STEPS = 0;
            L2 = 100000;
            canvasState = 2;
            nextState = false;
          }
          break;
        case 2:
          if (nextState && L1 > 0)
          {
            //En la segunda esquina se considera L2 = 0 y L1 igual al ancho total del canvas
            //Se almacena este valor en la memoria no volátil del ESP32
            M2_STEPS = 0;
            WIDTH = roundf(abs(L1));
            EEPROM.begin(64);
            EEPROM.writeFloat(32, WIDTH);
            EEPROM.end();
            POSX_M2 = WIDTH;
            canvasState = 3;
            nextState = false;
          }
          break;
        case 3:
          if (nextState)
          {
            //L1 debe ser igual o mayor que el largo de hipotenusa entre alto y ancho,
            //para asegurarse de que el carro ha verdaderamente descendido verticalmente
            float d = sqrt((WIDTH * WIDTH) + (L2 * L2));
            if (L1 >= d && L2 > 0)
            {
              //Tercera esquina, se considera que L2 es igual al alto total del canvas
              //Se almacena este valor en la memoria no volátil del ESP32
              HEIGHT = round(abs(L2));
              EEPROM.begin(64);
              EEPROM.writeFloat(48, HEIGHT);
              EEPROM.end();
              canvasState = 4;
              nextState = false;
            }
          }
          break;
        case 4:
          //Se ha guardado la posicion de las tres esquinas, es momento de llevar lentamente el carro
          // a la posicion de homing cargada en los parámetros
          if (!hom_L1 || !hom_L2)
          {
            digitalWrite(SERVO, LOW);

            L1 = PI * D1 * (M1_STEPS / (PPR_M1 * MICROSTEPS));
            L2 = PI * D2 * (M2_STEPS / (PPR_M2 * MICROSTEPS));

            float homL1 = sqrt(HOMING_X * HOMING_X + HOMING_Y * HOMING_Y);
            float homL2 = sqrt(((POSX_M2 - HOMING_X) * (POSX_M2 - HOMING_X)) + HOMING_Y * HOMING_Y);

            if (!hom_L1)
            {
              if (homL1 - L1 > 1)
              {
                VEL_L1 = 0.1;
              }
              else if (homL1 - L1 < -1)
              {
                VEL_L1 = -0.1;
              }
              else
              {
                VEL_L1 = 0;
                hom_L1 = true;
              }
            }
            if (!hom_L2)
            {
              if (homL2 - L2 > 1)
              {
                VEL_L2 = 0.1;
              }
              else if (homL2 - L2 < -1)
              {
                VEL_L2 = -0.1;
              }
              else
              {
                VEL_L2 = 0;
                hom_L2 = true;
              }
            }
          }
          else
          {
            //El carro ha llegado a posicion de homing
            //se termina rutina de configuración de canvas
            nextState = false;
            desactivarCanvasConfig();
          }
          break;
        }
      }
      else if (HOMING)
      {
        //Rutina de posicionamiento de referencia del carro
        L1 = PI * D1 * (M1_STEPS / (PPR_M1 * MICROSTEPS));
        L2 = PI * D2 * (M2_STEPS / (PPR_M2 * MICROSTEPS));
        switch (homingState)
        {
        case 0:
          if (nextState)
          {
            //En este estado se puede enrollar y desenrollar libremente los motores
            nextState = false;
            homingState = 1;
          }
        case 1:
          hom_L1 = false;
          hom_L2 = false;
          if (nextState)
          {
            //Al indicar fin de posicionamiento se cargan los valores de longitud de cuerda
            float homL1 = sqrt(HOMING_X * HOMING_X + HOMING_Y * HOMING_Y);
            float homL2 = sqrt(((POSX_M2 - HOMING_X) * (POSX_M2 - HOMING_X)) + HOMING_Y * HOMING_Y);
            M1_STEPS = (homL1 / (PI * D1)) * (PPR_M1 * MICROSTEPS);
            M2_STEPS = (homL2 / (PI * D2)) * (PPR_M2 * MICROSTEPS);
            homingState = 2;
            nextState = false;
          }
          break;
        case 2:
          //Fin de rutina de homing
          nextState = false;
          desactivarHoming();
          break;
        }
      }
      else if (SD_RECORDING)
      {
        //Rutina de grabación de movimientos
        //Cada cierto intervalo de tiempo se verifica la cantidad de clientes http conectados al ESP32
        //Para activar o desactivar el joystick HTML
        cnt_clientes++;
        if (cnt_clientes > 20)
        {
          cnt_clientes = 0;
          int clientes = WiFi.softAPgetStationNum();
          if (clientes != 0)
          {
            //Si hay un cliente se considera que está utilizando el joystick HTML
            joystickHTML = true;
          }
          else
          {
            joystickHTML = false;
          }
        }
        if (!joystickHTML)
        {
          //En caso de que no hayan clientes se le da prioridad a los comandos de joystick físico
          if(ENABLE_JOYSTICK){
            float mappedX = JoyXY.getMapped_X();
            float mappedY = JoyXY.getMapped_Y();
            updateXY(mappedX, mappedY);
          }
        }
        else
        {
          updateXY(html_CX, html_CY);
        }
        //En funcion de los valores leidos del joystick se calculan las velocidades asignadas a cada motor en este intervalo de tiempo
        moveXY();

        //Se almacenan estos valores en un arreglo temporal de tamaño 100
        int stServo = digitalRead(SERVO);
        SD_BUFFER[SD_REC_CNT][0] = VEL_X;
        SD_BUFFER[SD_REC_CNT][1] = VEL_Y;
        SD_BUFFER[SD_REC_CNT][2] = stServo;
        SD_BUFFER[SD_REC_CNT][3] = L1;
        SD_BUFFER[SD_REC_CNT][4] = L2;

        SD_REC_CNT++;
        if (SD_REC_CNT == SD_BUFFER_SIZE)
        {
          //Si se ha llenado el arreglo temporal, se lo graba en la memoria SD y se reinicia el contador para reutilizar este arreglo con las próximas consignas
          SD_REC_CNT = 0;
          writeToSD();
        }
      }
      else if (SD_PLAYING)
      {
        if (readStart)
        {
          if (cycleStart)
          {
            replay_cnt = 0;
            whileSDRead();
            cycleStart = false;
            replay_homing = true;
          }
          else
          {
            if (!hom_L1 || !hom_L2)
            {
              digitalWrite(SERVO, LOW);
              //homL1 y homL2 son las longitudes de cuerda correspondientes a la posicion inicial del movimiento grabado
              float homL1 = REPLAY_BUFFER[0][3];
              float homL2 = REPLAY_BUFFER[0][4];

              L1 = PI * D1 * (M1_STEPS / (PPR_M1 * MICROSTEPS));
              L2 = PI * D2 * (M2_STEPS / (PPR_M2 * MICROSTEPS));
              //Angulos calculados en paso previo
              alphaAnt = alpha_L2;
              gammaAnt = gamma_L1;
              //Angulos actuales
              alpha_L2 = acosf(-((L1 * L1) - (POSX_M2 * POSX_M2) - (L2 * L2)) / (2 * POSX_M2 * L2));
              gamma_L1 = acosf(-((L2 * L2) - (POSX_M2 * POSX_M2) - (L1 * L1)) / (2 * POSX_M2 * L1));

              //Verificar que no haya error de cálculo para no perder el posicionamiento
              if (isnan(alpha_L2))
              {
                alpha_L2 = alphaAnt;
              }
              if (isnan(gamma_L1))
              {
                gamma_L1 = gammaAnt;
              }
              //Posicion en X Y de acuerdo a angulo y longitud de cuerda
              POS_Y = L1 * sinf(gamma_L1);
              POS_X = L1 * cosf(gamma_L1);

              //Condiciones para llevar el carro a posición de referencia
              if (!hom_L1)
              {
                if (homL1 - L1 > 1)
                {
                  VEL_L1 = 0.1;
                }
                else if (homL1 - L1 < -1)
                {
                  VEL_L1 = -0.1;
                }
                else
                {
                  //Si la diferencia de longitud es menor a 1 mm se considera referenciado al inicio de movimiento
                  VEL_L1 = 0;
                  hom_L1 = true;
                }
              }
              if (!hom_L2)
              {
                if (homL2 - L2 > 1)
                {
                  VEL_L2 = 0.1;
                }
                else if (homL2 - L2 < -1)
                {
                  VEL_L2 = -0.1;
                }
                else
                {
                  VEL_L2 = 0;
                  hom_L2 = true;
                }
              }
            }
            else
            {
              //Si el carro se encuentra referenciado, esperar comando para resumir(iniciar) el movimiento
              if (resumePlay)
              {
                //Cargar valores de consignas y longitud de paso actual
                VEL_X = REPLAY_BUFFER[replay_cnt][0];
                VEL_Y = REPLAY_BUFFER[replay_cnt][1];
                digitalWrite(SERVO, REPLAY_BUFFER[replay_cnt][2]);
                L1 = REPLAY_BUFFER[replay_cnt][3];
                L2 = REPLAY_BUFFER[replay_cnt][4];

                moveXY();

                /*
                //Calculo auxiliar de error de longitud de las cuerdas en cada paso
                float errL1 = REPLAY_BUFFER[replay_cnt][3] - L1;
                float errL2 = REPLAY_BUFFER[replay_cnt][4] - L2;
                Serial.println(String(readCycle) + " " + String(errL1) + " " + String(errL2));
                */

                replay_cnt++;
                if (replay_cnt == REPLAY_IT_LINES)
                {
                  cycleStart = true;
                }
              }
            }
          }
        }
      }
      else //Modo manual
      {
        cnt_clientes++;
        if (cnt_clientes > 20)
        {
          cnt_clientes = 0;
          int clientes = WiFi.softAPgetStationNum();
          if (clientes != 0)
          {
            joystickHTML = true;
          }
          else
          {
            joystickHTML = false;
          }
        }
        if (!joystickHTML)
        {
          if (ENABLE_JOYSTICK)
          {
            float mappedX = JoyXY.getMapped_X();
            float mappedY = JoyXY.getMapped_Y();
            updateXY(mappedX, mappedY);
          }
        }
        else
        {
          updateXY(html_CX, html_CY);
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

        moveXY();
      }

      //Configurar canal PWM con nueva frecuencia (proporcional a la velocidad deseada) calculada
      FREQ_M1 = abs(VEL_L1) * MAX_FREQ_M1;
      FREQ_M2 = abs(VEL_L2) * MAX_FREQ_M2;

      ledcSetup(CHANNEL_M1, FREQ_M1, PWM_RES);
      ledcSetup(CHANNEL_M2, FREQ_M2, PWM_RES);

      if (VEL_L1 > 0)
      {
        ledcWrite(CHANNEL_M1, 1000);
        digitalWrite(DIRPIN_M1, HIGH);
      }
      else if (VEL_L1 < 0)
      {
        ledcWrite(CHANNEL_M1, 1000);
        digitalWrite(DIRPIN_M1, LOW);
      }
      else
      {
        ledcWrite(CHANNEL_M1, 0);
      }
      if (VEL_L2 > 0)
      {
        ledcWrite(CHANNEL_M2, 1000);
        digitalWrite(DIRPIN_M2, HIGH);
      }
      else if (VEL_L2 < 0)
      {
        ledcWrite(CHANNEL_M2, 1000);
        digitalWrite(DIRPIN_M2, LOW);
      }
      else
      {
        ledcWrite(CHANNEL_M2, 0);
      }
      digitalWrite(27, LOW);
    }

    vTaskDelay(2);
  }
}

void startup_serialprint()
{
  //Enviar por puerto serie todas las configuraciones cargadas
  Serial.println("Velocidad máxima - M1: " + String(MAXRPM_M1) + " M2: " + String(MAXRPM_M2));
  Serial.println("Velocidad nominal - M1: " + String(NOM_SPEED_M1) + " M2: " + String(NOM_SPEED_M2));
  //...
}

float stringFromHTMLToFloat(String x)
{
  //Se convierten los comandos de joystick de strin (50 ; 100) a flotante (-1.00 ; 1.00)
  return (x.toFloat() - 100.0) / 50.0;
}

void homingFromHTML(String x, String y)
{
  //Se convierten los comandos de joystick de strin (50 ; 100) a flotante (-1.00 ; 1.00)
  float x_html = (x.toFloat() - 100.0) / 50.0;
  float y_html = (y.toFloat() - 100.0) / 50.0;
  //Se envian comandos de joystick a cada motor por separado
  moveM1(x_html);
  moveM2(y_html);
  Serial.println("HX: " + String(x_html) + " HY: " + String(y_html));
}
void canvasingFromHTML(String x, String y)
{
  float x_html = (x.toFloat() - 100.0) / 50.0;
  float y_html = (y.toFloat() - 100.0) / 50.0;
  moveM1(x_html);
  moveM2(y_html);
  Serial.println("HX: " + String(x_html) + " HY: " + String(y_html));
}

void handleHttpAction(String input1, String input2)
{
  if (input1 == "GRABAR")
  {
    if (input2 == "0")
    {
      detenerGrabacion();
      //Serial.println("Detener grabacion");
    }
    else if (input2 == "1")
    {
      iniciarGrabacion();
      //Serial.println("Iniciar grabacion");
    }
  }
  else if (input1 == "PLAY")
  {
    if (input2 == "0")
    {
      detenerReproduccion();
      //Serial.println("Detener reproduccion");
    }
    else if (input2 == "1")
    {
      iniciarReproduccion();
      //Serial.println("Posición inicial reproducion");
    }
    else if (input2 == "2")
    {
      resumirReproduccion();
      //Serial.println("Iniciar reproduccion");
    }
  }
  else if (input1 == "HOMING")
  {
    if (input2 == "0")
    {
      desactivarHoming();
      Serial.println("FIN HOMING HTTP");
    }
    else if (input2 == "1")
    {
      activarHoming();
      Serial.println("INICIO HOMING HTTP");
    }
  }
  else if (input1 == "SERVO")
  {
    if (input2 == "0")
    {
      digitalWrite(SERVO, LOW);
      Serial.println("Servo Activado");
    }
    else if (input2 == "1")
    {
      digitalWrite(SERVO, HIGH);
      Serial.println("Servo Desactivado");
    }
  }
}

void updateHomingParams(String message1, String message2)
{
  //Se escribe en memoria no volátil las posiciones de homing ingresadas en la pantalla de Parámetros
  EEPROM.begin(64);
  HOMING_X = message1.toFloat();
  HOMING_Y = message2.toFloat();

  EEPROM.writeFloat(0, HOMING_X);
  EEPROM.writeFloat(16, HOMING_Y);

  HOMING_X = EEPROM.readFloat(0);
  HOMING_Y = EEPROM.readFloat(16);
  EEPROM.end();
  Serial.println("From eeprom save " + String(HOMING_X) + " " + String(HOMING_Y));
}
void updateCanvasParams(String message1, String message2)
{
  //Se escribe en memoria no volátil las dimensiones de canvas ingresadas en la pantalla de Parámetros
  EEPROM.begin(64);
  WIDTH = message1.toFloat();
  HEIGHT = message2.toFloat();

  EEPROM.writeFloat(32, WIDTH);
  EEPROM.writeFloat(48, HEIGHT);

  WIDTH = EEPROM.readFloat(32);
  HEIGHT = EEPROM.readFloat(48);
  POSX_M2 = WIDTH;
  EEPROM.end();
  Serial.println("From eeprom save canvas " + String(WIDTH) + " " + String(HEIGHT));
}

void startSDCard()
{
  //Se inicializa la tarjeta SD y se verifica que haya una presente antes de iniciar alguna lectura o escritura
  SDOK = SD.begin();
  if (SDOK)
  {
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE)
    {
      Serial.println("No hay tarjeta SD presente");
      SDOK = false;
    }
  }
  fileOK = false;
}

void asyncServerTask(void *parameter)
{
  //Pagina web de inicio
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });
  //GET request para resetear ESP
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/plain", "ok");
    ESP.restart();
  });
  //GET Request para acciones como homing, grabar y 'reproducir grabacion'
  server.on("/action", HTTP_GET, [](AsyncWebServerRequest *request) {
    String inputMessage1;
    String inputMessage2;
    if (request->hasParam(PARAM_MESSAGE4) && request->hasParam(PARAM_MESSAGE6))
    {
      inputMessage1 = request->getParam(PARAM_MESSAGE4)->value();
      inputMessage2 = request->getParam(PARAM_MESSAGE6)->value();
      handleHttpAction(inputMessage1, inputMessage2);
    }
    Serial.print("Action: " + inputMessage1 + " : " + inputMessage2);
    request->send(200, "text/plain", "OK");
  });
  //GET Request uilizado para recibir consignas X/Y de joystick HTML unicamente
  server.on("/joystick", HTTP_GET, [](AsyncWebServerRequest *request) {
    String message1;
    String message2;
    if (request->hasParam(PARAM_MESSAGE1) && request->hasParam(PARAM_MESSAGE2))
    {
      message1 = request->getParam(PARAM_MESSAGE1)->value();
      message2 = request->getParam(PARAM_MESSAGE2)->value();
    }
    else if (request->hasParam(PARAM_MESSAGE3))
    {
      message1 = request->getParam(PARAM_MESSAGE3)->value();
    }
    request->send(200, "text/plain", "OK");
    html_CX = stringFromHTMLToFloat(message1);
    html_CY = stringFromHTMLToFloat(message2);
  });

  //GET request para recibir comando de movimiento de motores en modo homing
  server.on("/homing", HTTP_GET, [](AsyncWebServerRequest *request) {
    String message1;
    String message2;
    if (request->hasParam(PARAM_MESSAGE7) && request->hasParam(PARAM_MESSAGE8))
    {
      message1 = request->getParam(PARAM_MESSAGE7)->value();
      message2 = request->getParam(PARAM_MESSAGE8)->value();
    }
    request->send(200, "text/plain", String(homingState));
    homingFromHTML(message1, message2);
  });

  //GET request para configurar tamaño de canvas
  server.on("/canvasing", HTTP_GET, [](AsyncWebServerRequest *request) {
    String message1;
    String message2;
    if (request->hasParam(PARAM_MESSAGE7) && request->hasParam(PARAM_MESSAGE8))
    {
      message1 = request->getParam(PARAM_MESSAGE7)->value();
      message2 = request->getParam(PARAM_MESSAGE8)->value();
    }
    request->send(200, "text/plain", String(canvasState));
    canvasingFromHTML(message1, message2);
  });

  //GET request para recibir configuraciones de Posicion de homing y dimensiones de Canvas
  server.on("/homing_params", HTTP_GET, [](AsyncWebServerRequest *request) {
    String message1;
    String message2;
    String message3;
    String message4;
    if (request->hasParam(PARAM_MESSAGE9) && request->hasParam(PARAM_MESSAGE10) && request->hasParam(PARAM_MESSAGE11) && request->hasParam(PARAM_MESSAGE12))
    {
      message1 = request->getParam(PARAM_MESSAGE9)->value();
      message2 = request->getParam(PARAM_MESSAGE10)->value();
      message3 = request->getParam(PARAM_MESSAGE11)->value();
      message4 = request->getParam(PARAM_MESSAGE12)->value();
      updateHomingParams(message1, message2);
      updateCanvasParams(message3, message4);
    }
    request->send(200, "text/plain", "OK");
  });

  //GET Request para pagina web de visualizacion de parámetros
  server.on("/plotter_params", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", params_html);
    //Serial.println("Params request");
  });

  //GET request para estado de reproduccion
  server.on("/getPlayState", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(playState));
  });

  //Get request para HTML de homing
  server.on("/homing_task", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", homing);
    //Serial.println("Homing request");
  });

  //GET request para HTML de canvas
  server.on("/canvas_task", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", canvasing);
    //Serial.println("Canvasing request");
  });
  //GET request para servo
  server.on("/getServo", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(digitalRead(SERVO)));
  });
  //GET request para velocidad de homing/canvas
  server.on("/getVel", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(homingGain));
  });

  //GET request para incrementar velocidad de homing
  server.on("/incVel", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (HOMING)
    {
      homingGain += 1;
      if (homingGain > 10)
      {
        homingGain = 10;
      }
      request->send(200, "text/plain", String(homingGain));
    }
    if (CANVAS)
    {
      canvasGain += 1;
      if (canvasGain > 10)
      {
        canvasGain = 10;
      }
      request->send(200, "text/plain", String(canvasGain));
    }
  });

  //GET request para reducir velocidad de homing
  server.on("/redVel", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (HOMING)
    {
      homingGain -= 1;
      if (homingGain < 1)
      {
        homingGain = 1;
      }
      request->send(200, "text/plain", String(homingGain));
    }
    if (CANVAS)
    {
      canvasGain -= 1;
      if (canvasGain < 1)
      {
        canvasGain = 1;
      }
      request->send(200, "text/plain", String(canvasGain));
    }
  });

  //GET request para posicion X del plotter
  server.on("/getPosX", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(POS_X));
  });
  //GET request para posicion Y del plotter
  server.on("/getPosY", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(POS_Y));
  });
  //GET request para estado de homing
  server.on("/getHomingStatus", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(homingState));
  });
  //GET request para incrementar estado de homing
  server.on("/incHomingStatus", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (homingState == 0)
    {
      activarHoming();
    }
    if (debounceCounter >= 3)
    {
      nextState = true;
      debounceCounter = 0;
    }
    request->send(200, "text/plain", String(homingState));
  });
  //GET request para estado de canvas
  server.on("/getCanvasStatus", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(canvasState));
  });
  //GET request para incrementar estado de canvas
  server.on("/incCanvasStatus", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (canvasState == 0)
    {
      activarCanvasConfig();
    }
    if (debounceCounter >= 3)
    {
      nextState = true;
      debounceCounter = 0;
    }
    request->send(200, "text/plain", String(canvasState));
  });
  //GET request para configuracion de homing x
  server.on("/getHX", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(HOMING_X));
    Serial.println("HX request");
  });
  //GET request para configuracion de homing y
  server.on("/getHY", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(HOMING_Y));
    Serial.println("HY request");
  });
  //GET request para configuracion de ancho de canvas
  server.on("/getCVW", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(WIDTH));
    Serial.println("CVW request");
  });
  //GET request para configuracion de homing y
  server.on("/getCVH", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(HEIGHT));
    Serial.println("CVH request");
  });

  server.onNotFound(notFound);
  server.begin();
  delay(500);
  while (1)
  {
    vTaskDelay(10);
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(22, OUTPUT); //Para depuración - Borrar luego
  pinMode(27, OUTPUT); //Para depuración - Borrar luego

  //Configuracion de GPIOS
  pinMode(POT_X, INPUT);
  pinMode(POT_Y, INPUT);
  pinMode(FEEDBACK_M1, INPUT);
  pinMode(FEEDBACK_M2, INPUT);
  pinMode(BOTON_JOY, INPUT);
  pinMode(DIRPIN_M1, OUTPUT);
  pinMode(DIRPIN_M2, OUTPUT);
  pinMode(LED_HOMING, OUTPUT);
  pinMode(SERVO, OUTPUT);

  //Se inicializa el plotter con el servo y homing desactivado
  digitalWrite(LED_HOMING, LOW);
  digitalWrite(SERVO, LOW);

  //Se verifican valores almacenados en memoria no volátil
  EEPROM.begin(64);
  HOMING_X = EEPROM.readFloat(0);
  HOMING_Y = EEPROM.readFloat(16);
  WIDTH = EEPROM.readFloat(32);  //ancho del canvas
  HEIGHT = EEPROM.readFloat(48); //alto del canvas
  EEPROM.end();
  if (isnan(HOMING_X) || isnan(HOMING_Y))
  {
    HOMING_X = 0.0;
    HOMING_Y = 0.0;
  }
  if (isnan(WIDTH) || isnan(HEIGHT))
  {
    WIDTH = 100.0;
    HEIGHT = 50.0;
  }

  POSX_M2 = WIDTH; //posicion X del motor 2 - igual al ancho total del canvas

  //Se considera que inicia en posicion de referencia, se carga el valor de cuentas correspondiente a esta posicion
  M1_STEPS = sqrt(pow(INI_X, 2) + pow(INI_Y, 2)) / RES_M1;
  M2_STEPS = sqrt(pow(POSX_M2 - INI_X, 2) + pow(INI_Y, 2)) / RES_M2;

  Serial.println("From eeprom " + String(HOMING_X) + " " + String(HOMING_Y));
  Serial.println("From eeprom " + String(WIDTH) + " " + String(HEIGHT));

  //Configuracion de pines de cuenta de pasos de motor como interrupción dado que con módulo PWM no se pueden contar lo pulsos
  attachInterrupt(digitalPinToInterrupt(FEEDBACK_M1), feedback_m1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(FEEDBACK_M2), feedback_m2, CHANGE);

  attachInterrupt(digitalPinToInterrupt(BOTON_JOY), botonJoy, FALLING);

  ledcSetup(CHANNEL_M1, FREQ_M1, PWM_RES);
  ledcSetup(CHANNEL_M2, FREQ_M2, PWM_RES);

  ledcAttachPin(STEP_M1, CHANNEL_M1); //Canal PWM de motor 1
  ledcAttachPin(STEP_M2, CHANNEL_M2); //Canal PWM de motor 2

  //Creacion de Access Point
  WiFi.softAP(SSID, PASSWORD, 1, BROADCAST_SSID, 1);
  delay(100);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  //Tarea para lectura de valores de joystick
  xTaskCreatePinnedToCore(
      joystickTask,    //Función asociada a la area
      "Joystick Task", //Nombre de la tarea
      50000,           // Stack size (bytes)
      NULL,            //Parametro de entrada de la tarea
      2,               //Prioridad de la tarea
      NULL,            //Handle de la tarea
      1);              //Core asociado a la tarea

  //Tarea para servidor web asincrono
  xTaskCreatePinnedToCore(
      asyncServerTask, //Función asociada a la area
      "Server Task",   //Nombre de la tarea
      50000,           // Stack size (bytes)
      NULL,            //Parametro de entrada de la tarea
      5,               //Prioridad de la tarea
      NULL,            //Handle de la tarea
      0);              //Core asociado a la tarea

  startSDCard();
  startup_serialprint();
}

void loop()
{
}
