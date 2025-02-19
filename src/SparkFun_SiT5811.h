/*
    SparkFun SiT5811 OCXO Arduino Library

    Repository
    https://github.com/sparkfun/SparkFun_SiT5811_OCXO_Arduino_Library

    SPDX-License-Identifier: MIT

    Copyright (c) 2024 SparkFun Electronics

    Name: SparkFun_SiT5811.h

    Description:
    An Arduino Library for the SiT5811 Digitally-Controlled
    Oven-Compensated Crystal Oscillator from SiTime.
    Requires the SparkFun Toolkit:
    https://github.com/sparkfun/SparkFun_Toolkit

    Notes:
    The SiT5811 has three 16-bit registers which define the 39-bit frequency control word.
    It also has four read-only 16-bit registers which define:
        DCXO Clip - the maximum pull range (13-bit)
        Power Indicator (PID output) (18-bit)
        Chip ID (16-bit)
    The frequency is changed via the frequency control word.
    The frequency control word does not set the frequency directly. Instead it defines how
    far the base frequency is to be pulled as a fraction of 800ppm.
    The DCXO Clip defines the maximum pull range (pull range limit), but does not change the
    fractional frequency resolution. That is fixed at 5.0x10^-14 per LSB.

*/

#pragma once

#include <stdint.h>

#include <Arduino.h>
// .. some header order issue right now...
// clang-format off
#include <SparkFun_Toolkit.h>
#include "sfTk/sfDevSiT5811.h"
// clang-format on

class SfeSiT5811ArdI2C : public sfDevSiT5811
{
  public:
    SfeSiT5811ArdI2C()
    {
    }

    // /// @brief  Sets up Arduino I2C driver using the default I2C address then calls the super class begin.
    // /// @return True if successful, false otherwise.
    // bool begin(void)
    // {
    //     if (_theI2CBus.init(kDefaultSiT5811Addr) != kSTkErrOk)
    //         return false;

    //     return beginDevice();
    // }

    /// @brief  Sets up Arduino I2C driver using the specified I2C address then calls the super class begin.
    /// @return True if successful, false otherwise.
    bool begin(const uint8_t address = kDefaultSiT5811Addr)
    {
        if (_theI2CBus.init(address) != ksfTkErrOk)
            return false;

        return beginDevice();
    }

    /// @brief  Sets up Arduino I2C driver using the specified I2C address then calls the super class begin.
    /// @return True if successful, false otherwise.
    bool begin(TwoWire &wirePort, const uint8_t address = kDefaultSiT5811Addr)
    {
        if (_theI2CBus.init(wirePort, address) != ksfTkErrOk)
            return false;

        return beginDevice();
    }

  private:
    bool beginDevice(void)
    {

        // the intent is that the bus is setup and we can see if the device is connected
        if (_theI2CBus.ping() != ksfTkErrOk)
            return false;

        _theI2CBus.setStop(false); // Use restarts not stops for I2C reads

        return sfDevSiT5811::begin(&_theI2CBus) == ksfTkErrOk;
    }

    sfTkArdI2C _theI2CBus;
};
