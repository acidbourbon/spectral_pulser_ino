

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


  testText();
  //delay(1000);

  //demo_plot();
  set_attenuator_dB(0);
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
//  pulse_mv_combo(400.*(float(read_att_pot())/1023.));

  
   int lines = 4;
   int i = random(0,lines);
   pulse_mv_combo(4*10*pow(2,i) ); // 10 because 20 db attenuator
  
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

  if(loop_cnt == 0 ){
      
    delay(100);
    tft.fillScreen(ILI9341_GREEN);
//     tft.fillRect(20,
//                180,
//                100,
//                20,
//                ILI9341_BLACK);
    tft_debug_print( 20,180,1,    "ADC_RISE: "+String(meas_rise_pot()) );
    tft_debug_print( 20,190,1,    "ADC_TAIL: "+String(meas_tail_pot()) );
    tft_debug_print( 20,200,1,    "ADC_TAIL: "+String(meas_bat()) );
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
