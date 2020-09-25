//GPIOs utilizados
#define POT_X       36      //Potenciómetro Eje X Joystick
#define POT_Y       39      //Potenciómetro Eje Y Joystick
#define STEP_M1     16      //Salida de pulsos de pasos para motor 1
#define STEP_M2     17      //Salida de pulsos de pasos para motor 2
#define DIRPIN_M1   4       //Salida de pulsos de dirección para motor 1
#define DIRPIN_M2   2       //Salida de pulsos de dirección para motor 2
#define ENDSTOP_X   32      //Entrada de fin de carrera de Eje X
#define ENDSTOP_Y   33      //Entrada de fin de carrera de Eje Y
#define FEEDBACK_M1 34      //Entrada de tren de pulsos de motor 1
#define FEEDBACK_M2 35      //Entrada de tren de pulsos de motor 2
#define BOTON_JOY   23      //Entrada para boton de joystick
#define LED_HOMING  25       //LED Indicador de homing encurso
#define LED_SERVO   26       //LED Indicador de servo activado desactivado
//Parámetros 
#define LIM_INF_X 50              //Límite inferior de posicion Eje X - Por debajo de este valor no se moverán más los motores
#define LIM_INF_Y 50              //Límite inferior de posicion Eje Y
#define LIM_SUP_X 450            //Límite superior de posición Eje X - Por encima de este valor no se moverán más los motores
#define LIM_SUP_Y 450            //Límite superior de posición Eje Y
#define PPR_M1 200.0              //Pasos por revolución de Stepper X
#define PPR_M2 200.0              //Pasos por revolución de Stepper Y
#define RES_M1 (PI * D1 / PPR_M1) //Avance lineal mm por paso motor 1
#define RES_M2 (PI * D2 / PPR_M2) //Avance lineal mm por paso motor 2
#define CHANNEL_M1 0              //Canal PWM Motor 1
#define CHANNEL_M2 2              //Canal PWM Motor 2
#define MAXRPM_M1 300.0           //Máximas RPM Motor 1
#define MAXRPM_M2 300.0           //Máximas RPM Motor 2
#define HOMING_MAXRPM_M1 100.0    //Máximas RPM Motor 1
#define HOMING_MAXRPM_M2 100.0    //Máximas RPM Motor 2
#define HOMING_X    250.0         //Posición de homing X
#define HOMING_Y    50.0          //Posición de homing Y
#define D1 10                     //Diametro de tambor/carrete de motor 1 en mm
#define D2 10                     //Diametro de tambor/carrete de motor 1 en mm
#define OFFSET_DISPLAY 1          //Informar por puerto serie mediciones  X-Y de Joystick
#define COMMANDS_DISPLAY 1        //Informar por puerto serie posicion de carro, calculo de longitudes de cuerda y posicion de motores (en pasos)
#define DEADBAND_X 0.1            //Banda muerta de eje X de Joystick para eliminar pequeños desplazamientos por ruido en la lectura
#define DEADBAND_Y 0.1            //Banda muerta de eje Y de Joystick para eliminar pequeños desplazamientos por ruido en la lectura
#define POSX_M2 500               //Posixion X del Motor 2. Se asumen los dos motores en Y=0 y el motor 1 en X1=0
#define JOY_CENTER_X 1875.0       //Valor obtenido de la posicion central X del joystick luego de primera calibración
#define JOY_CENTER_Y 1585.0       //Valor obtenido de la posicion central Y del joystick luego de primera calibración
#define JOY_GAIN    3             //Ganancia de Joystick para aumentar o dosminuir velocidad de movimiento del carro