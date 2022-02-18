

#include "global.h"



// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);



// the modes (state machine states)

#define NO_MODES 2
#define PULSE_MODE 0
#define ATTENUATOR_MODE 1





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
  clear_screen();
  set_attenuator_dB(0);
  
  set_TX_LED(0);
  set_RX_LED(0);
  set_USER_LED(0);
}


// ##################################################
// ##               global variables               ##
// ##################################################


// common global variables
// for all subroutines
uint8_t mode = PULSE_MODE;
// uint8_t mode = ATTENUATOR_MODE;

int loop_cnt = -1;

float tau_rise_ns = 1; 
float tau_tail_ns = 10;
float raw_amp_mV     = 0;
float Q_pC           = 0;
float real_amp_mV    = 0;
float bat_V          = 0;

int rise_pot = 2000;
int tail_pot = 2000;
int att_pot  = 2000;
int last_rise_pot = 2000;
int last_tail_pot = 2000;
int last_rise_adc = 2000;
int last_tail_adc = 2000;
int last_att_pot  = 2000;
uint8_t buttons_pressed = 0;

const String dummy   = "";
const String ch_mode = "ch mode";
uint8_t amp_range = AMP_RANGE_500MV;

uint8_t lines = 1;


void change_mode(uint8_t new_mode){
  mode = new_mode%NO_MODES;
  clear_screen();
  loop_cnt = -1; // this is how you identify a mode change
}


void loop() {
  
  // once every 4 ms
  
  
  //   if(0){
  //     int lines = 4;
  //     int i = random(0,lines);
  //     pulse_mV_combo(4*10*pow(2,i) ); // 10 because 20 db attenuator
  //   }
  
  
  
  
  
  
  switch (mode){
    case PULSE_MODE:
      pulse_mode_subroutine();
      break;
    case ATTENUATOR_MODE:
      attenuator_mode_subroutine();  
      break;
  }
  
  
}

// ##################################################
// ##          ATTENUATOR MODE subroutine          ##
// ##################################################


void attenuator_mode_subroutine(){
  
  const int col1_x = 15;
  const int col1_y = 100;
  
  const int l1_y = col1_y;
  const int l2_y = col1_y+30;
  const int l3_y = l2_y+30;
  const int l4_y = l3_y+30;
  
  // "setup()"
  if(loop_cnt == -1  ){
    tft_debug_print(col1_x,l1_y,"attenuator mode",GLCD_CL_RED,2); 
    tft_debug_print(col1_x,l2_y,"attenuation (dB): ",GLCD_CL_WHITE,2); 
    tft_debug_print(col1_x,l3_y,"U_out/U_in      : ",GLCD_CL_WHITE,2); 
    tft_debug_print(col1_x,l4_y,"U_in/U_out      : ",GLCD_CL_WHITE,2); 
    
    draw_lemos_and_jumper_field();
    draw_footer(ch_mode,dummy,dummy,dummy);
  }
  
  att_pot = read_att_pot();
  if (abs(att_pot-last_att_pot)>10 or (loop_cnt == -1)){
    last_att_pot = att_pot;
    float attenuation_dB = 63./2.*(1-float(att_pot)/1023.) ;
    attenuation_dB = set_attenuator_dB(attenuation_dB) +1.; // 1 dB is insertion loss
    tft_debug_print(col1_x+18*6*2,l2_y,String(attenuation_dB,1)+" ",GLCD_CL_WHITE,2); 
    tft_debug_print(col1_x+18*6*2,l3_y,String(pow(10,-attenuation_dB/20.),3)+" ",GLCD_CL_WHITE,2); 
    tft_debug_print(col1_x+18*6*2,l4_y,String(pow(10,attenuation_dB/20.),2)+" ",GLCD_CL_WHITE,2); 
  }
  
  delay(5);
  
  // must be before button things
  loop_cnt = (loop_cnt+1)%100;
  
  // specific button actions
  // don't evaluate buttons that often
  if((loop_cnt % 5) == 0){
    buttons_pressed = adc_buttons_pressed();  
    if(buttons_pressed & BUTTON_A){
      toggle_USER_LED();  
      
      change_mode(mode+1);
    }
//     if(buttons_pressed & BUTTON_B){
//       toggle_TX_LED();  
//     }
//     if(buttons_pressed & BUTTON_C){
//       toggle_RX_LED();  
//     }
//     if(buttons_pressed & BUTTON_D){
//       toggle_USER_LED();  
//     }
  }
  
  
}










// ##################################################
// ##          the PULSE MODE subroutine           ##
// ##################################################


void pulse_mode_subroutine(){
  
  // the static ("local global") variables 
  const int update_count_down_reset_val  = 20;
  const int update_count_down_redraw_val = 15;
  
  static int update_count_down = 0;
  
  static int meta_loop_cnt = 0;
  static int meta_loop_overflow = 10;
  
  const int scan_interval_fast = 50;
  const int scan_interval_slow = 500;
  static int scan_interval = scan_interval_fast;
  
  
  
  
  // "setup()"
  if(loop_cnt == -1){
    String d_str = "";
    if(        amp_range == AMP_RANGE_500MV){
      d_str = "0-500mV";
    } else if (amp_range == AMP_RANGE_50MV){
      d_str = "0-50mV";
    } else {
      d_str = "0-5mV";
    }
    draw_footer(ch_mode,dummy,dummy,d_str);
    prepare_plot_area();
  }
  
  
  if(loop_cnt <= 0 ){
    
    
    
    // blink the user LED when we are in the fast scannig mode
    if(scan_interval == scan_interval_fast){
      toggle_USER_LED();
    }
    
    
    before_pot_measurement();
    // for the capacitors to recover
    delay(20);
    int rise_adc = meas_rise_adc();
    int tail_adc = meas_tail_adc();
    after_pot_measurement();
    
    att_pot = read_att_pot();
    raw_amp_mV = MAX_DESIGN_AMP*(float(att_pot)/1023.) ;
    if (amp_range == AMP_RANGE_50MV){
      raw_amp_mV /=10.;
    } else if (amp_range == AMP_RANGE_5MV){
      raw_amp_mV /=100;
    }
    
    rise_pot = calc_rise_pot(rise_adc);
    tail_pot = calc_tail_pot(tail_adc);
    
    
    // has rise or tail pot turned?
    if( (abs(rise_adc - last_rise_adc) > 2) or (abs(tail_adc - last_tail_adc) > 2) or (loop_cnt == -1) ){
      tau_rise_ns = (rise_pot + R_SER_RISE)*C_RISE*1e9;
      tau_tail_ns = (tail_pot + R_SER_TAIL)*C_TAIL*1e9;
      
      Q_pC = calc_Q_pC(raw_amp_mV,tau_tail_ns);
      real_amp_mV = max_amplitude(Q_pC,tau_rise_ns,tau_tail_ns);
      
      last_rise_pot = rise_pot;
      last_tail_pot = tail_pot;
      last_rise_adc = rise_adc;
      last_tail_adc = tail_adc;
      
      
      
      display_status(0xFF & ~2); // update text and graph, but don't clear plot area
      meta_loop_cnt = 0; // delay unconditional redraw
      scan_interval = scan_interval_fast;
      update_count_down = update_count_down_reset_val;
      
      // has attenuator poti turned?
    } else if (abs(att_pot-last_att_pot)>2){
      Q_pC = calc_Q_pC(raw_amp_mV,tau_tail_ns);
      real_amp_mV = max_amplitude(Q_pC,tau_rise_ns,tau_tail_ns);
      last_att_pot = att_pot;
      display_status(8+4); // just update amplitude and plot
      meta_loop_cnt = 0; // delay unconditional redraw
      scan_interval = scan_interval_fast;
      update_count_down = update_count_down_reset_val;
      
    } else if (update_count_down > 0) {
      // is it time for a final redraw?
      if (update_count_down == update_count_down_redraw_val) {
        display_status(0xFF); // clear plot area, redraw graph, update text
        meta_loop_cnt = 0; // delay unconditional redraw
      } else if (update_count_down == 1) {
        // don't waste pulses - slower scanning for new input
        scan_interval = scan_interval_slow;
        //add_pk_ampl_to_plot();
        set_USER_LED(0);
      }
      // count down until you reach zero
      update_count_down --;
    }
    
    // for the capacitors to recover
    delay(50);
    
    
    
    meta_loop_cnt = (meta_loop_cnt+1)%meta_loop_overflow;
    if (meta_loop_cnt == 0){
      // update battery reading 
      bat_V = meas_bat();
      display_status(16);
    }
  } 
  
  
  
  uint8_t i = random(0,lines);
  
  // order a pulse
  pulse_mV_combo(raw_amp_mV/float(lines)*(i+1));
  
  // must be before button things
  loop_cnt = (loop_cnt+1)%scan_interval;
  
  // specific button actions
  // don't evaluate buttons that often
  if((loop_cnt % 5) == 0){
    buttons_pressed = adc_buttons_pressed();  
    if(buttons_pressed & BUTTON_A){
//       toggle_USER_LED();  
      
      change_mode(mode+1);
    }
//     if(buttons_pressed & BUTTON_B){
//       toggle_TX_LED();  
//     }
//     if(buttons_pressed & BUTTON_C){
//       toggle_RX_LED();  
//     }
    if(buttons_pressed & BUTTON_D){
      toggle_USER_LED();  
      amp_range = (amp_range +1)%AMP_RANGES;
      loop_cnt = -1;
    }
  }
  
}






