





const int DISPLAY_WIDTH = 320;
const int DISPLAY_HEIGHT = 240;

const int PLOT_POS_X = 35;
const int PLOT_POS_Y = 20;
const int PLOT_HEIGHT = 100;
const int PLOT_WIDTH = 260;

const int PLOT_BACKGND_COL = ILI9341_BLACK;
// const int PLOT_GRID_COL    = GLCD_CL_DARK_CYAN;
const int PLOT_GRID_COL    = DARK_GREY_BLUE;



const int XTICS_PX = 24;
const int YTICS_PX = 18;

const float XTICS = 10;
const float YTICS = 20;

const float XSCALE = 1.0/XTICS_PX * XTICS; // you multiply #pixels by XSCALE to go to the real number x
const float YSCALE = 1.0/YTICS_PX * YTICS; // you divide f(x) by YSCALE to go to vertical pixels on screen

const float PULSE_DELAY = 10;



void init_tft_pins(){
  // Pins for TFT
  pinMode(TFT_RST,OUTPUT);
  digitalWrite(TFT_RST,0);
  delay(10);
  digitalWrite(TFT_RST,1);
  delay(50);
}




void clear_screen() {
  tft.fillScreen(ILI9341_BLACK);
}


void draw_footer(
    String a,
    String b,
    String c,
    String d  ){
  
  const int footer_line_height = 12;
  const int footer_text_height = 10;
    tft.drawLine(0,DISPLAY_HEIGHT-footer_line_height,
                 DISPLAY_WIDTH,DISPLAY_HEIGHT-footer_line_height,
                 PLOT_GRID_COL);
    
  tft.fillRect(0,DISPLAY_HEIGHT-footer_text_height,
               DISPLAY_WIDTH,   footer_text_height,
               PLOT_BACKGND_COL);
  
  tft_debug_print(DISPLAY_WIDTH/4*0,
                  DISPLAY_HEIGHT-footer_text_height,1,
                  "A: "+a);
  tft_debug_print(DISPLAY_WIDTH/4*1,
                  DISPLAY_HEIGHT-footer_text_height,1,
                  "B: "+b);
  tft_debug_print(DISPLAY_WIDTH/4*2,
                  DISPLAY_HEIGHT-footer_text_height,1,
                  "C: "+c);
  tft_debug_print(DISPLAY_WIDTH/4*3,
                  DISPLAY_HEIGHT-footer_text_height,1,
                  "D: "+d);
    
}


void add_pk_ampl_to_plot(){
  
  const int yoffset = PLOT_POS_Y+PLOT_HEIGHT;
  const int xoffset = PLOT_POS_X;
  const int width   = PLOT_WIDTH;
 
  
  
  
  tft.setCursor(
    xoffset + (PULSE_DELAY+peaking_time(tau_rise_ns,tau_tail_ns))/XSCALE -5, // divide by XSCALE to go to pixels
//     yoffset - ( 100.*real_amp_mv/raw_amp_mv)/YSCALE  // divide by YSCALE to go to pixels
    yoffset - 10 -   max_amplitude(abs(tau_rise_ns-tau_fall_ns)*100., tau_rise_ns,tau_fall_ns)/YSCALE; // -10 because of text height
  );
  tft.setTextColor(ILI9341_RED);
  tft.setTextSize(1);
  tft.print(String(real_amp_mv)+" mV");
}

void pulse_preview(float tau_rise, float tau_fall,int clear){
    //float tau_rise = 0.03;
    //float tau_fall = 20;
    //const int normalize = 0;
    
    float q = abs(tau_rise-tau_fall)*100;// if tau rise super small, then abs ampl is 70
    //float tdelay = 20;
    
    //if(normalize){
    //  float max_amp = max_amplitude(q,tau_rise,tau_fall);
    //  // scale to 100 %
    //  q = q*100./max_amp;
    //}
    
    if (clear){
      prepare_plot_area();
    }
    
    plot_pulse(q, tau_rise, tau_fall, PULSE_DELAY,ILI9341_RED);
}
    
    
void prepare_plot_area(){
      clear_plot_area();
      plot_grid(XTICS_PX,YTICS_PX);
      plot_axis_numbers(XTICS_PX,YTICS_PX);
      tft_debug_print(288,125,1, "ns");
      tft.setRotation(2);
      tft_debug_print(155,4,1, "a.u.");
      tft.setRotation(3);
  
}


void demo_plot(void) {

    tft.fillScreen(ILI9341_BLACK);
  
  
    float tau_rise = 0.03;
    float tau_fall = 20;
    float q = abs(tau_rise-tau_fall)*100;// if tau rise super small, then abs ampl is 70
    //float tdelay = 20;
    
    clear_plot_area();
    plot_grid(XTICS_PX,YTICS_PX);
    plot_axis_numbers(XTICS_PX,YTICS_PX);
    
    plot_pulse(q, tau_rise, tau_fall, PULSE_DELAY,ILI9341_RED);
    plot_pulse(q, 2.5, tau_fall, PULSE_DELAY,ILI9341_YELLOW);
    plot_pulse(q, 5, tau_fall, PULSE_DELAY,ILI9341_GREEN);
    plot_pulse(q, 10, tau_fall, PULSE_DELAY,ILI9341_BLUE);
    
    //delay(2000);

}

float peaking_time(float tau1, float tau2){
  return log(tau1/tau2) / (1./tau2 - 1./tau1);
}

float max_amplitude(float q, float tau1, float tau2){
  return 50.*q/tau1 * pow(tau1/tau2,-tau2/(tau1-tau2));
}

float calc_Q_pC(float raw_amplitude_mv, float tau2_ns){
//   return raw_amplitude_mv/1000. * tau2_ns*1e-9 /50. *1e12;
  // the powers of ten cancel out
  return raw_amplitude_mv * tau2_ns /50.;
}


float pulse_func(const float x, const float q, const float tau_rise, const float tau_fall, const float tdelay){
  float x_ = x-tdelay;
  return (x_>0) * q/(tau_rise - tau_fall)*(exp(-x_/tau_rise) - exp(-x_/tau_fall));
}

void clear_plot_area(){
  // don't fill whole screen, just overwrite the plot area
  tft.fillRect(PLOT_POS_X,
               PLOT_POS_Y,
               PLOT_WIDTH,
               PLOT_HEIGHT,
               PLOT_BACKGND_COL);
  
}

void plot_grid(int xtics_px, int ytics_px){
  
  const int yoffset = PLOT_POS_Y+PLOT_HEIGHT;
  const int xoffset = PLOT_POS_X;
  
  for ( int i = 0; i <= PLOT_WIDTH; i += xtics_px){
    tft.drawLine(xoffset+i,yoffset,xoffset+i,yoffset-PLOT_HEIGHT,PLOT_GRID_COL);
  }
  for ( int j = 0; j <= PLOT_HEIGHT; j += ytics_px){
    tft.drawLine(xoffset,yoffset-j,xoffset+PLOT_WIDTH,yoffset-j,PLOT_GRID_COL);
  }
  
}

void plot_axis_numbers(int xtics_px, int ytics_px){
  
  const int yoffset = PLOT_POS_Y+PLOT_HEIGHT;
  const int xoffset = PLOT_POS_X;
  
  for ( int i = 0; i < PLOT_WIDTH; i += xtics_px){
    tft.setCursor(xoffset+i -3,yoffset + 5);
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
    int x_number = int(float(i)*XSCALE - PULSE_DELAY);
    if (x_number >= 0)
      tft.println(x_number);
  }
  for ( int j = 0; j < PLOT_HEIGHT; j += ytics_px){
    tft.setCursor(xoffset-15,yoffset-j -3);
    tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
    int y_number = int(float(j)*YSCALE);
    if (y_number >= 0)
      tft.println(y_number);
  }
  
}

void plot_pulse(float q, float tau_rise, float tau_fall, float tdelay, int plotcolor){
  
  const int yoffset = PLOT_POS_Y+PLOT_HEIGHT;
  const int xoffset = PLOT_POS_X;
  const int width   = PLOT_WIDTH;
  
  
  
  
  //int plotcolor = ILI9341_RED;
  
  int last_x = 0;
  int last_y = 1000;
  
  for (int i = 0; i < width; i++){
    int x = i;
    int y = int(pulse_func(float(x*XSCALE),q, tau_rise, tau_fall, tdelay) * 1/YSCALE);
    
    
    
    
    if ((y < PLOT_HEIGHT) && (y >= 0))
      if ((last_y < PLOT_HEIGHT) && (last_y >= 0))
        tft.drawLine(xoffset + last_x,
                     yoffset - last_y,
                     xoffset + x,
                     yoffset - y,
                     plotcolor);
    
    last_x = x;
    last_y = y;
  }
  
  //tft.drawLine(10, 10, 60, 60, plotcolor);
  
}


unsigned long testText() {
  tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.println("Hello World! Spectral");
  tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(ILI9341_RED);    tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.println("SpectralPulser1");
//   tft.setTextSize(2);
//   tft.println("I implore thee,");
//   tft.setTextSize(1);
//   tft.println("my foonting turlingdromes.");
//   tft.println("And hooptiously drangle me");
//   tft.println("with crinkly bindlewurdles,");
//   tft.println("Or I will rend thee");
//   tft.println("in the gobberwarts");
//   tft.println("with my blurglecruncheon,");
//   tft.println("see if I don't!");
  return micros() - start;
}


void tft_debug_print(int debug_pos_x,
                     int debug_pos_y,
                     int size,
                     String text) {
    
  tft.fillRect(debug_pos_x,
               debug_pos_y,
               text.length()*6*size,
               8*size,
               ILI9341_BLACK);
               
  tft.setCursor(debug_pos_x,debug_pos_y);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(size);
  tft.print(text);
    
}



void display_status(uint8_t update_level){
    
    //update level:
    // 1 general text
    // 2 clear plot area
    // 4 plot
    
    // 8 just update pk ampl
    // 16 just update battery 
  
    // these can be added
    // if you want to update all, just do display_status(0xFF)
    
    const int report_pos_y = 150;
    const int report_pos_x = 20;
    const int col2_xoffs   = 160;
    
    
    if (update_level & 1){
    
        tft_debug_print( 180,4,1,    "RisePot (R): "+String(rise_pot) +"  " );
        tft_debug_print( 20,4,1,    "TailPot (R): "+String(tail_pot) +"  " );
        tft_debug_print( report_pos_x,report_pos_y   ,1,    "pk_time  (ns): "+String(  peaking_time(tau_rise_ns,tau_tail_ns) ) +"  " );
        tft_debug_print( report_pos_x,report_pos_y+10,1,    "pk_ampl. (mV): "+String(real_amp_mv ) +"  " );
        tft_debug_print( report_pos_x,report_pos_y+20,1,    "tau_rise (ns): "+String(tau_rise_ns ) +"  " );
        tft_debug_print( report_pos_x,report_pos_y+30,1,    "tau_tail (ns): "+String(tau_tail_ns ) +"  " );
        tft_debug_print( report_pos_x,report_pos_y+40,1,    "battery   (V): "+String(bat_V,2 ));
        
        tft_debug_print( report_pos_x+col2_xoffs,report_pos_y+0,1,    "charge (pC): "+String(Q_pC ) +"  " );
        
    } else {
        // separate update of readings
        // "the fast lane"
       
      if (update_level & 8){
        tft_debug_print( report_pos_x+15*6,report_pos_y+10,1, String(real_amp_mv ) +"  " );
      }
      if (update_level & 16){
        tft_debug_print( report_pos_x+15*6,report_pos_y+40,1, String(meas_bat(),2 ));
      }
    }
    
    if (update_level & 2){
      prepare_plot_area();
    }
    if (update_level & 4){
      pulse_preview(tau_rise_ns,tau_tail_ns,0);
    }
    
    
}

    // display buttons
//     if(0){ 
//         String dummy ="";
//         int buttons = decode_adc_buttons();           
//         for (int j = 3; j>=0; j--)
//             dummy += String((buttons&(1<<j))>>j);          
//         
//         
//         tft_debug_print( 20,200,2,dummy);
//         
//         tft_debug_print( 200,200,2,String(read_att_pot()));
//     }
