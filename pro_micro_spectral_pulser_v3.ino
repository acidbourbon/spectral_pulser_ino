

#include "global.h"


// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);


const int chr_pin = 2;
const int trig_pin = 3;

const int adc_tail_pin = 1;
const int gpio_tail_pin = 7;
const int gpio_rise_a_pin = 8;
const int gpio_rise_b_pin = 9; // the one with the transistor
const int adc_rise_pin = 0;
const int adc_bat_pin = 2;


void init_pulser_pins() {
  pinMode(gpio_tail_pin, INPUT);
  pinMode(gpio_rise_a_pin, INPUT);
  pinMode(gpio_rise_b_pin, OUTPUT);
  
}

float meas_bat(){
  int adc_val = analogRead(adc_bat_pin);
  return float(adc_val)*2./1023.*3.3/4.28*4.4;
}

int meas_tail_pot(){

  const float r_ser_gpio = 1000.0;
  const float r_ser_pot = 10.0;

  int adc_val = 0;
  pinMode(gpio_tail_pin, OUTPUT);
  digitalWrite(gpio_tail_pin,HIGH);

  delay(20);
  adc_val = analogRead(adc_tail_pin);

  digitalWrite(gpio_tail_pin,LOW);
  pinMode(gpio_tail_pin, INPUT);  
  delay(20);

  float adc_quot = float(adc_val)/1023.0 ;
  return r_ser_gpio/(1.0/adc_quot - 1.0 ) - r_ser_pot;
  
}

int meas_rise_pot(){

  const float r_ser_gpio = 1000.0;
  const float r_ser_pot = 0.0;

  int adc_val = 0;
  pinMode(gpio_rise_a_pin, OUTPUT);
  digitalWrite(gpio_rise_a_pin,HIGH);
  digitalWrite(gpio_rise_b_pin,HIGH);

  delay(20);
  adc_val = analogRead(adc_rise_pin);

  digitalWrite(gpio_rise_a_pin,LOW);
  digitalWrite(gpio_rise_b_pin,LOW);
  pinMode(gpio_rise_a_pin, INPUT);  
  delay(20);

  float adc_quot = float(adc_val)/1023.0 ;
  return r_ser_gpio/(1.0/adc_quot - 1.0 ) - r_ser_pot;
  //return adc_val;
  
}



void setup() {
  // put your setup code here, to run once:
  pinMode(chr_pin, OUTPUT);
  digitalWrite(chr_pin, 0);
  pinMode(trig_pin, OUTPUT);
  digitalWrite(trig_pin, 0);  
  randomSeed(analogRead(0));

  init_pulser_pins();

  Serial.begin(115200);



  pinMode(TFT_RST,OUTPUT);
  digitalWrite(TFT_RST,0);
  delay(10);
  digitalWrite(TFT_RST,1);
  delay(500);
 
  tft.begin();
  tft.setRotation(3);


  //testText();
  //delay(1000);

  demo_plot();
}

void pulse_mv(float mv){

  //const float scale_voltage = 1.5;
  const float scale_voltage = 2.0;
  // dirty trick to counteract gain losses
  
  const int a_us = 1000; //us
  const float a = a_us*1e-6;
  const float tau = 100e3*10e-9; // RC
  const float b = exp(-float(a)/tau);
  float d = -log( (mv*scale_voltage)/3300. + b)*tau ;
  int d_us = int(d*1e6);
  int c_us = a_us-d_us;
  pulse(c_us,d_us);
  
}

void pulse(int charge_time_us, int discharge_time_us){

  // disable interrupts -> precise timing please
  cli();
  
  // default position
  digitalWrite(chr_pin,0);
  digitalWrite(trig_pin,0);
  delayMicroseconds(100);
  
  // charge
  digitalWrite(chr_pin,1);
  delayMicroseconds(charge_time_us);

  // discharge
  digitalWrite(chr_pin,0);
  delayMicroseconds(discharge_time_us); 

  // trigger and  full discharge
  digitalWrite(trig_pin,1);
  digitalWrite(chr_pin,0);

  delayMicroseconds(500);

  // default position
  digitalWrite(chr_pin,0);
  digitalWrite(trig_pin,0);

  // enable interrupts again
  sei();
  
}



int loop_cnt = 0;


int read_att_pot(void){
  return analogRead(ADC_POT);
}

int decode_adc_buttons(){


  const int button_adc_pin = ADC_BUTTONS ;
//   const int buttons = 4;
//   const int adc_range = 1023;

//   const int fraction = int( float(adc_range)/float(buttons) );
//   const int safety_margin = fraction/2;
//   return (analogRead(button_adc_pin) + safety_margin)/fraction -1;
  
// ADC: 0000-0018, bit code: 0000
// ADC: 0197-0235, bit code: 0001
// ADC: 0257-0294, bit code: 0011
// ADC: 0360-0398, bit code: 0101
// ADC: 0413-0451, bit code: 0010
// ADC: 0531-0569, bit code: 0110
// ADC: 0587-0625, bit code: 1001
// ADC: 0629-0667, bit code: 0100
// ADC: 0740-0778, bit code: 1010
// ADC: 0810-0848, bit code: 1100
// ADC: 0850-0888, bit code: 1000
  
  int adcval = analogRead(button_adc_pin);
  
  if (adcval <= 18)
    return 0b0000;
  else if (adcval <= 253)
    return 0b0001;
  else if (adcval <= 294)
    return 0b0011;
  else if (adcval <= 398)
    return 0b0101;
  else if (adcval <= 451)
    return 0b0010;
  else if (adcval <= 569)
    return 0b0110;
  else if (adcval <= 625)
    return 0b1001;
  else if (adcval <= 667)
    return 0b0100;
  else if (adcval <= 778)
    return 0b1010;
  else if (adcval <= 848)
    return 0b1100;
  else if (adcval <= 888)
    return 0b1000;
  else
    return -1;
  
}


void tft_debug_print(int debug_pos_x,
                     int debug_pos_y,
                     int size,
                     String text) {
    
  tft.fillRect(debug_pos_x,
               debug_pos_y,
               80,
               20,
               ILI9341_BLACK);
               
  tft.setCursor(debug_pos_x,debug_pos_y);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(size);
  tft.print(text);
    
}


void loop() {
    
const int debug_pos_x = 20;
const int debug_pos_y = 200;
  // put your main code here, to run repeatedly:

  //delay(5);

  int lines = 4;
  int i = random(0,lines);
  //pulse_mv(150);

  
  pulse_mv(4*10*pow(2,i) ); // 10 because 20 db attenuator
  
//
//  switch(i){
//    case 0:
//      pulse_mv(300);
//      break;
//    case 1:
//      pulse_mv(50);
//      break;
//    case 2:
//      pulse_mv(100);
//      break;
//    case 3:
//      pulse_mv(150);
//      break;
//    case 4:
//      pulse_mv(200);
//      break;
//    case 5:
//      pulse_mv(250);
//      break;
//  }


  loop_cnt = (loop_cnt+1)%200;

//   if(loop_cnt == 0){
//     Serial.print("meas_tail_pot(): ");
//     Serial.println(meas_tail_pot());
//     Serial.print("meas_rise_pot(): ");
//     Serial.println(meas_rise_pot());
//     Serial.print("meas_bat(): ");
//     Serial.println(meas_bat());
//     Serial.println();
//   }

  

  //tft.fillRect(debug_pos_x,
  //             debug_pos_y,
  //             80,
  //             20,
  //             ILI9341_BLACK);
  //             
  //tft.setCursor(debug_pos_x,debug_pos_y);
  //tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
  //int buttons = decode_adc_buttons();           
  //for (int j = 3; j>=0; j--)
  //  tft.print((buttons&(1<<j))>>j);          
  String dummy ="";
  int buttons = decode_adc_buttons();           
  for (int j = 3; j>=0; j--)
    dummy += String((buttons&(1<<j))>>j);          
  
  
  tft_debug_print( 20,200,2,dummy);
  delay(50);
  
  
}
