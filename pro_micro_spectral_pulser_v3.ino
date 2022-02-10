

#include "global.h"


// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);





void init_pulser_pins() {
  pinMode(GPIO_TAIL, INPUT);
  pinMode(GPIO_RISE_A, INPUT);
  pinMode(GPIO_RISE_B, OUTPUT);
  
}

float meas_bat(){
  int adc_val = analogRead(VBAT2);
  return float(adc_val)*2./1023.*3.3/4.28*4.4;
}

int meas_tail_pot(){

  const float r_ser_gpio = 1000.0;
  const float r_ser_pot = 10.0;

  int adc_val = 0;
  pinMode(GPIO_TAIL, OUTPUT);
  digitalWrite(GPIO_TAIL,HIGH);

  delay(20);
  adc_val = analogRead(ADC_TAIL);

  digitalWrite(GPIO_TAIL,LOW);
  pinMode(GPIO_TAIL, INPUT);  
  delay(20);

  float adc_quot = float(adc_val)/1023.0 ;
  return r_ser_gpio/(1.0/adc_quot - 1.0 ) - r_ser_pot;
  
}

int meas_rise_pot(){

  const float r_ser_gpio = 1000.0;
  const float r_ser_pot = 0.0;

  int adc_val = 0;
  pinMode(GPIO_RISE_A, OUTPUT);
  digitalWrite(GPIO_RISE_A,HIGH);
  digitalWrite(GPIO_RISE_B,HIGH);

  delay(20);
  adc_val = analogRead(ADC_RISE);

  digitalWrite(GPIO_RISE_A,LOW);
  digitalWrite(GPIO_RISE_B,LOW);
  pinMode(GPIO_RISE_A, INPUT);  
  delay(20);

  float adc_quot = float(adc_val)/1023.0 ;
  return r_ser_gpio/(1.0/adc_quot - 1.0 ) - r_ser_pot;
  //return adc_val;
  
}



void setup() {
  // put your setup code here, to run once:
  pinMode(DAC_CHARGE, OUTPUT);
  digitalWrite(DAC_CHARGE, 0);
  pinMode(PULSE_TRIGGER, OUTPUT);
  digitalWrite(PULSE_TRIGGER, 0);  
  
  pinMode(ATT_CLK, OUTPUT);
  digitalWrite(ATT_CLK, 0);
  pinMode(ATT_LE, OUTPUT);
  digitalWrite(ATT_LE, 0);
  pinMode(ATT_SDI, OUTPUT);
  digitalWrite(ATT_SDI, 0);
  
  
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
  set_attenuator_dB(0);
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
  digitalWrite(DAC_CHARGE,0);
  digitalWrite(PULSE_TRIGGER,0);
  delayMicroseconds(100);
  
  // charge
  digitalWrite(DAC_CHARGE,1);
  delayMicroseconds(charge_time_us);

  // discharge
  digitalWrite(DAC_CHARGE,0);
  delayMicroseconds(discharge_time_us); 

  // trigger and  full discharge
  digitalWrite(PULSE_TRIGGER,1);
  digitalWrite(DAC_CHARGE,0);

  delayMicroseconds(500);

  // default position
  digitalWrite(DAC_CHARGE,0);
  digitalWrite(PULSE_TRIGGER,0);

  // enable interrupts again
  sei();
  
}

void pulse_mv_combo(float mv){
  float max_amp_mv = 400.;
  
  float total_att = (mv+1e-9)/max_amp_mv;
  float total_att_dB = -20* log10(total_att);
  int half_dB_steps = int(total_att_dB*2);
  if (half_dB_steps > 63){
      half_dB_steps = 63;
  }
  //half_dB_steps = 12;
  set_attenuator(63-half_dB_steps);
  float rest_att_linear = pow(10, -float(half_dB_steps)/40. );
  //pulse_mv(rest_att_linear * max_amp_mv);
  pulse_mv(max_amp_mv);
}



int read_att_pot(void){
  // i wired the poti the wrong way so inverting here
  return 1023 - analogRead(ADC_POT);
}

void set_attenuator(int val){
  for (int i = 7; i >=0 ; i--){
    digitalWrite(ATT_CLK, 0);
    
    digitalWrite(ATT_SDI, (val&(1<<i))>>i );
    digitalWrite(ATT_CLK, 1);
  }
    digitalWrite(ATT_CLK, 0);
    digitalWrite(ATT_LE,  1);
    digitalWrite(ATT_CLK, 1);
    digitalWrite(ATT_LE,  0);
    digitalWrite(ATT_CLK, 0);
    digitalWrite(ATT_SDI, 0);
}

void set_attenuator_dB(float dB){
   int LSB = int(dB*2);
   set_attenuator(63-LSB);
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
    
//   tft.fillRect(debug_pos_x,
//                debug_pos_y,
//                80,
//                20,
//                ILI9341_BLACK);
               
  tft.setCursor(debug_pos_x,debug_pos_y);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(size);
  tft.print(text);
    
}


int loop_cnt = 0;

void loop() {
    
const int debug_pos_x = 20;
const int debug_pos_y = 200;
  // put your main code here, to run repeatedly:

  //delay(5);

  
  //pulse_mv(400.*(float(read_att_pot())/1023.));
//   set_attenuator_dB( 31.*(float(read_att_pot())/1023.)   );
//   pulse_mv(400);
  pulse_mv_combo(400.*(float(read_att_pot())/1023.));

  
//   int lines = 4;
//   int i = random(0,lines);
//   pulse_mv(4*10*pow(2,i) ); // 10 because 20 db attenuator
  
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


  loop_cnt = (loop_cnt+1)%300;
  
  
//   if(loop_cnt == 0){
//         set_attenuator_dB(0);
//   }
//   if(loop_cnt == 50){
//         set_attenuator_dB(6);
//   }
//   if(loop_cnt == 100){
//         set_attenuator_dB(12);
//   }
//   if(loop_cnt == 150){
//         set_attenuator_dB(18);
//   }
//   if(loop_cnt == 200){
//         set_attenuator_dB(24);
//   }
//   if(loop_cnt == 250){
//         set_attenuator_dB(30);
//   }

//    if(loop_cnt == 0){
//      Serial.print("meas_tail_pot(): ");
//      Serial.println(meas_tail_pot());
//      Serial.print("meas_rise_pot(): ");
//      Serial.println(meas_rise_pot());
//      Serial.print("meas_bat(): ");
//      Serial.println(meas_bat());
//      Serial.println();
//    }

  if(loop_cnt == 0 and 0){
      
    delay(100);
    tft.fillRect(20,
               180,
               100,
               20,
               ILI9341_BLACK);
    tft_debug_print( 20,180,1,    "ADC_RISE: "+String(meas_rise_pot()) );
    tft_debug_print( 20,190,1,    "ADC_TAIL: "+String(meas_tail_pot()) );
    delay(300);
  } 
  
  if(0){ 
    String dummy ="";
    int buttons = decode_adc_buttons();           
    for (int j = 3; j>=0; j--)
        dummy += String((buttons&(1<<j))>>j);          
    
    
    tft_debug_print( 20,200,2,dummy);
    
    tft_debug_print( 200,200,2,String(read_att_pot()));
    
  }
  
  
  
}
