


void init_attenuator_pins(){
    
  pinMode(ATT_CLK, OUTPUT);
  digitalWrite(ATT_CLK, 0);
  pinMode(ATT_LE, OUTPUT);
  digitalWrite(ATT_LE, 0);
  pinMode(ATT_SDI, OUTPUT);
  digitalWrite(ATT_SDI, 0);
    
}


void set_attenuator(int val){
  
  //ATT_CLK -> D5 -> PC6
  //ATT_SDI -> D6 -> PD7
  //ATT_LE  -> D7 -> PE6

  // speeding it up by fast commands
  for (int i = 5; i >=0 ; i--){
    PORTC &= ~(1<<6); //digitalWrite(ATT_CLK, 0);
   
    PORTD &= ~(1<<7);
    PORTD |= ((val&(1<<i))>>i)<<7; //digitalWrite(ATT_SDI, (val&(1<<i))>>i );
    
    PORTC |= (1<<6); //digitalWrite(ATT_CLK, 1);
  }
    PORTC &= ~(1<<6); //digitalWrite(ATT_CLK, 0);
    PORTE |=  (1<<6); //digitalWrite(ATT_LE,  1);
    PORTC |=  (1<<6); //digitalWrite(ATT_CLK, 1);
    PORTE &= ~(1<<6); //digitalWrite(ATT_LE,  0);
    
    // // with arduino functions
    //   for (int i = 5; i >=0 ; i--){
    //     digitalWrite(ATT_CLK, 0);
    //     
    //     digitalWrite(ATT_SDI, (val&(1<<i))>>i );
    //     digitalWrite(ATT_CLK, 1);
    //   }
    //     digitalWrite(ATT_CLK, 0);
    //     digitalWrite(ATT_LE,  1);
    //     digitalWrite(ATT_CLK, 1);
    //     digitalWrite(ATT_LE,  0);
    //     //digitalWrite(ATT_CLK, 0);
    //     //digitalWrite(ATT_SDI, 0);
}

float set_attenuator_dB(float dB){
   int LSB = int(dB*2);
   set_attenuator(63-LSB);
   return LSB/2.;
}

void set_max_attenuation(){
   set_attenuator(0);
    
}
