


void init_adc_aux_pins(){
    
  // Pins for reading out potis
  pinMode(GPIO_TAIL, INPUT);
  pinMode(GPIO_RISE_A, INPUT);
  pinMode(GPIO_RISE_B, OUTPUT);
    
}


float meas_bat(){
  int adc_val = analogRead(VBAT2);
  return float(adc_val)*2./1023.*3.3;
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

int read_att_pot(void){
  // i wired the poti the wrong way so inverting here
  return 1023 - analogRead(ADC_POT);
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
