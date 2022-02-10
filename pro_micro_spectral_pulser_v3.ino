

#include "global.h"



// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);








void setup() {
  // put your setup code here, to run once:
    
    
  init_attenuator_pins();
  init_pulser_pins();
  init_tft_pins();
  init_adc_aux_pins();
  
  
  randomSeed(analogRead(0));


  Serial.begin(115200);



 
  tft.begin();
  tft.setRotation(3);


  tft.fillScreen(ILI9341_BLACK);
  //testText();
  //delay(1000);

  //demo_plot();
  set_attenuator_dB(0);
}




float tau_rise_ns = 1; 
float tau_tail_ns = 10;


int loop_cnt = 0;

void loop() {
    

  
  //pulse_mv(400.*(float(read_att_pot())/1023.));
//   set_attenuator_dB( 31.*(float(read_att_pot())/1023.)   );
//   pulse_mv(400);
  //pulse_mv_combo(400.*(float(read_att_pot())/1023.));

  if(0){
    int lines = 4;
    int i = random(0,lines);
    pulse_mv_combo(4*10*pow(2,i) ); // 10 because 20 db attenuator
  }else {
      
//     float target_amplitude_mv = 200;
//     float q = abs(tau_rise_ns-tau_tail_ns)*target_amplitude_mv;
//     float max_amp = max_amplitude(q,tau_rise_ns,tau_tail_ns);
//     float factor = target_amplitude_mv/max_amp;
//     
//     
//     pulse_mv_combo(target_amplitude_mv*factor);
    
      
    pulse_mv_combo(400.*(float(read_att_pot())/1023.));
  }
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

  if(loop_cnt == 0  ){
      
    delay(100);
    
    tau_rise_ns = (meas_rise_pot() + R_SER_RISE)*C_RISE*1e9;
    tau_tail_ns = (meas_tail_pot() + R_SER_TAIL)*C_TAIL*1e9;

    //tft_debug_print( 20,180,1,    "RisePot (R): "+String(meas_rise_pot()) +"  " );
    //tft_debug_print( 20,190,1,    "TailPot (R): "+String(meas_tail_pot()) +"  " );
    tft_debug_print( 20,190,1,    "pk_time  (ns): "+String(  peaking_time(tau_rise_ns,tau_tail_ns) ) +"  " );
    tft_debug_print( 20,200,1,    "Battery (V): "+String(meas_bat(),2 ));
    tft_debug_print( 20,210,1,    "tau_rise (ns): "+String(tau_rise_ns ) +"  " );
    tft_debug_print( 20,220,1,    "tau_tail (ns): "+String(tau_tail_ns ) +"  " );
    
    pulse_preview(tau_rise_ns,tau_tail_ns);
    
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
