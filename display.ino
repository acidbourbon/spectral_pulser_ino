






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

const float XSCALE = 1.0/XTICS_PX * XTICS; 
const float YSCALE = 1.0/YTICS_PX * YTICS;

const float PULSE_DELAY = 10;




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
