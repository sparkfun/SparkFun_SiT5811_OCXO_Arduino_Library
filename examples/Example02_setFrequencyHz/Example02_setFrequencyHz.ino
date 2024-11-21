/*
  Set the frequency of the SiT5811 OCXO.

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

  myOCXO.setFrequencyHz(10000010.0); // Set the frequency to 10.000010MHz (+1ppm)

  Serial.print("Frequency set to ");
  Serial.print(myOCXO.getFrequencyHz());
  Serial.println(" Hz");

  // Frequency control word should be 1ppm / 800ppm * (2^38 - 1) , rounded down
  Serial.print("Frequency control word should be 343597383. It is ");
  Serial.println(myOCXO.getFrequencyControlWord());
}

void loop()
{
  // Nothing to do here
}
