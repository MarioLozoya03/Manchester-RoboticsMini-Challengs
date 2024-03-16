// Incluye las bibliotecas necesarias para micro-ROS y la interacción con el hardware.
#include <micro_ros_arduino.h>
#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/float32.h>

// Definición de los publicadores y el suscriptor
rcl_publisher_t raw_pot_publisher; // Publicador para el valor crudo del potenciómetro
std_msgs_msg_Int32 raw_pot_msg; // Mensaje para el valor crudo

rcl_publisher_t voltage_publisher; // Publicador para el valor de voltaje calculado
std_msgs_msg_Float32 voltage_msg; // Mensaje para el voltaje

rcl_subscription_t pwm_duty_cycle_subscriber; // Suscriptor para el ciclo de trabajo de PWM
std_msgs_msg_Float32 pwm_duty_cycle_msg; // Mensaje para el ciclo de trabajo de PWM

// Ejecutor, soporte y nodo para micro-ROS
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

// Temporizadores para las tareas periódicas
rcl_timer_t timer_1, timer_2;

// Definición de pines
#define LED_PIN 13 // Pin para el LED de error
#define PWM_PIN 15 // Pin para la señal de PWM
#define ADC_PIN 36 // Pin para leer el potenciómetro

// Variables globales para almacenar los últimos valores leídos
int last_raw_value = 0; // Último valor crudo leído del potenciómetro
float last_voltage_value = 0.0; // Último valor de voltaje calculado

// Función de error para indicar problemas mediante el parpadeo del LED
void error_loop(){
  while(1){
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    delay(100);
  }
}

// Callback para el primer temporizador: Lee el potenciómetro y calcula el voltaje
void timer_1_callback(rcl_timer_t * timer, int64_t last_call_time) {
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    last_raw_value = analogRead(ADC_PIN); // Lee el valor crudo del potenciómetro
    last_voltage_value = (last_raw_value / 4096.0) * 3.3; // Calcula el voltaje
  }
}

// Callback para el segundo temporizador: Publica los valores crudo y de voltaje
void timer_2_callback(rcl_timer_t * timer, int64_t last_call_time) {
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {
    raw_pot_msg.data = last_raw_value; // Asigna el valor crudo al mensaje
    rcl_publish(&raw_pot_publisher, &raw_pot_msg, NULL); // Publica el valor crudo

    voltage_msg.data = last_voltage_value; // Asigna el valor de voltaje al mensaje
    rcl_publish(&voltage_publisher, &voltage_msg, NULL); // Publica el valor de voltaje
  }
}

// Callback para la suscripción al ciclo de trabajo de PWM
void pwm_duty_cycle_callback(const void * msgin) {
  const std_msgs_msgFloat32 * msg = (const std_msgsmsg_Float32 *)msgin;
  float duty_cycle_percentage = msg->data; // Obtiene el porcentaje del ciclo de trabajo del mensaje
  // Asegura que el porcentaje esté dentro de un rango válido
  duty_cycle_percentage = fmax(fmin(duty_cycle_percentage, 100.0), 0.0);
  // Calcula el valor de PWM basado en el porcentaje
  int pwm_value = (int)((duty_cycle_percentage / 100.0) * 255.0);
  // Ajusta el ciclo de trabajo de PWM en el canal 0
  ledcWrite(0, pwm_value);
}

// Configuración inicial del programa
void setup() {
  set_microros_transports();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  
  pinMode(ADC_PIN, INPUT);

  // Configura el PWM con una frecuencia de 5000Hz, canal 0 y resolución de 8 bits
  const int pwmFrequency = 5000;
  const int pwmChannel = 0;
  const int pwmResolution = 8;
  ledcSetup(pwmChannel, pwmFrequency, pwmResolution);
  ledcAttachPin(PWM_PIN, pwmChannel);

  delay(2000);

  allocator = rcl_get_default_allocator();
  rclc_support_init(&support, 0, NULL, &allocator);
  rclc_node_init_default(&node, "micro_ros_esp32_node", "", &support);

  // Inicializa los publicadores y el suscriptor
  rclc_publisher_init_default(&raw_pot_publisher, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32), "micro_ros_esp32/raw_pot");
  rclc_publisher_init_default(&voltage_publisher, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32), "micro_ros_esp32/voltage");
  rclc_subscription_init_default(&pwm_duty_cycle_subscriber, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32), "/micro_ros_esp32/pwm_duty_cycle");

  // Inicializa y configura los temporizadores
  rclc_timer_init_default(&timer_1, &support, RCL_MS_TO_NS(10), timer_1_callback);
  rclc_timer_init_default(&timer_2, &support, RCL_MS_TO_NS(100), timer_2_callback);

  // Configura el ejecutor con las suscripciones y temporizadores
  rclc_executor_init(&executor, &support.context, 4, &allocator);
  rclc_executor_add_subscription(&executor, &pwm_duty_cycle_subscriber, &pwm_duty_cycle_msg, &pwm_duty_cycle_callback, ON_NEW_DATA);
  rclc_executor_add_timer(&executor, &timer_1);
  rclc_executor_add_timer(&executor, &timer_2);
}

// Bucle principal del programa
void loop() {
  delay(100); // Pequeña pausa para no saturar el CPU
  // Ejecuta las tareas pendientes del ejecutor, incluyendo el manejo de mensajes y temporizadores
  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
}
