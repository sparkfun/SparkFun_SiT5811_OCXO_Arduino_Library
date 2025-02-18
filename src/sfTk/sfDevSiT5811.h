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

// include the sparkfun toolkit headers
#include <sfTk/sfToolkit.h>

// Bus interfaces
#include <sfTk/sfTkII2C.h>

///////////////////////////////////////////////////////////////////////////////
// I2C Addressing
///////////////////////////////////////////////////////////////////////////////
// The SiT5811 can be ordered with a pre-programmed I2C address in the range
// 0x50 to 0x5F (unshifted). It can also be ordered with a selectable address
// of 0x50/52/58/5A via the A0/A1 Pins. Here we assume a default address of 0x50.
// The actual address can be defined via the begin method.
const uint8_t kDefaultSiT5811Addr = 0x50; //

///////////////////////////////////////////////////////////////////////////////
// 16-bit Register Addresses
///////////////////////////////////////////////////////////////////////////////

const uint8_t kSfeSiT5811RegClip = 0x00;       // DCXO Clip 13-bit
const uint8_t kSfeSiT5811RegControlMSW = 0x0C; // Digital Frequency Control Most Significant Word (MSW)
const uint8_t kSfeSiT5811RegControlNSW = 0x0D; // Digital Frequency Control Next Significant Word (NSW)
const uint8_t kSfeSiT5811RegControlLSW = 0x0E; // Digital Frequency Control Least Significant Word (LSW)

///////////////////////////////////////////////////////////////////////////////
// OCXO Clip Register Description
///////////////////////////////////////////////////////////////////////////////

// A union is used here so that individual values from the register can be
// accessed or the whole register can be accessed.
typedef union {
    struct
    {
        uint16_t clip : 13;   // DCXO_Clip[12:0]
        uint16_t notUsed : 3; // Not used
    };
    uint16_t word;
} sfe_SiT5811_reg_clip_t;

///////////////////////////////////////////////////////////////////////////////
// Digital Frequency Control Least Significant Word (LSW) Register Description
///////////////////////////////////////////////////////////////////////////////

// A union is used here so that individual values from the register can be
// accessed or the whole register can be accessed.
typedef union {
    struct
    {
        uint16_t notUsed : 9;     // Not used
        uint16_t freqControl : 7; // DCXO[6:0]
    };
    uint16_t word;
} sfe_SiT5811_reg_control_lsw_t;

///////////////////////////////////////////////////////////////////////////////

class sfDevSiT5811
{
  public:
    // @brief Constructor. Instantiate the driver object using the specified address (if desired).
    sfDevSiT5811() : _baseFrequencyHz{10000000.0}, _maxFrequencyChangePPB{800000.0}
    {
    }

    /// @brief Begin communication with the SiT5811. Read the registers.
    /// @return 0 for success, negative for errors, positive for warnings
    sfTkError_t begin(sfTkII2C *commBus = nullptr);

    /// @brief Read the SiT5811 OCXO Clip register and update the driver's internal copy
    /// @return true if the read is successful
    bool readClipRegister(void);

    /// @brief Read the three SiT5811 frequency control registers and update the driver's internal copies
    /// @return true if the read is successful
    bool readRegisters(void);

    /// @brief Get the 39-bit frequency control word - from the driver's internal copy
    /// @return The 39-bit frequency control word as int64_t (signed, two's complement)
    int64_t getFrequencyControlWord(void);

    /// @brief Set the 39-bit frequency control word - and update the driver's internal copy
    /// @param freq the frequency control word as int64_t (signed, two's complement)
    /// @return true if the write is successful
    bool setFrequencyControlWord(int64_t freq);

    /// @brief Get the 13-bit clip value - from the driver's internal copy
    /// @return The 13-bit clip as uint16_t
    uint16_t getPullRangeClip(void);

    /// @brief Get the clip value - from the driver's internal copy. Convert to maximum pull available
    /// @return The clip value converted to maximum pull available (double)
    double getMaxPullAvailable(void);

    /// @brief Get the base oscillator frequency - from the driver's internal copy
    /// @return The oscillator base frequency as double
    double getBaseFrequencyHz(void);

    /// @brief Set the base oscillator frequency in Hz - set the driver's internal _baseFrequencyHz
    /// @param freq the base frequency in Hz
    /// @return true if the write is successful
    void setBaseFrequencyHz(double freq);

    /// @brief Get the oscillator frequency based on the base frequency and control word
    /// @return The oscillator frequency as double
    double getFrequencyHz(void);

    /// @brief Set the oscillator frequency based on the base frequency and pull range limit
    /// @param freq the oscillator frequency in Hz
    /// @return true if the write is successful
    /// Note: The frequency change will be limited by the pull range capabilities of the device.
    ///       Call getFrequencyHz to read the frequency set.
    /// Note: setFrequencyHz ignores _maxFrequencyChangePPB.
    bool setFrequencyHz(double freq);

    /// @brief Get the maximum frequency change in PPB
    /// @return The maximum frequency change in PPB - from the driver's internal store
    double getMaxFrequencyChangePPB(void);

    /// @brief Set the maximum frequency change in PPB - set the driver's internal _maxFrequencyChangePPB
    /// @param ppb the maximum frequency change in PPB
    void setMaxFrequencyChangePPB(double ppb);

    /// @brief Set the frequency according to the GNSS receiver clock bias in milliseconds
    /// @param bias the GNSS RX clock bias in milliseconds
    /// @param Pk the Proportional term
    /// @param Ik the Integral term
    /// @return true if the write is successful
    /// Note: the frequency change will be limited by: the pull range capabilities of the device;
    ///       and the setMaxFrequencyChangePPB. Call getFrequencyHz to read the frequency set.
    /// The default values for Pk and Ik come from very approximate Ziegler-Nichols tuning:
    /// oscillation starts when Pk is TODO; with a period of TODO seconds.
    bool setFrequencyByBiasMillis(double bias, double Pk = 0.5, double Ik = 0.1);

  protected:
    /// @brief Sets the communication bus to the specified bus.
    /// @param theBus Bus to set as the communication devie.
    void setCommunicationBus(sfTkII2C *theBus);

  private:
    sfTkII2C *_theBus; // Pointer to bus device.

    const double _maxPullRange = 800e-6; // Maximum pull range is +/- 800ppm
    int64_t _frequencyControl;           // Local store for the frequency control word. 39-Bit, 2's complement
    uint16_t _clip;                      // Local store for the 13-bit OCXO Clip register value
    double _baseFrequencyHz;             // The base frequency used by getFrequencyHz and setFrequencyHz
    double _maxFrequencyChangePPB;       // The maximum frequency change in PPB for setFrequencyByBiasMillis
};
