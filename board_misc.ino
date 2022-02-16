
void set_TX_LED(int state){
  if(state > 0){
    PORTD &= ~(1<<5);
  } else {
    PORTD |= 1<<5;  
  }
}

void set_RX_LED(int state){
  if(state > 0){
    digitalWrite(RX_LED,0);
  } else {
    digitalWrite(RX_LED,1);
  }
}

void set_USER_LED(int state){
  if(state > 0){
    digitalWrite(USER_LED,1);
  } else {
    digitalWrite(USER_LED,0);
  }
}
