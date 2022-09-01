// #include <SoftwareSerial.h>
#include <TimerOne.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


#define clk 7                           //pin clock encoder
#define dt  6                           //pin DT encoder
#define btn 4                           //pin boton encoder
#define pin_full_spectrum1 3            //Pin Rele 1 - FullSpectrum 
#define pin_full_spectrum2 5
#define pin_blanco_calido1 8
#define pin_blanco_calido2 9
#define pin_blanco_frio1 10
#define pin_blanco_frio2 11
#define sensor 2
#define pin_cooler_luces 12
#define pin_cooler_carpa 13

DHT dht(sensor, DHT22);
LiquidCrystal_I2C lcd(0x27, 16, 2);


String opciones_principal[] = {"VALORES", "PROGRAMACION", "MODO"};  //OPCIONES DE MENU PRINCIPAL

    String opciones_valores [] = {"Humedad y Temp", "Minimas", "Maximas", "Reset", "Datos Fase", "Estado Actual", "Atras"}; //OPCIONES DENTRO DE VALORES
    String opciones_programacion [] = {"Iinicial", "Vegetacion", "Floracion", "Personalizado", "Atras"}; //OPCIONES DENTRO DE PROGRAMACION
    String opciones_personalizacion [] = {"Iluminacion", "Timer", "Clima", "Atras"}; // OPCIONES DENTRO DE CADA OPCION DE PROGRAMACION- FASE
    String opciones_iluminacion [] = {"F.Spect 1", "F.Spect 2", "B.Calido 1", "B.Calido 2", "B.Frio 1", "B.Frio 2", "Atras"}; //OPCIONES DENTRO DE ILUMINACION
    String opciones_encendido [] = {"ON", "OFF",}; //OPCIONES DENTRO DE CADA LUZ
    String opciones_timer [] = {"Hs ON", "Hs OFF", "H.Inicial", "Duracion Dias", "Atras"}; //OPCIONES TIMER
    String opciones_fases [] = {"1 a 15 Dias", "Vegetacion", "Floracion", "Personalizado", "Atras"}; //MENU DE SELECCION DE FASE
    String opciones_clima [] = {"Hum.Min:", "Hum.Max:", "Temp.Min:", "Temp.Max:", "Atras"}; //MENU DE SELECCION DE FASE

int menu = 0 ;
float temperatura;
float humedad;
int temp_min ;
int temp_min_old;
int temp_max ;
int temp_max_old;
int hum_min ;
int hum_min_old;
int hum_max ;
int hum_max_old ;

 // variables para configuracion de luces en fase de inicio
bool inicio_fullspectrum1 = true ;
bool inicio_fullspectrum2 = true ;
bool inicio_blancofrio1 = false ;
bool inicio_blancofrio2 = false ;
bool inicio_blancocalido1 = false ;
bool inicio_blancocalido2 = false ;
int inicio_horas_encendido = 6 ;
int inicio_horas_apagado = 2;
int inicio_hora_inicio = 0;
int inicio_cantidad_dias = 6;


 // variables para configuracion de luces en fase de vegetacion
bool vegetacion_fullspectrum1 = true ;
bool vegetacion_fullspectrum2 = true ;
bool vegetacion_blancofrio1 = true ;
bool vegetacion_blancofrio2 = true ;
bool vegetacion_blancocalido1 = false ;
bool vegetacion_blancocalido2 = false ;
int vegetacion_horas_encendido = 16 ;
int vegetacion_horas_apagado = 8;
int vegetacion_hora_inicio = 0;
int vegetacion_cantidad_dias = 4;


 // variables para configuracion de luces en fase de vegetacion
bool floracion_fullspectrum1 = true ;
bool floracion_fullspectrum2 = true ;
bool floracion_blancofrio1 = false ;
bool floracion_blancofrio2 = false ;
bool floracion_blancocalido1 = true ;
bool floracion_blancocalido2 = true ;
int floracion_horas_encendido = 12 ;
int floracion_horas_apagado = 12 ;
int floracion_hora_inicio = 0;
int floracion_cantidad_dias = 20 ;


//cantidad de opciones por matriz de seleccion menu
int max_opciones_principal = sizeof(opciones_principal)/sizeof(opciones_principal[0]);
int max_opciones_valores = sizeof(opciones_valores)/sizeof(opciones_valores[0]);
int max_opciones_programacion = sizeof(opciones_programacion)/sizeof(opciones_programacion[0]);
int max_opciones_fases = sizeof(opciones_fases)/sizeof(opciones_fases[0]);
int max_opciones_personalizacion = sizeof(opciones_personalizacion)/sizeof(opciones_personalizacion[0]);
int max_opciones_iluminacion = sizeof(opciones_iluminacion)/sizeof(opciones_iluminacion[0]);
int max_opciones_encendido = sizeof(opciones_encendido)/sizeof(opciones_encendido[0]);
int max_opciones_timer = sizeof(opciones_timer)/sizeof(opciones_timer[0]);
int max_opciones_clima = sizeof(opciones_clima)/sizeof(opciones_clima[0]);
//int max_menu = sizeof(menu)/sizeof(menu[0]);

//variables para encoder
int state_clk_old;
int state_btn_old;
int count = 0;
int ultimo_cambio = 0;
int modo_carpa = 0 ; // 0 : INICIO 1 A 15 DIAS , 1 PARA VEGETACION , 2 PARA FLORACION, 3 PARA PERSONALIZADO 

//variables para reloj

int dias = 0;
int horas=0;
int minutos=0;
volatile int segundos=0;
volatile boolean actualizar=true;
int horas_on = 0 ;
int horas_off = 0 ;
int horas_old = 0;
int horas2 = 0;
int minutos2 = 0;
int dias_old = 0;
int horas_restantes ;
int dias_transcurridos ;


int opcion = 0; //variable para establecer valor de on y off en pantalla de luces

bool estado ; //establece el estado true = on / false = off 
int dias_restantes = 1;

void setup() {
 // Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(clk, INPUT);  //establece pin encoder como input
  pinMode(dt, INPUT);   //establece pin encoder como input
  pinMode(btn, INPUT_PULLUP);  //establece pin encoder como input

  //establecer pins output luces y cooler
  pinMode(pin_full_spectrum1, OUTPUT);          //Pin 
  pinMode(pin_full_spectrum2, OUTPUT);          //Pin 
  pinMode(pin_blanco_calido1, OUTPUT);          //Pin 
  pinMode(pin_blanco_calido2, OUTPUT);          //Pin 
  pinMode(pin_blanco_frio1, OUTPUT);            //Pin 
  pinMode(pin_blanco_frio2, OUTPUT);            //Pin 
  pinMode(pin_cooler_luces, OUTPUT);            //Pin 
  pinMode(pin_cooler_carpa, OUTPUT);            //Pin 
  

  Timer1.initialize(1000000);
  Timer1.attachInterrupt(manejadoraTimer);
  
  state_clk_old = digitalRead(clk);
  state_btn_old = digitalRead(btn);
  lcd.setCursor(0,0);
  lcd.print("Bienvenido");
  lcd.setCursor(0,1);

  int animacion = 80 ; //comienza animacion de incio
   
  lcd.print("C");
  delay(animacion);
    lcd.print("A");
  delay(animacion);
  lcd.print("N");
  delay(animacion);
  lcd.print("N");
  delay(animacion);
  lcd.print("A");
  delay(animacion);
  lcd.print("B");
  delay(animacion);
  lcd.print("I");
  delay(animacion);
  lcd.print("C");
  delay(animacion);
  lcd.print("O");
  delay(animacion);
  lcd.print(" ");
  delay(animacion);
  lcd.print("A");
  delay(animacion);
  lcd.print(" ");
  delay(animacion);
  lcd.print("F");
  delay(animacion);
  lcd.print("U");
  delay(animacion);
  lcd.print("L");
  delay(animacion);
  lcd.print("L");
  delay(animacion);

  dht.begin(); //inicializacion sensor dht22

  delay(2000);

mostrar_menu_principal(); //muestra el menu prinicipal
  


//establecer pins de reles en modo salida

pinMode(pin_full_spectrum1, OUTPUT) ;
pinMode(pin_full_spectrum2, OUTPUT) ;

pinMode(pin_blanco_calido1, OUTPUT) ;
pinMode(pin_blanco_calido2, OUTPUT) ;

pinMode(pin_blanco_frio1, OUTPUT) ;
pinMode(pin_blanco_frio2, OUTPUT) ;

pinMode(pin_cooler_luces, OUTPUT) ;
pinMode(pin_cooler_carpa, OUTPUT) ;


dias = 0;
horas=0;
minutos=0;
segundos=0;
horas_on = 0 ;
horas_off = 0 ;
horas_old = 0;
horas2 = 0;
minutos2 = 0;
dias_old = 0;
horas_restantes ;
dias_transcurridos ;

resetear_valores();

}

void loop() {


// debug serial




 if(actualizar == true){
    actualizarReloj();

  //Serial.print("Menu: ");
  //Serial.print(menu);
  //Serial.print("  -  ");
  //Serial.print("count: ");
  //Serial.print(count);
  //Serial.print("  -  ");
  //Serial.print("Hora: ");
  //Serial.print(horas);
  //Serial.print("  -  ");
 // Serial.print("opcion: ");
 // Serial.print(opcion);
//  Serial.print("  -  ");

//  Serial.print("MODO: ");
//  Serial.print(modo_carpa);

//  Serial.print("- Dias transcurridos: ");
//  Serial.print(dias_transcurridos) ;

  //Serial.print("- Estado: ");
//  Serial.println(estado);
    
    actualizar = false;
  }

    
  int state_btn = digitalRead(btn);

   temperatura = dht.readTemperature();
   humedad = dht.readHumidity();
   
  encoder();

  modo_funcionamiento();

  switch_modo();

  funcionDias();

  actualizar_historicos ();

  horas_old = horas;

  dias_old = dias;
  
  
   if(state_btn_old == HIGH && state_btn == LOW){
        ultimo_cambio = 1;
        run_option();
        }
    ultimo_cambio = 0;

  state_btn_old = state_btn;
  
}


void run_option(){


    switch (menu) {
       case 0:   //MENU PRINICIPAL
             switch (count){
                case 0:
                mostrar_valores();
                break;
                case 1:
                mostrar_programacion();
                break;
                default :
                break;
             }
             break;
                
        case 1: // MENU DE VALORES
            switch (count){
                 case 0:
                    mostrar_humedad_temperatura(); //do something when var equals 2
                    break;
                 case 1:
                    mostrar_humedad_temperatura();  //minima
                    break;
                 case 2:
                    mostrar_humedad_temperatura(); //maxima
                    break;
                 case 3:
                    resetear_valores();  //resetear
                    break;
                 case 4:
                    mostrar_humedad_temperatura(); //datos de fase 
                    break;
                 case 5:
                    mostrar_menu_principal();  //estado y horas
                    break;
                 case 6:
                    mostrar_humedad_temperatura(); //do something when var equals 2
                    break;
                 case 7:
                    mostrar_menu_principal();  //algo
                    break;
             }
             break;

         case 2:  //MENU FASES PERSONALIZADAS

            switch (count){
                 case 0:
                    mostrar_inicio(); 
                    break;
                    
                 case 1:
                    mostrar_vegetacion(); 
                    break;

                 case 2:
                    mostrar_floracion(); 
                    break;                  

                 case 3:
                   mostrar_personalizado();  
                    break;

                 case 4:
                    mostrar_menu_principal();  
                    break;
                    
             }
             break;

             
         case 3:  //MENU PERSONALIZACION INICIO 


            switch (count){
                 case 0:
                    mostrar_iluminacion_inicio(); //do something when var equals 2
                    break;
                    
                 case 1:
                    mostrar_timer_inicio();
                    break;

                 case 2:
                    mostrar_clima_inicio();
                    break;            

                 case 3:
                    mostrar_menu_principal();  
                    break;      
             }
             break;

             
         case 31:  //MENU PERSONALIZACION LUCES INICIO 

            switch (count){
                 case 0:
                    fullspectrum1_inicio() ;
                    delay(20);
                    actualizarLuz_inicio()   ;       
                    mostrar_iluminacion_inicio();
                    break;
                 case 1:
                    fullspectrum2_inicio();  
                    delay(20);
                    actualizarLuz_inicio()   ; 
                    mostrar_iluminacion_inicio();
                    break;

                 case 2: 
                    blancocalido1_inicio();
                    delay(20);
                    actualizarLuz_inicio()   ;
                    mostrar_iluminacion_inicio(); 
                    break;
              
                 case 3:
                    blancocalido2_inicio();
                    delay(20);
                    actualizarLuz_inicio()   ;
                    mostrar_iluminacion_inicio(); 
                    break;            

                 case 4:
                    blancofrio1_inicio();
                    delay(20);
                    actualizarLuz_inicio()   ; 
                    mostrar_iluminacion_inicio();
                    break;   
                 case 5:
                    blancofrio2_inicio();
                    delay(20);
                    actualizarLuz_inicio()   ; 
                    mostrar_iluminacion_inicio();
                    break;
                 case 6:
                    mostrar_inicio();
                    break;       
             }
             break; 

         case 32:  //MENU PERSONALIZACION TIMER INICIO 

            switch (count){
                 case 0:
                  //  horasencendido_inicio(); 
                    break;
                 case 1:
                  //  horasapagado_inicio();  
                    break;
                 case 2: 
                  //  horadeinicio_inicio();
                    break;
                 case 3:
                  //  cantidaddedias_inicio();
                    break;            
                 case 4:
                    mostrar_inicio();
                    break;   
             }
             break;
             
         case 33:  //MENU PERSONALIZACION CLIMA INICIO 

            switch (count){
                 case 0:
                 //   humedadminima_inicio(); 
                    break;
                 case 1:
                 //   humedadmaxima_inicio();  
                    break;
                 case 2: 
                 //   temperaturaminima_inicio();
                    break;
                 case 3:
                 //   temperaturamaxima_inicio();
                    break;            
                 case 4:
                    mostrar_inicio();
                    break;   
                }

            break;

             
         case 4:  //MENU PERSONALIZACION VEGETACION 


            switch (count){
                 case 0:
                    mostrar_iluminacion_vegetacion(); 
                    break;
                 case 1:
                    mostrar_timer_vegetacion();
                    break;
                 case 2:
                    mostrar_clima_vegetacion();
                    break;            
                 case 3:
                    mostrar_menu_principal();  
                    break;      
             }
             break;

             
         case 41:  //MENU PERSONALIZACION ILUMINACION VEGETACION 

            switch (count){
                 case 0:
                    fullspectrum1_vegetacion() ;
                    delay(20);
                    actualizarLuz_vegetacion()   ;       
                    mostrar_iluminacion_vegetacion();
                    break;
                 case 1:
                    fullspectrum2_vegetacion();  
                    delay(20);
                    actualizarLuz_vegetacion()   ; 
                    mostrar_iluminacion_vegetacion();
                    break;

                 case 2: 
                    blancocalido1_vegetacion();
                    delay(20);
                    actualizarLuz_vegetacion()   ;
                    mostrar_iluminacion_vegetacion(); 
                    break;
              
                 case 3:
                    blancocalido2_vegetacion();
                    delay(20);
                    actualizarLuz_vegetacion()   ;
                    mostrar_iluminacion_vegetacion(); 
                    break;            

                 case 4:
                    blancofrio1_vegetacion();
                    delay(20);
                    actualizarLuz_vegetacion()   ; 
                    mostrar_iluminacion_vegetacion();
                    break;   
                 case 5:
                    blancofrio2_vegetacion();
                    delay(20);
                    actualizarLuz_vegetacion()   ; 
                    mostrar_iluminacion_vegetacion();
                    break;
                 case 6:
                    mostrar_inicio();
                    break;       
             }
             break; 

         case 42:  //MENU PERSONALIZACION TIMER VEGETACION 

            switch (count){
                 case 0:
                  //  horasencendido_vegetacion(); 
                    break;
                 case 1:
                  //  horasapagado_vegetacion();  
                    break;
                 case 2: 
                  //  horadeinicio_vegetacion();
                    break;
                 case 3:
                  //  cantidaddedias_vegetacion();
                    break;            
                 case 4:
                    mostrar_vegetacion();
                    break;     
             }
             break;
             
         case 43:  //MENU PERSONALIZACION CLIMA VEGETACION 

            switch (count){
                 case 0:
                 //   humedadminima_vegetacion(); 
                    break;
                 case 1:
                 //   humedadmaxima_vegetacion();  
                    break;
                 case 2: 
                 //   temperaturaminima_vegetacion();
                    break;
                 case 3:
                 //   temperaturamaxima_vegetacion();
                    break;            
                 case 4:
                    mostrar_vegetacion();
                    break;   
             }
             break;
 
         case 5:  //MENU PERSONALIZACION FLORACION 

            switch (count){
                 case 0:
                    mostrar_iluminacion_floracion(); 
                    break;
                 case 1:
                    mostrar_timer_floracion();
                    break;
                 case 2:
                    mostrar_clima_floracion();
                    break;            
                 case 3:
                    mostrar_menu_principal(); 
                    break;      
             }
             break;
           
         case 51:  //MENU PERSONALIZACION ILUMINACION FLORACION 

            switch (count){
                 case 0:
                    fullspectrum1_floracion() ;
                    delay(20);
                    actualizarLuz_floracion()   ;       
                    mostrar_iluminacion_floracion();
                    break;
                 case 1:
                    fullspectrum2_floracion();  
                    delay(20);
                    actualizarLuz_floracion()   ; 
                    mostrar_iluminacion_floracion();
                    break;

                 case 2: 
                    blancocalido1_floracion();
                    delay(20);
                    actualizarLuz_floracion()   ;
                    mostrar_iluminacion_floracion(); 
                    break;
              
                 case 3:
                    blancocalido2_floracion();
                    delay(20);
                    actualizarLuz_floracion()   ;
                    mostrar_iluminacion_floracion(); 
                    break;            

                 case 4:
                    blancofrio1_floracion();
                    delay(20);
                    actualizarLuz_floracion()   ; 
                    mostrar_iluminacion_floracion();
                    break;   
                 case 5:
                    blancofrio2_floracion();
                    delay(20);
                    actualizarLuz_floracion()   ; 
                    mostrar_iluminacion_floracion();
                    break;
                 case 6:
                    mostrar_inicio();
                    break;       
             }
             break; 

         case 52:  //MENU PERSONALIZACION TIMER FLORACION 

            switch (count){
                 case 0:
                 //   horasencendido_floracion(); 
                    break;
                 case 1:
                 //   horasapagado_floracion();  
                    break;
                 case 2: 
                 //   horadeinicio_floracion();
                    break;
                 case 3:
                 //   cantidaddedias_floracion();
                    break;            
                 case 4:
                    mostrar_floracion();
                    break;   
             }
             break;
             
         case 53:  //MENU PERSONALIZACION CLIMA FLORACION 

            switch (count){
                 case 0:
                 //   humedadminima_floracion(); 
                    break;
                 case 1:
                 //   humedadmaxima_floracion();  
                    break;
                 case 2: 
                 //   temperaturaminima_floracion();
                    break;
                 case 3:
                 //   temperaturamaxima_floracion();
                    break;            
                 case 4:
                    mostrar_floracion();
                    break;   
               
             }
             
             break;

             
         default:
                // if nothing else matches, do the default
                // default is optional
         break;
         
    }
}


void encoder(){
  
  int state_clk = digitalRead(clk);
  int state_dt = digitalRead(dt);


  
  if(state_clk_old == HIGH && state_clk == LOW){  //actualiza valor de contador 
    if(state_dt == LOW){
      count--;
    }else{
      count++;
    }


   switch(menu) {
  

         case 0:
   
                   if(count < 0) count = 0; 
                   if(count > max_opciones_principal-1) {
                    count = max_opciones_principal - 1 ; 
                    };
                    mostrar_menu_principal();
                    break;

         case 1:
   
                if(count < 0) count = 0;
                if(count > max_opciones_valores-1) count = max_opciones_valores-1;
                mostrar_valores();
                break;

         case 11:
   
                if(count < 0) count = 0;
                if(count > max_opciones_valores-1) count = max_opciones_valores-1;
                mostrar_humedad_temperatura();
                break;
    
        case 2:
   
                if(count < 0) count = 0;
                if(count > max_opciones_programacion-1) count = max_opciones_programacion-1;
                mostrar_programacion();
                break;

        case 3:
   
                if(count < 0) count = 0;
                if(count > max_opciones_personalizacion-1) count = max_opciones_personalizacion-1;
                mostrar_inicio();
                break;

        case 31:
   
                if(count < 0) count = 0;
                if(count > max_opciones_iluminacion-1) count = max_opciones_iluminacion-1;
                actualizarLuz_inicio();
                mostrar_iluminacion_inicio();
                break;

        case 32:
   
                if(count < 0) count = 0;
                if(count > max_opciones_timer-1) count = max_opciones_timer-1;
                mostrar_timer_inicio();
                break;
                
        case 33:
   
                if(count < 0) count = 0;
                if(count > max_opciones_clima-1) count = max_opciones_clima-1;
                mostrar_clima_inicio();    
                break;


        case 4:
   
                if(count < 0) count = 0;
                if(count > max_opciones_personalizacion-1) count = max_opciones_personalizacion-1;
                mostrar_vegetacion();
                break;

        case 41:

                if(count < 0) count = 0;
                if(count > max_opciones_iluminacion-1) count = max_opciones_iluminacion-1;
                actualizarLuz_vegetacion();
                mostrar_iluminacion_vegetacion();
                break;

        case 42:
   
                if(count < 0) count = 0;
                if(count > max_opciones_timer-1) count = max_opciones_timer-1;
                mostrar_timer_vegetacion();
                break;
                
        case 43:
   
                if(count < 0) count = 0;
                if(count > max_opciones_clima-1) count = max_opciones_clima-1;
                mostrar_clima_vegetacion();    
                break;                

        case 5:
   
                if(count < 0) count = 0;
                if(count > max_opciones_personalizacion-1) count = max_opciones_personalizacion-1;
                mostrar_floracion();
                break;

        case 51:
   
                if(count < 0) count = 0;
                if(count > max_opciones_iluminacion-1) count = max_opciones_iluminacion-1;
                actualizarLuz_floracion();
                mostrar_iluminacion_vegetacion();
                break;

        case 52:
   
                if(count < 0) count = 0;
                if(count > max_opciones_timer-1) count = max_opciones_timer-1;
                mostrar_timer_floracion();
                break;
                
        case 53:
   
                if(count < 0) count = 0;
                if(count > max_opciones_clima-1) count = max_opciones_clima-1;
                mostrar_clima_floracion();    
                break;   
    
        default :
          break;

        }

  }
  delay(5);
  state_clk_old = state_clk;
}

void manejadoraTimer(){
    segundos++;
    actualizar=true;
}

void actualizarReloj(){


  // horas += segundos / 60;
 
  minutos += segundos / 60;
  segundos = segundos % 60;

  horas += minutos / 60;
  minutos = minutos % 60;

  dias += horas / 24 ;
  horas = horas % 24;

  horas2 = horas ;

 // minutos2 = minutos ;
  

}

void switch_modo(){ //establece el modo de la carpa segun la cantidad de dias

     switch (modo_carpa) {

          case 0:
          if ( dias_transcurridos == inicio_cantidad_dias) {
          dias_transcurridos=0; 
          modo_carpa = 1 ;
          }
          break;

          case 1:
          if ( dias_transcurridos == vegetacion_cantidad_dias) {
          dias_transcurridos=0; 
          modo_carpa = 2 ;
          }        
          break;

          case 2:
          if ( dias_transcurridos == floracion_cantidad_dias) {
          dias_transcurridos=0; 
          modo_carpa = 0 ;
          }            
          break;

          case 3:
          //accionLuz_personalizado();
          break;
        }

}

void funcionDias(){
     if (dias > dias_old){ //evalua el cambio de dia e incrementa la variable de dias transcurridos dentro de un modo
    dias_transcurridos++;
    Serial.println("Aumenta dia_transcurrido");
 }
}

void modo_funcionamiento(){ //establece el modo de funcionamiento on/of para las luces segun modo en que se encuentre la carpa y timers

    if (estado == true) { //evalua si la la carpa esta prendida

        if (horas_old < horas){ //suma una hora al timer en caso de que este encendido
            horas_on++;
        }

     switch (modo_carpa) {

          case 0:
          if (horas_on < inicio_horas_encendido) {
            accionLuz_inicio();
          } else {
            estado = false;
            horas_on = 0;
          }
          break;

          case 1:
          if (horas_on < vegetacion_horas_encendido) {
            accionLuz_vegetacion();
          } else {
            estado = false;
            horas_on = 0;
          }          
          break;

          case 2:
          if (horas_on < floracion_horas_encendido) {
            accionLuz_floracion();
          } else {
            estado = false;
            horas_on = 0;
          }          
          break;

          case 3:
          //accionLuz_personalizado();
          break;
        }
    }
        else if (estado == false ) {

        if (horas_old < horas){
            horas_off++;
        }

        switch (modo_carpa) {

          case 0:
          if (horas_off < inicio_horas_apagado) {
            accionLuz_off();
          } else {
            estado = true;
            horas_off = 0;
          }          
          break;

          case 1:
          if (horas_off < vegetacion_horas_apagado) {
            accionLuz_off();
          } else {
            estado = true;
            horas_off = 0;
          }              
          break;

          case 2:
          if (horas_off < floracion_horas_apagado) {
            accionLuz_off();
          } else {
            estado = true;
            horas_off = 0;
          }    
          break;

          case 3:
          //accionLuz_personalizado();
          break;
        }        
        }

  
}


//sector de menu por pantalla

void mostrar_menu_principal(){
  
    if (ultimo_cambio == 1) {
    count = 0 ;
     }
     
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Menu principal:");
  lcd.setCursor(0,1);
  lcd.print(opciones_principal[count]);
  menu = 0;
}


void mostrar_valores(){
  
    if (ultimo_cambio == 1) {
    count = 0 ;
     }
     
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Valores:");
  lcd.setCursor(0,1);
  lcd.print(opciones_valores[count]);
  menu = 1;
  
}

void mostrar_programacion(){
  
    if (ultimo_cambio == 1) {
    count = 0 ;
     }
     
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Programacion de fase:");
  lcd.setCursor(0,1);
  lcd.print(opciones_programacion[count]);
  menu=2;
}

void mostrar_humedad_temperatura(){
  
    if (ultimo_cambio == 1) {
    count = 0 ;
     }
     
    switch (count){
        case 0:  
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Clima Actual:");
            lcd.setCursor(0,1);
            //  lcd.print("Temp: ");
            lcd.print(temperatura);
            lcd.print("C - ");
            // lcd.print("Hum: ");
            lcd.print(humedad);
            lcd.print("% hum");
        break;
        case 1:
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Minimas histo:");
            lcd.setCursor(0,1);
            //  lcd.print("Temp: ");
            lcd.print(temp_min);
            lcd.print("C - ");
            // lcd.print("Hum: ");
            lcd.print(hum_min);
            lcd.print("% hum");
        break;
        case 2:  
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Maximas histo:");
            lcd.setCursor(0,1);
            //  lcd.print("Temp: ");
            lcd.print(temp_max);
            lcd.print("C - ");
            // lcd.print("Hum: ");
            lcd.print(hum_max);
            lcd.print("% hum");
        break;
        case 3:
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Resetear Valores");
            lcd.setCursor(0,1);
            lcd.print("Click para confirmar");
        break;
        case 4:  
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Fase:");
            lcd.setCursor(0,1);
            lcd.print("M: ");
            switch (modo_carpa) {
                case 0:
                lcd.print("I");
                break;
                case 1:
                lcd.print("V");
                break;
                case 2:
                lcd.print("F");
                break;
            }
            lcd.print(" -");
            lcd.print("Dias: ");
            lcd.print(dias_transcurridos);
            lcd.print("% hum");
        break;
        case 5:  
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Actual  -  Timer");
            lcd.setCursor(0,1);
            if (estado == true) {
                lcd.print("ON");
                lcd.print(" . ");
                lcd.print(horas_on);
            } else if (estado == false ) {
                lcd.print("OFF");
                lcd.print(" . ");
                lcd.print(horas_off);
            }
        
        break;
        default:

        break;

    }

  menu = 11;
}



void mostrar_inicio(){
  
    if (ultimo_cambio == 1) {
    count = 0 ;
     }
     
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("1 a 15 dias:");
  lcd.setCursor(0,1);
  lcd.print(opciones_personalizacion[count]);
  menu = 3;
}

void mostrar_vegetacion(){
  
    if (ultimo_cambio == 1) {
    count = 0 ;
     }
     
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Vegetacion:");
  lcd.setCursor(0,1);
  lcd.print(opciones_personalizacion[count]);
  menu = 4;
}

void mostrar_floracion(){
  
    if (ultimo_cambio == 1) {
    count = 0 ;
     }
     
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Floracion:");
  lcd.setCursor(0,1);
  lcd.print(opciones_personalizacion[count]);
  menu = 5;
}

void mostrar_personalizado(){
  
    if (ultimo_cambio == 1) {
    count = 0 ;
     }
     
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Personalizado:");
  lcd.setCursor(0,1);
  lcd.print(opciones_personalizacion[count]);
  menu = 6;
}



void mostrar_iluminacion_inicio(){

  
 if (menu != 31) {
  
    if (ultimo_cambio == 1) {
    count = 0 ;
     }
 }
     
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Luz - 1a15:");
  lcd.setCursor(0,1);
  lcd.print(opciones_iluminacion[count]);
  lcd.print("  ");
  lcd.print(opciones_encendido[opcion]);
  menu = 31;
}

void mostrar_timer_inicio() {
  
    if (ultimo_cambio == 1) {
    count = 0 ;
     }
     
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Timer - 1a15:");
  lcd.setCursor(0,1);
  lcd.print(opciones_timer[count]);

  switch (count) {

    case 0: 
    lcd.print(" ");
    lcd.print(inicio_horas_encendido);
    break;
    
    case 1: 
    lcd.print(" ");
    lcd.print(inicio_horas_apagado);
    break;

    case 2: 
    lcd.print(" ");
    lcd.print(inicio_hora_inicio);
    break;

    case 3: 
    lcd.print(" ");
    lcd.print(inicio_cantidad_dias);
    break;    

    default:
    break;
    
  }



  menu = 32;
}

void mostrar_clima_inicio(){
  
    if (ultimo_cambio == 1) {
    count = 0 ;
     }
     
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Clima - 1a15:");
  lcd.setCursor(0,1);
  lcd.print(opciones_clima[count]);
    
  menu = 33;
}




void mostrar_iluminacion_vegetacion(){

  
 if (menu != 41) {
  
    if (ultimo_cambio == 1) {
    count = 0 ;
     }
 }
     
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Luz - Vege:");
  lcd.setCursor(0,1);
  lcd.print(opciones_iluminacion[count]);
  lcd.print("  ");
  lcd.print(opciones_encendido[opcion]);

  menu = 41;
}

void mostrar_timer_vegetacion(){
  
    if (ultimo_cambio == 1) {
    count = 0 ;
     }
     
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Timer - Vege:");
  lcd.setCursor(0,1);
  lcd.print(opciones_timer[count]);

  switch (count) {

    case 0: 
    lcd.print(" ");
    lcd.print(vegetacion_horas_encendido);
    break;
    
    case 1: 
    lcd.print(" ");
    lcd.print(vegetacion_horas_apagado);
    break;

    case 2: 
    lcd.print(" ");
    lcd.print(vegetacion_hora_inicio);
    break;

    case 3: 
    lcd.print(" ");
    lcd.print(vegetacion_cantidad_dias);
    break;    

    default:
    break;
    
  }



  menu = 42;
}

void mostrar_clima_vegetacion(){
  
    if (ultimo_cambio == 1) {
    count = 0 ;
     }
     
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Clima - Vege:");
  lcd.setCursor(0,1);
  lcd.print(opciones_clima[count]);
    
  menu = 43;
}


void mostrar_iluminacion_floracion(){


 if (menu != 51) {
  
    if (ultimo_cambio == 1) {
    count = 0 ;
     }
 }
     
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("LUZ FLORACION:");
  lcd.setCursor(0,1);
  lcd.print(opciones_iluminacion[count]);
  lcd.print("  ");
  lcd.print(opciones_encendido[opcion]);

  menu = 51;
}

void mostrar_timer_floracion(){
  
    if (ultimo_cambio == 1) {
    count = 0 ;
     }
     
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("TIMER FLORACION:");
  lcd.setCursor(0,1);
  lcd.print(opciones_timer[count]);

  switch (count) {

    case 0: 
    lcd.print(" ");
    lcd.print(floracion_horas_encendido);
    break;
    
    case 1: 
    lcd.print(" ");
    lcd.print(floracion_horas_apagado);
    break;

    case 2: 
    lcd.print(" ");
    lcd.print(floracion_hora_inicio);
    break;

    case 3: 
    lcd.print(" ");
    lcd.print(floracion_cantidad_dias);
    break;    

    default:
    break;
    
  }



  menu = 52;
}

void mostrar_clima_floracion(){
  
    if (ultimo_cambio == 1) {
    count = 0 ;
     }
     
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("CLIMA FLORACION:");
  lcd.setCursor(0,1);
  lcd.print(opciones_clima[count]);
    
  menu = 53;
}


//modificacion de parametros luces//

void fullspectrum1_inicio() {
  
    if (inicio_fullspectrum1 == false) {
        inicio_fullspectrum1 = true ;
    } else if (inicio_fullspectrum1 == true) {
                inicio_fullspectrum1 = false ;
                } 
}

void fullspectrum2_inicio(){
  
    if (inicio_fullspectrum2 == false) {
        inicio_fullspectrum2 = true ;
    } else if (inicio_fullspectrum2 == true) {
                inicio_fullspectrum2 = false ;
                }    

  
}

void blancocalido1_inicio(){
  
    if (inicio_blancocalido1 == false) {
        inicio_blancocalido1 = true ;
    }else if (inicio_blancocalido1 == true) {
                inicio_blancocalido1 = false ;

        }          

}

void blancocalido2_inicio(){
  
    if (inicio_blancocalido2 == false) {
        inicio_blancocalido2 = true ;
    } else if (inicio_blancocalido2 == true) {
                inicio_blancocalido2 = false ;
                }   

  
}

void blancofrio1_inicio(){
  
    if (inicio_blancofrio1 == false) {
        inicio_blancofrio1 = true ;
    } else if (inicio_blancofrio1 == true) {
                inicio_blancofrio1 = false ;
                } 
}

void blancofrio2_inicio(){
  
    if (inicio_blancofrio2 == false) {
        inicio_blancofrio2 = true ;
    } else if (inicio_blancofrio2 == true) {
                inicio_blancofrio2 = false ;
                } 
}

void fullspectrum1_vegetacion(){
  
    if (vegetacion_fullspectrum1 == false) {
        vegetacion_fullspectrum1 = true ;
    } else if (vegetacion_fullspectrum1 == true) {
                vegetacion_fullspectrum1 = false ;
                } 
}

void fullspectrum2_vegetacion(){
  
    if (vegetacion_fullspectrum2 == false) {
        vegetacion_fullspectrum2 = true ;
    }  else if (vegetacion_fullspectrum2 == true) {
                vegetacion_fullspectrum2 = false ;
                } 
  
}

void blancocalido1_vegetacion(){
  
    if (vegetacion_blancocalido1 == false) {
        vegetacion_blancocalido1 = true ;
    }  else if (vegetacion_blancocalido1 == true) {
                vegetacion_blancocalido1 = false ;
                } 
}

void blancocalido2_vegetacion(){
  
    if (vegetacion_blancocalido2 == false) {
        vegetacion_blancocalido2 = true ;
    } else if (vegetacion_blancocalido2 == true) {
                vegetacion_blancocalido2 = false ;
                } 
}

void blancofrio1_vegetacion(){
  
    if (vegetacion_blancofrio1 == false) {
        vegetacion_blancofrio1 = true ;
    }   else if (vegetacion_blancofrio1 == true) {
                vegetacion_blancofrio1 = false ;
                } 
}

void blancofrio2_vegetacion(){
  
    if (vegetacion_blancofrio2 == false) {
        vegetacion_blancofrio2 = true ;
    }   else if (vegetacion_blancofrio2 == true) {
                vegetacion_blancofrio2 = false ;
                } 
}


void fullspectrum1_floracion(){
  
    if (floracion_fullspectrum1 == false) {
        floracion_fullspectrum1 = true ;
    }    else if (floracion_fullspectrum1 == true) {
                floracion_fullspectrum1 = false ;
                } 
}

void fullspectrum2_floracion(){
  
    if (floracion_fullspectrum2 == false) {
        floracion_fullspectrum2 = true ;
    }   else if (floracion_fullspectrum2 == true) {
                floracion_fullspectrum2 = false ;
                } 
}

void blancocalido1_floracion(){
  
    if (floracion_blancocalido1 == false) {
        floracion_blancocalido1 = true ;
    }  else if (floracion_blancocalido1 == true) {
                floracion_blancocalido1 = false ;
                } 
}

void blancocalido2_floracion(){
  
    if (floracion_blancocalido2 == false) {
        floracion_blancocalido2 = true ;
    }    else if (floracion_blancocalido2 == true) {
                floracion_blancocalido2 = false ;
                } 
}

void blancofrio1_floracion(){
  
    if (floracion_blancofrio1 == false) {
        floracion_blancofrio1 = true ;
    }   else if (floracion_blancofrio1 == true) {
                floracion_blancofrio1 = false ;
                } 
}

void blancofrio2_floracion(){
  
    if (floracion_blancofrio2 == false) {
        floracion_blancofrio2 = true ;
    }   else if (floracion_blancofrio2 == true) {
                floracion_blancofrio2 = false ;
                } 
}


void actualizarLuz_inicio() {


                switch (count) {

                    case 0:

                        if (inicio_fullspectrum1 == false) {
                            opcion = 0;
                        }else if (inicio_fullspectrum1 == true) {
                                opcion = 1;
                            }
                    break;

                    case 1:

                        if (inicio_fullspectrum2 == false) {
                            opcion = 0;
                        }else if (inicio_fullspectrum2 == true) {
                                opcion = 1;
                        }
                    break;


                    case 2:

                        if (inicio_blancocalido1 == false) {
                            opcion = 0;
                        }else if (inicio_blancocalido1 == true) {
                                opcion = 1;
                        }
                    break;


                    case 3:

                        if (inicio_blancocalido2 == false) {
                            opcion = 0;
                        } else if (inicio_blancocalido2 == true) {
                                opcion = 1;
                            }
                        
                    break;

                    case 4:

                        if (inicio_blancofrio1 == false) {
                            opcion = 0;
                        }
                            else if (inicio_blancofrio1 == true) {
                                opcion = 1;
                            
                        }
                    break;

                    case 5:

                        if (inicio_blancofrio2 == false) {
                            opcion = 0;
                        } else if (inicio_blancofrio2 == true ) {
                                opcion = 1;
                            }
                    break;

                    case 6:

                        
                    break;


                }
}

void actualizarLuz_vegetacion() {


                switch (count) {

                    case 0:

                        if (vegetacion_fullspectrum1 == false) {
                            opcion = 0;
                        }else if (vegetacion_fullspectrum1 == true) {
                                opcion = 1;
                            }
                    break;

                    case 1:

                        if (vegetacion_fullspectrum2 == false) {
                            opcion = 0;
                        }else if (vegetacion_fullspectrum2 == true) {
                                opcion = 1;
                        }
                    break;


                    case 2:

                        if (vegetacion_blancocalido1 == false) {
                            opcion = 0;
                        }else if (vegetacion_blancocalido1 == true) {
                                opcion = 1;
                        }
                    break;


                    case 3:

                        if (vegetacion_blancocalido2 == false) {
                            opcion = 0;
                        } else if (vegetacion_blancocalido2 == true) {
                                opcion = 1;
                            }
                        
                    break;

                    case 4:

                        if (vegetacion_blancofrio1 == false) {
                            opcion = 0;
                        }
                            else if (vegetacion_blancofrio1 == true) {
                                opcion = 1;
                            
                        }
                    break;

                    case 5:

                        if (vegetacion_blancofrio2 == false) {
                            opcion = 0;
                        } else if (vegetacion_blancofrio2 == true ) {
                                opcion = 1;
                            }
                    break;

                    case 6:

                        
                    break;


                }
}

void actualizarLuz_floracion() {


                switch (count) {

                    case 0:

                        if (floracion_fullspectrum1 == false) {
                            opcion = 0;
                        }else if (floracion_fullspectrum1 == true) {
                                opcion = 1;
                            }
                    break;

                    case 1:

                        if (floracion_fullspectrum2 == false) {
                            opcion = 0;
                        }else if (floracion_fullspectrum2 == true) {
                                opcion = 1;
                        }
                    break;


                    case 2:

                        if (floracion_blancofrio1 == false) {
                            opcion = 0;
                        }else if (floracion_blancofrio1 == true) {
                                opcion = 1;
                        }
                    break;


                    case 3:

                        if (floracion_blancofrio2 == false) {
                            opcion = 0;
                        } else if (floracion_blancofrio2 == true) {
                                opcion = 1;
                            }
                        
                    break;

                    case 4:

                        if (floracion_blancocalido1 == false) {
                            opcion = 0;
                        }
                            else if (floracion_blancocalido1 == true) {
                                opcion = 1;
                            
                        }
                    break;

                    case 5:

                        if (floracion_blancocalido2 == false) {
                            opcion = 0;
                        } else if (floracion_blancocalido2 == true ) {
                                opcion = 1;
                            }
                    break;

                    case 6:

                        
                    break;


                }
}


//encender luces segun modo

void accionLuz_inicio() {

     if (inicio_fullspectrum1 == false) {
        digitalWrite(pin_full_spectrum1, HIGH) ;
        }else if (inicio_fullspectrum1 == true) {
        digitalWrite(pin_full_spectrum1, LOW) ;
        }

     if (inicio_fullspectrum2 == false) {
        digitalWrite(pin_full_spectrum2, HIGH) ;
        }else if (inicio_fullspectrum2 == true) {
        digitalWrite(pin_full_spectrum2, LOW) ;
        }

     if (inicio_blancocalido1 == false) {
        digitalWrite(pin_blanco_calido1, HIGH) ;
        }else if (inicio_blancocalido1 == true) {
        digitalWrite(pin_blanco_calido1, LOW) ;
        }

     if (inicio_blancocalido2 == false) {
        digitalWrite(pin_blanco_calido2, HIGH) ;
        }else if (inicio_blancocalido2 == true) {
        digitalWrite(pin_blanco_calido2, LOW) ;
        }

     if (inicio_blancofrio1 == false) {
        digitalWrite(pin_blanco_frio1, HIGH) ;
        }else if (inicio_blancofrio1 == true) {
        digitalWrite(pin_blanco_frio1, LOW) ;
        }

     if (inicio_blancofrio2 == false) {
        digitalWrite(pin_blanco_frio2, HIGH) ;
        }else if (inicio_blancofrio2 == true) {
        digitalWrite(pin_blanco_frio2, LOW) ;
        }

}

void accionLuz_vegetacion() {

     if (vegetacion_fullspectrum1 == false) {
        digitalWrite(pin_full_spectrum1, HIGH) ;
        }else if (vegetacion_fullspectrum1 == true) {
        digitalWrite(pin_full_spectrum1, LOW) ;
        }

     if (vegetacion_fullspectrum2 == false) {
        digitalWrite(pin_full_spectrum2, HIGH) ;
        }else if (vegetacion_fullspectrum2 == true) {
        digitalWrite(pin_full_spectrum2, LOW) ;
        }

     if (vegetacion_blancocalido1 == false) {
        digitalWrite(pin_blanco_calido1, HIGH) ;
        }else if (vegetacion_blancocalido1 == true) {
        digitalWrite(pin_blanco_calido1, LOW) ;
        }

     if (vegetacion_blancocalido2 == false) {
        digitalWrite(pin_blanco_calido2, HIGH) ;
        }else if (vegetacion_blancocalido2 == true) {
        digitalWrite(pin_blanco_calido2, LOW) ;
        }

     if (vegetacion_blancofrio1 == false) {
        digitalWrite(pin_blanco_frio1, HIGH) ;
        }else if (vegetacion_blancofrio1 == true) {
        digitalWrite(pin_blanco_frio1, LOW) ;
        }

     if (vegetacion_blancofrio2 == false) {
        digitalWrite(pin_blanco_frio2, HIGH) ;
        }else if (vegetacion_blancofrio2 == true) {
        digitalWrite(pin_blanco_frio2, LOW) ;
        }

}

void accionLuz_floracion() {

     if (floracion_fullspectrum1 == false) {
        digitalWrite(pin_full_spectrum1, HIGH) ;
        }else if (floracion_fullspectrum1 == true) {
        digitalWrite(floracion_fullspectrum1, LOW) ;
        }

     if (floracion_fullspectrum2 == false) {
        digitalWrite(pin_full_spectrum2,  HIGH) ;
        }else if (floracion_fullspectrum2 == true) {
        digitalWrite(pin_full_spectrum2, LOW) ;
        }

     if (floracion_blancocalido1 == false) {
        digitalWrite(pin_blanco_calido1, HIGH) ;
        }else if (floracion_blancocalido1 == true) {
        digitalWrite(pin_blanco_calido1, LOW) ;
        }

     if (inicio_blancocalido2 == false) {
        digitalWrite(pin_blanco_calido2, HIGH) ;
        }else if (floracion_blancocalido2 == true) {
        digitalWrite(pin_blanco_calido2, LOW) ;
        }

     if (inicio_blancofrio1 == false) {
        digitalWrite(pin_blanco_frio1, HIGH) ;
        }else if (floracion_blancofrio1 == true) {
        digitalWrite(pin_blanco_frio1, LOW) ;
        }

     if (inicio_blancofrio2 == false) {
        digitalWrite(pin_blanco_frio2, HIGH) ;
        }else if (inicio_blancofrio2 == true) {
        digitalWrite(pin_blanco_frio2, LOW) ;
        }

}

//apaga las luces

void accionLuz_off() {

        digitalWrite(pin_full_spectrum1, HIGH) ;
        digitalWrite(pin_full_spectrum2, HIGH) ;
        digitalWrite(pin_blanco_calido1, HIGH) ;
        digitalWrite(pin_blanco_calido2, HIGH) ;
        digitalWrite(pin_blanco_frio1, HIGH) ;
        digitalWrite(pin_blanco_frio2, HIGH) ;

}


void actualizar_historicos(){
    if (humedad < hum_min) {
        hum_min = humedad ;
    };
    if (humedad > hum_max) {
        hum_max = humedad ;
    }
    if ( temperatura < temp_min ){
        temp_min = temperatura ;
    }
    if (temperatura > temp_max) {
        temp_max = temperatura ;
    }
}

void resetear_valores (){
    hum_min = 100 ;
    hum_max = 0;
    temp_min = 100;
    temp_max = 0 ;
}
