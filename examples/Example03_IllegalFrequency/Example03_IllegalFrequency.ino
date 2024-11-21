/*
  Set the frequency of the SiT5811 OCXO.

  This example demonstrates what happens when an illegal (out of bounds) frequency is selected.

  By: Paul Clark
  SparkFun Electronics
  Date: 2024/11/21
  SparkFun code, firmware, and software is released under the MIT License.
  Please see LICENSE.md for further details.

*/

// You will need the SparkFun Toolkit. Click here to get it: http://librarymanager/All#SparkFun_Toolkit

#include <SparkFun_SiT5811.h> // Click here to get the library: http://librarymanager/All#SparkFun_SiT5811

SfeSiT5811ArdI2C myOCXO;

void setup()
{
  delay(1000); // Allow time for the microcontroller to start up

  Serial.begin(115200); // Begin the Serial console
  while (!Serial)
  {
    delay(100); // Wait for the user to open the Serial Monitor
  }
  Serial.println("SparkFun SiT5811 Example");

  Wire.begin(); // Begin the I2C bus

  if (!myOCXO.begin())
  {
    Serial.println("SiT5811 not detected! Please check the address and try again...");
    while (1); // Do nothing more
  }

  myOCXO.setBaseFrequencyHz(10000000.0); // Pass the oscillator base frequency into the driver

  Serial.print("Base frequency set to ");
  Serial.print(myOCXO.getBaseFrequencyHz());
  Serial.println(" Hz");

  // Read the available (clipped) pull range
  double pullAvailable = myOCXO.getMaxPullAvailable();
  Serial.printf("Maximum frequency pull is: %e\r\n", pullAvailable);

  double lowestFrequency = 10e6 - (10e6 * pullAvailable);
  Serial.print("The lowest frequency available is ");
  Serial.print(lowestFrequency);
  Serial.println(" Hz");

  myOCXO.setFrequencyHz(9900000.0); // Try to set the frequency to 9.9MHz (-10000ppm)

  Serial.print("Frequency set to ");
  Serial.print(myOCXO.getFrequencyHz());
  Serial.println(" Hz");

  double fractionalPull = pullAvailable / 800e-6; // Fractional pull compared to 800ppm
  fractionalPull *= pow(2, 38); // Convert to expected control word
  int64_t expectedControlWord = 0 - fractionalPull;

  Serial.print("Frequency control word should be ");
  Serial.print(expectedControlWord);
  Serial.print(". It is ");
  Serial.println(myOCXO.getFrequencyControlWord());
}

void loop()
{
  // Nothing to do here
}
