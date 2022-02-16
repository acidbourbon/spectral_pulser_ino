

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
    
    set_TX_LED(1);
    set_RX_LED(1);
    set_USER_LED(1);
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

uint8_t last_buttons = 0x00;

const int update_count_down_reset_val  = 20;
const int update_count_down_redraw_val = 15;

int update_count_down = 0;

int loop_cnt = 0;
int meta_loop_cnt = 0;
int meta_loop_overflow = 10;

const int scan_interval_fast = 50;
const int scan_interval_slow = 500;
int scan_interval = scan_interval_fast;





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
        
        
        // has rise or tail pot turned?
        if( (abs(rise_adc - last_rise_adc) > 2) or (abs(tail_adc - last_tail_adc) > 2) ){
            meta_loop_cnt = 0;
            tau_rise_ns = (rise_pot + R_SER_RISE)*C_RISE*1e9;
            tau_tail_ns = (tail_pot + R_SER_TAIL)*C_TAIL*1e9;
            
            Q_pC = calc_Q_pC(raw_amp_mv,tau_tail_ns);
            real_amp_mv = max_amplitude(Q_pC,tau_rise_ns,tau_tail_ns);
            
            last_rise_pot = rise_pot;
            last_tail_pot = tail_pot;
            last_rise_adc = rise_adc;
            last_tail_adc = tail_adc;
            
            
            
            display_status(1); // update text and graph, but don't clear plot area
            scan_interval = scan_interval_fast;
            update_count_down = update_count_down_reset_val;
        
        // has attenuator poti turned?
        } else if (abs(att_pot-last_att_pot)>2){
            Q_pC = calc_Q_pC(raw_amp_mv,tau_tail_ns);
            real_amp_mv = max_amplitude(Q_pC,tau_rise_ns,tau_tail_ns);
            last_att_pot = att_pot;
            display_status(10); // just update amplitude
            scan_interval = scan_interval_fast;
            update_count_down = update_count_down_reset_val;
            
        // nothing has changed, maybe do an update after a while
        } else {
            if (update_count_down == 1) {
                
                //display_status(2);
                update_count_down = 0;
                scan_interval = scan_interval_slow;
                
            } else {
                if (update_count_down == update_count_down_redraw_val) {
                    display_status(2);
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
    
    
    uint8_t buttons = decode_adc_buttons();  
    uint8_t buttons_pressed = ~(last_buttons) & buttons; //detect rising edge
    last_buttons = buttons;
    
    if(buttons_pressed & (1<<3)){
      toggle_TX_LED();  
    }
    if(buttons_pressed & (1<<2)){
      toggle_RX_LED();  
    }
    if(buttons_pressed & (1<<1)){
      toggle_USER_LED();  
    }
    
//     set_USER_LED( buttons & (1<<3) );
//     set_TX_LED( buttons & (1<<2) );
//     set_RX_LED( buttons & (1<<1) );
    
    
    
    
}
