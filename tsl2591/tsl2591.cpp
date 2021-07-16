/* TSL2591 Digital Light Sensor */
/* Dynamic Range: 600M:1 */
/* Maximum Lux: 88K */

#include "mbed.h"
#include "Adafruit_TSL2591.h"

// Example for demonstrating the TSL2591 library - public domain!

// connect SCL to I2C Clock
// connect SDA to I2C Data
// connect Vin to 3.3-5V DC
// connect GROUND to common ground
I2C i2c(I2C_SDA , I2C_SCL );

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
  //tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain
  
  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  //tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
  tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)

  /* Display the gain and integration time for reference sake */  
  printf("------------------------------------\n");
  printf("Gain:         ");
  tsl2591Gain_t gain = tsl.getGain();
  switch(gain)   {
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
}

/**************************************************************************/
/*
    Shows how to perform a basic read on visible, full spectrum or
    infrared light (returns raw 16-bit ADC values)
*/
/**************************************************************************/
void simpleRead(void) {
  // Simple data read example. Just read the infrared, fullspecrtrum diode 
  // or 'visible' (difference between the two) channels.
  // This can take 100-600 milliseconds! Uncomment whichever of the following you want to read
  uint16_t x = tsl.getLuminosity(TSL2591_VISIBLE);
  //uint16_t x = tsl.getLuminosity(TSL2591_FULLSPECTRUM);
  //uint16_t x = tsl.getLuminosity(TSL2591_INFRARED);

  printf("Luminosity: %d\n", x);
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


/**************************************************************************/
/*
    Program entry point for the Arduino sketch
*/
/**************************************************************************/
int main() {
  
  printf("Starting Adafruit TSL2591 Test!\n");
  
  if(!tsl.begin(i2c)) {
    printf("No sensor found ... check your wiring?\n");
    while (1);
  }
    
  /* Configure the sensor */
  configureSensor();

  // Now we're ready to get readings ... move on to loop()!
  while(true) { 
    //simpleRead(); 
    advancedRead();
    thread_sleep_for(500);
  }
}
