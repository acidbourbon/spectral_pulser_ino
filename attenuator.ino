



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
