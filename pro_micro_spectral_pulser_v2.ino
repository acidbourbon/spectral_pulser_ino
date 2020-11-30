

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

const int debug_pos_x = 20;
const int debug_pos_y = 200;

int decode_adc_buttons(){


  const int button_adc_pin = 3;
  const int buttons = 4;
  const int adc_range = 1023;

  const int fraction = int( float(adc_range)/float(buttons) );
  const int safety_margin = fraction/2;
  return (analogRead(button_adc_pin) + safety_margin)/fraction -1;
  
}

void loop() {
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

  if(loop_cnt == 0){
    Serial.print("meas_tail_pot(): ");
    Serial.println(meas_tail_pot());
    Serial.print("meas_rise_pot(): ");
    Serial.println(meas_rise_pot());
    Serial.print("meas_bat(): ");
    Serial.println(meas_bat());
    Serial.println();
  }

  

  tft.fillRect(debug_pos_x,
               debug_pos_y,
               60,
               10,
               ILI9341_BLACK);
               
  tft.setCursor(debug_pos_x,debug_pos_y);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.println(decode_adc_buttons());           
  delay(100);
  
  
}
