

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
float raw_amp_mv     = 0;
float Q_pC           = 0;
float real_amp_mv    = 0;


int rise_pot = 2000;
int tail_pot = 2000;
int att_pot  = 2000;
int last_rise_pot = 2000;
int last_tail_pot = 2000;
int last_rise_adc = 2000;
int last_tail_adc = 2000;
int last_att_pot  = 2000;

const int update_count_down_reset_val  = 20;
const int update_count_down_redraw_val = 15;

int update_count_down = 0;

int loop_cnt = 0;
int meta_loop_cnt = 0;
int meta_loop_overflow = 10;

const int scan_interval_fast = 50;
const int scan_interval_slow = 500;
int scan_interval = scan_interval_fast;

void display_status(int clear){
    const int report_pos_y = 150;
    const int report_pos_x = 20;
    const int col2_xoffs   = 160;
    tft_debug_print( 180,4,1,    "RisePot (R): "+String(rise_pot) +"  " );
    tft_debug_print( 20,4,1,    "TailPot (R): "+String(tail_pot) +"  " );
    tft_debug_print( report_pos_x,report_pos_y   ,1,    "pk_time  (ns): "+String(  peaking_time(tau_rise_ns,tau_tail_ns) ) +"  " );
    tft_debug_print( report_pos_x,report_pos_y+10,1,    "pk_ampl. (mV): "+String(real_amp_mv ) +"  " );
    tft_debug_print( report_pos_x,report_pos_y+20,1,    "tau_rise (ns): "+String(tau_rise_ns ) +"  " );
    tft_debug_print( report_pos_x,report_pos_y+30,1,    "tau_tail (ns): "+String(tau_tail_ns ) +"  " );
    tft_debug_print( report_pos_x,report_pos_y+40,1,    "battery   (V): "+String(meas_bat(),2 ));
    
    tft_debug_print( report_pos_x+col2_xoffs,report_pos_y+0,1,    "charge (pC): "+String(Q_pC ) +"  " );
    
    pulse_preview(tau_rise_ns,tau_tail_ns,clear);
    
}




void loop() {
    
    // once every 4 ms
    
    
    if(0){
        int lines = 4;
        int i = random(0,lines);
        pulse_mv_combo(4*10*pow(2,i) ); // 10 because 20 db attenuator
    }
    
    pulse_mv_combo(raw_amp_mv);
    
    loop_cnt = (loop_cnt+1)%scan_interval;
    
    if(loop_cnt == 0  ){
        
        
        
        
        before_pot_measurement();
        // for the capacitors to recover
        delay(20);
        int rise_adc = meas_rise_adc();
        int tail_adc = meas_tail_adc();
        after_pot_measurement();
        
        att_pot = read_att_pot();
        raw_amp_mv = 600.*(float(att_pot)/1023.) ;
        
        rise_pot = calc_rise_pot(rise_adc);
        tail_pot = calc_tail_pot(tail_adc);
        
        
        
        if( (abs(rise_adc - last_rise_adc) > 2) or (abs(tail_adc - last_tail_adc) > 2)
            or (abs(att_pot-last_att_pot)>2)
        ){
            meta_loop_cnt = 0;
            tau_rise_ns = (rise_pot + R_SER_RISE)*C_RISE*1e9;
            tau_tail_ns = (tail_pot + R_SER_TAIL)*C_TAIL*1e9;
            
            Q_pC = calc_Q_pC(raw_amp_mv,tau_tail_ns);
            real_amp_mv = max_amplitude(Q_pC,tau_rise_ns,tau_tail_ns);
            
            last_rise_pot = rise_pot;
            last_tail_pot = tail_pot;
            last_rise_adc = rise_adc;
            last_tail_adc = tail_adc;
            
            last_att_pot = att_pot;
            
            
            display_status(0);
            scan_interval = scan_interval_fast;
            update_count_down = update_count_down_reset_val;
        } else {
            if (update_count_down == 1) {
                
                display_status(1);
                update_count_down = 0;
                scan_interval = scan_interval_slow;
                
            } else {
                if (update_count_down == update_count_down_redraw_val) {
                    display_status(1);
                }
                
                if (update_count_down > 1) {
                    update_count_down --;
                }
            }
        }
        // for the capacitors to recover
        delay(50);
        
        
        
        meta_loop_cnt = (meta_loop_cnt+1)%meta_loop_overflow;
        if (meta_loop_cnt == 0){
            // update battery reading 
            display_status(0);
        }
    } 
    
    // display buttons
    if(0){ 
        String dummy ="";
        int buttons = decode_adc_buttons();           
        for (int j = 3; j>=0; j--)
            dummy += String((buttons&(1<<j))>>j);          
        
        
        tft_debug_print( 20,200,2,dummy);
        
        tft_debug_print( 200,200,2,String(read_att_pot()));
    }
    
    
    
}
