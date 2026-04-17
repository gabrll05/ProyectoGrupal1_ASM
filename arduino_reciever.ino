#include <Arduino.h>
#include "arduinoFFT.h"

/* --- CONSTANTES DE COMUNICACIÓN --- */
#define N 64                 // Tamaño de cada bloque de datos recibido
#define N_FFT 66             // Tamaño total del buffer para procesamiento
#define HEADER_A 0xAA         // Marcador de inicio de trama
#define HEADER_B 0xAB         
#define FOOTER 0x55           
#define SAMPLE_PERIOD 125     
#define BAUD_RATE_PC 10000  

/* --- CONSTANTES DE CONTROL --- */
#define ACK_SIGNAL 'K'   
#define GET 'G'          
#define ERROR 'E'        
#define START 'S'        
#define ACKNOWLEDGE 'A'  

/* --- VARIABLES GLOBALES --- */
uint8_t buffer[N];            
uint8_t FFT_buffer[N_FFT];    
int block_counter = 0;        

/* --- VARIABLES FFT --- */
float vReal[N_FFT];
float vImag[N_FFT];
ArduinoFFT<float> FFT = ArduinoFFT<float>(vReal, vImag, N_FFT, 8000);

void setup() {
  // En Leonardo, Serial.begin() no depende del baudrate físico por ser USB nativo,
  // pero se mantiene por estructura.
  Serial.begin(BAUD_RATE_PC);
  
  // Esperar a que el puerto serial USB esté listo (específico de Leonardo)
  while (!Serial); 

  // --- HANDSHAKE INICIAL ---
  while (true) {
    if (Serial.available() > 0 && Serial.read() == START) {
      Serial.write(ACKNOWLEDGE);
      break;
    }
  }
}
void loop() {
  // 1. LIMPIAR BUFFER Y SOLICITAR BLOQUE
  while(Serial.available() > 0) Serial.read(); 
  Serial.write(GET);

  // 2. ESPERAR HASTA QUE LLEGUE AL MENOS EL HEADER
  uint32_t t_espera = millis();
  while (Serial.available() == 0) { 
    if (millis() - t_espera > 1) { // Timeout reducido para mayor fluidez
      Serial.write(GET); 
      t_espera = millis();
    }
  }

  // 3. PROCESAR LA TRAMA EN CUANTO LLEGA EL PRIMER BYTE
  if (Serial.read() == HEADER_A) {
    
    // Serial.readBytes es eficiente: espera hasta que lleguen N bytes 
    // o hasta que se cumpla el Serial.setTimeout()
    Serial.readBytes(buffer, N);
    
    // Verificar que el cierre de trama sea correcto
    if (Serial.read() == FOOTER) {
      
      // Llenar buffer para procesamiento
      for (int i = 0; i < N; i++) {
        vReal[i] = (float)buffer[i];
        vImag[i] = 0.0;
      }

      // --- ESPACIO PARA PROCESAMIENTO MATEMÁTICO ---
      block_counter = 0; 

    } else {
      // Si el footer no está, informamos el error
      Serial.write(ERROR);
      while(Serial.available()) Serial.read(); 
    }
  } else {
    // Si lo primero que llegó no fue el HEADER, limpiamos basura
    while(Serial.available() && Serial.peek() != HEADER_A) Serial.read();
  }
}