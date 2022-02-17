#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

inline void clear_screen() __attribute__((always_inline));

inline int meas_tail_adc() __attribute__((always_inline));
inline int meas_rise_adc() __attribute__((always_inline));
inline int calc_tail_pot(int adc_val) __attribute__((always_inline));
inline int calc_rise_pot(int adc_val) __attribute__((always_inline));
inline int read_att_pot(void) __attribute__((always_inline));
  
inline void set_attenuator_dB(float dB) __attribute__((always_inline));
inline void set_max_attenuation() __attribute__((always_inline));
inline void attenuator_mode_subroutine() __attribute__((always_inline));
inline void pulse_mode_subroutine() __attribute__((always_inline));



inline float peaking_time(float tau1, float tau2)             __attribute__((always_inline));
inline float max_amplitude(float q, float tau1, float tau2)   __attribute__((always_inline));
inline float calc_Q_pC(float raw_amplitude_mV, float tau2_ns) __attribute__((always_inline)); 


inline float pulse_func(const float x, const float q, const float tau_rise, const float tau_tail, const float tdelay)  __attribute__((always_inline));


#define GLCD_CL_BLACK 0x0000
#define GLCD_CL_WHITE 0xFFFF
#define GLCD_CL_GRAY 0x7BEF
#define GLCD_CL_LIGHT_GRAY 0xC618
#define GLCD_CL_GREEN 0x07E0
#define GLCD_CL_LIME 0x87E0
#define GLCD_CL_BLUE 0x001F
#define GLCD_CL_RED 0xF800
#define GLCD_CL_AQUA 0x5D1C
#define GLCD_CL_YELLOW 0xFFE0
#define GLCD_CL_MAGENTA 0xF81F
#define GLCD_CL_CYAN 0x07FF
#define GLCD_CL_DARK_CYAN 0x03EF
#define GLCD_CL_ORANGE 0xFCA0
#define GLCD_CL_PINK 0xF97F
#define GLCD_CL_BROWN 0x8200
#define GLCD_CL_VIOLET 0x9199
#define GLCD_CL_SILVER 0xA510
#define GLCD_CL_GOLD 0xA508
#define GLCD_CL_NAVY 0x000F
#define GLCD_CL_MAROON 0x7800
#define GLCD_CL_PURPLE 0x780F
#define GLCD_CL_OLIVE 0x7BE0

#define DARK_GREY_BLUE 0x0105

#define BUTTON_A (1<<3)
#define BUTTON_B (1<<2)
#define BUTTON_C (1<<1)
#define BUTTON_D (1<<0)

// Arduino Pro Micro 3.3V
// == LilyPad Arduino USB



#define TFT_CS   12
#define TFT_DC   8
#define TFT_RST  11

// Hardware SPI on the Lilipad/Leonardo clone:
// MISO -> D14
// SCK  -> D15
// MOSI -> D16

#define ADC_RISE 0
#define ADC_TAIL 1
#define DAC_ADC
#define VBAT2 3
#define ADC_BUTTONS 4
#define ADC_POT 5

#define GPIO_RISE_A 2
#define GPIO_RISE_B 3
#define GPIO_TAIL 4
#define ATT_CLK 5
#define ATT_SDI 6
#define ATT_LE 7
#define DAC_CHARGE 9
#define PULSE_TRIGGER 10
#define F_CS 13
#define SD_CS 1

#define USER_LED 13
#define RX_LED   17
//TX_LED PD5


// empirical constants based on calib data
#define C_RISE 12.9e-12
#define C_TAIL 114.7e-12
#define R_SER_TAIL 26
#define R_SER_RISE 70.6

#define AMPLITUDE_CALIB_FACTOR 1.1
