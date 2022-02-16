


void init_attenuator_pins(){
    
  pinMode(ATT_CLK, OUTPUT);
  digitalWrite(ATT_CLK, 0);
  pinMode(ATT_LE, OUTPUT);
  digitalWrite(ATT_LE, 0);
  pinMode(ATT_SDI, OUTPUT);
  digitalWrite(ATT_SDI, 0);
    
}


void set_attenuator(int val){
  for (int i = 5; i >=0 ; i--){
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

void set_max_attenuation(){
   set_attenuator(0);
    
}
