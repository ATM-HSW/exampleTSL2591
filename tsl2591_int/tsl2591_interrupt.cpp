/* TSL2591 Digital Light Sensor, example with (simple) interrupt support  */
/* Dynamic Range: 600M:1 */
/* Maximum Lux: 88K */

/*  This example shows how the interrupt system on the TLS2591
 *  can be used to detect a meaningful change in light levels.
 *  
 *  Two thresholds can be set: 
 *  
 *  Lower Threshold - Any light sample on CHAN0 below this value
 *                    will trigger an interrupt
 *  Upper Threshold - Any light sample on CHAN0 above this value
 *                    will trigger an interrupt
 *                    
 *  If CHAN0 (full light) crosses below the low threshold specified,
 *  or above the higher threshold, an interrupt is asserted on the interrupt
 *  pin. The use of the HW pin is optional, though, since the change can
 *  also be detected in software by looking at the status byte via
 *  tsl.getStatus().
 *  
 *  An optional third parameter can be used in the .registerInterrupt
 *  function to indicate the number of samples that must stay outside
 *  the threshold window before the interrupt fires, providing some basic
 *  debouncing of light level data.
 *  
 *  For example, the following code will fire an interrupt on any and every
 *  sample outside the window threshold (meaning a sample below 100 or above
 *  1500 on CHAN0 or FULL light):
 *  
 *    tsl.registerInterrupt(100, 1500, TSL2591_PERSIST_ANY);
 *  
 *  This code would require five consecutive changes before the interrupt
 *  fires though (see tls2591Persist_t in Adafruit_TLS2591.h for possible
 *  values):
 *  
 *    tsl.registerInterrupt(100, 1500, TSL2591_PERSIST_5);
 */

#include "mbed.h"
#include "Adafruit_TSL2591.h"

// Example for demonstrating the TSL2591 library - public domain!

// connect SCL to I2C Clock
// connect SDA to I2C Data
// connect Vin to 3.3-5V DC
// connect GROUND to common ground
I2C i2c(I2C_SDA , I2C_SCL );

// Interrupt thresholds and persistance
#define TLS2591_INT_THRESHOLD_LOWER  (100)
#define TLS2591_INT_THRESHOLD_UPPER  (1500)
//#define TLS2591_INT_PERSIST        (TSL2591_PERSIST_ANY) // Fire on any valid change
#define TLS2591_INT_PERSIST          (TSL2591_PERSIST_60)  // Require at least 60 samples to fire

Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)

/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2591
*/
/**************************************************************************/
void configureSensor(void) {
  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
  //tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
  tsl.setGain(TSL2591_GAIN_MED);      // 25x gain
  // tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain

  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)

  /* Display the gain and integration time for reference sake */
  printf("------------------------------------\n");
  printf("Gain:         ");
  tsl2591Gain_t gain = tsl.getGain();
  switch(gain) {
    case TSL2591_GAIN_LOW:
      printf("1x (Low)\n");
      break;
    case TSL2591_GAIN_MED:
      printf("25x (Medium)\n");
      break;
    case TSL2591_GAIN_HIGH:
      printf("428x (High)\n");
      break;
    case TSL2591_GAIN_MAX:
      printf("9876x (Max)\n");
      break;
  }
  printf("------------------------------------\n");

  /* Setup the SW interrupt to trigger between 100 and 1500 lux */
  /* Threshold values are defined at the top of this sketch */
  tsl.clearInterrupt();
  tsl.registerInterrupt(TLS2591_INT_THRESHOLD_LOWER,
                        TLS2591_INT_THRESHOLD_UPPER,
                        TLS2591_INT_PERSIST);

  /* Display the interrupt threshold window */
  printf("Interrupt Threshold Window: %d to %d\n", TLS2591_INT_THRESHOLD_LOWER, TLS2591_INT_THRESHOLD_UPPER);
}

/**************************************************************************/
/*
    Show how to read IR and Full Spectrum at once and convert to lux
*/
/**************************************************************************/
void advancedRead(void) {
  // More advanced data read example. Read 32 bits with top 16 bits IR, bottom 16 bits full spectrum
  // That way you can do whatever math and comparisons you want!
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  printf("IR: %d  Full: %d  Visible: %d  Lux: %f\n", ir, full, full-ir, tsl.calculateLux(full, ir));
}


void getStatus(void) {
  uint8_t x = tsl.getStatus();
  // bit 4: ALS Interrupt occured
  // bit 5: No-persist Interrupt occurence
  if (x & 0x10) {
    printf("ALS Interrupt occured\n");
  }
  if (x & 0x20) {
    printf("No-persist Interrupt occured\n");
  }

  printf("Status: %x\n", x);
  tsl.clearInterrupt();
}

/**************************************************************************/
/*
    Program entry point
*/
/**************************************************************************/
int main() {

  printf("Starting Adafruit TSL2591 interrupt Test!\n");

  if(!tsl.begin(i2c)) {
    printf("No sensor found ... check your wiring?\n");
    while (1);
  }

  uint8_t id = tsl.getID();
  printf("------------------------------------\n");
  printf("ID: %x\n", id);
  printf("------------------------------------\n");

  /* Configure the sensor */
  configureSensor();

  // Now we're ready to get readings ... move on to loop()!
  while(true) { 
    advancedRead();
    getStatus();
    thread_sleep_for(500);
  }
}
