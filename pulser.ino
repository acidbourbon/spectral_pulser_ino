

void init_pulser_pins(){
    
  pinMode(DAC_CHARGE, OUTPUT);
  digitalWrite(DAC_CHARGE, 0);
  pinMode(PULSE_TRIGGER, OUTPUT);
  digitalWrite(PULSE_TRIGGER, 0);  
    
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
  
  set_max_attenuation();

  // default position
  digitalWrite(DAC_CHARGE,0);
  digitalWrite(PULSE_TRIGGER,0);

  // enable interrupts again
  sei();
  
}

void pulse_mv_combo(float mv){
  float max_amp_mv = 600.;
  
  float total_att = (mv/AMPLITUDE_CALIB_FACTOR+1e-9)/max_amp_mv;
  float total_att_dB = -20* log10(total_att);
  int half_dB_steps = int(total_att_dB*2);
  if (half_dB_steps > 63){
      half_dB_steps = 63;
  }
  set_attenuator(63-half_dB_steps);
  
  float rest_att_linear = total_att/pow(10, -float(half_dB_steps)/40. );
  pulse_mv(rest_att_linear * max_amp_mv);
}
