
const int chr_pin = 2;
const int trig_pin = 3;



void setup() {
  // put your setup code here, to run once:
  pinMode(chr_pin, OUTPUT);
  digitalWrite(chr_pin, 0);
  pinMode(trig_pin, OUTPUT);
  digitalWrite(trig_pin, 0);  
  randomSeed(analogRead(0));

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



  
  
}
