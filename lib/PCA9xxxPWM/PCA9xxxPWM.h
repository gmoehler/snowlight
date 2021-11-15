/*
  The MIT License (MIT)
  Copyright (c) Kiyo Chinzei (kchinzei@gmail.com)
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/
/*
  PCA9xxxPWM - Arduino / C++ class for NXP PCA9xxx I2C connected PWM devices.

  Make Asayake to Wake Project.
  Kiyo Chinzei
  https://github.com/kchinzei/PCA9xxxPWM
*/

#ifndef _PCA9XXX_PWM_H_
#define _PCA9XXX_PWM_H_

#include <Arduino.h>
#include <Wire.h>

#define ALLPORTS 0xFF

class PCA9xxxPWM {
public:
  /** Constructor of this class
   *
   * @param i2cAddr In 7 bit (last bit 0/1 for write/read).
   * @param i2cPort Arduino Wire object
   * @note This class a pure virtual class.
   * @note Wire uses 7bit addressing. https://www.arduino.cc/en/Reference/Wire
   */
  PCA9xxxPWM(uint8_t i2c_addr, TwoWire *i2cPort);
  virtual ~PCA9xxxPWM();

  /** Establish I2C connection with specified address, then calling init().
   *
   * @returns True if connection success.
   */
  boolean begin();

  /** Examine this I2C device connected and responds.
   *
   * @returns True if responded.
   * @note    It doesn't examine if connected device belongs to this class.
   */
  boolean isConnected();

  /** Examine an I2C device connected at specified address.
   *
   * @param i2cAddr In 7 bit (last bit 0/1 for write/read).
   * @param i2cPort Arduino Wire object (default: Wire)
   * @returns       True if some device responds.
   */
  static boolean isConnected(uint8_t i2cAddr, TwoWire *i2cPort = &Wire);

  /** Examine if device at specified address belongs to this class.
   *
   * @param i2cAddr I2C-bus address
   * @param i2cPort Arduino Wire object (default: Wire)
   * @returns       True if it a device connected and belongs to this class.
   * @note          A derived class must implements it and called from this
   * class.
   */
  static boolean isMyDevice(uint8_t i2cAddr, TwoWire *i2cPort = &Wire);

  /** Obtain device type name of this device.
   *
   * @returns Type name such as "PCA9626".
   */
  virtual String type_name(void) = 0;

  /** Broadcast reset sequences to the I2C bus.
   *
   * @param i2cPort Arduino Wire object (default: Wire)
   * @note Sending this will reset all connected devices that
   * understand the reset sequences.
   */
  static void reset(TwoWire *i2cPort = &Wire);

  /** Set the output PWM ratio, specified as a percentage (float).
   *
   * @param port  Selecting output port
   *    'ALLPORTS' can be used to set all port duty-cycle same value.
   * @param v     A floating-point value representing the output current,
   *    specified as a percentage. The value should lie between
   *    0.0f (representing on 0%) and 1.0f (representing on 100%).
   *    Values outside this range will have undefined behavior.
   */
  virtual void pwm(uint8_t port, float v);

  /** Set all output port PWM ratio, specified as a percentage (array of float).
   *
   * @param vp    Array to floating-point values representing the output PWM
   * ratio, specified as a percentage. The value should lie between 0.0f
   * (representing on 0%) and 1.0f (representing on 100%).
   *
   *  @note
   *    The length of array should be at least number_of_ports().
   */
  virtual void pwm(float *vp);

  /** Number of PWM ports of this device.
   *
   */
  virtual uint8_t number_of_ports(void) = 0;

  /** Getter for _i2cAddr
   *
   * @returns I2C address currently assigned.
   */
  uint8_t get_i2cAddr(void) { return _i2cAddr; };

  /** Getter for _i2cPort
   *
   * @returns I2C port currently assigned.
   */
  TwoWire *get_i2cPort(void) { return _i2cPort; };

  /** Getter for use_exponential
   *
   * @preturns use_exponential
   */
  boolean get_use_exponential(void) { return use_exponential; };

  /** Apply exponential scale between 0 - 255.
   * PCA9955A has h/w function.
   *
   * @param exp_on on/off exponential scale (default: false)
   */
  virtual void exponential_adjustment(boolean exp_on);

  /** High level write methods to this device's register.
   *
   * @param reg_addr Register to write
   * @param data     One byte to write
   * @param val      Data array to write
   * @param len      Length of val
   * @returns        False when fails.
   */
  boolean write(uint8_t reg_addr, uint8_t data);
  boolean write(uint8_t *val, uint8_t len);

  /** High level read method from this device's register.
   *
   * @param regAddr  Register to read from
   * @returns        False when fails.
   */
  uint8_t read(uint8_t regAddr);

  /** Low level read/write methods.
   *
   * @param i2cAddr In 7 bit (last bit 0/1 for write/read).
   * @param i2cPort Arduino Wire object
   * @param val     Data array to write
   * @param len     Length of val
   * @param regAddr Register to read from
   * @returns       False/0 when fails.
   */
  static boolean write(uint8_t i2cAddr, TwoWire *i2cPort, uint8_t *val,
                       uint8_t len);
  static uint8_t read(uint8_t i2cAddr, TwoWire *i2cPort, uint8_t reg_addr);

  /** Name of the commonly used PCA9xxx registers (for direct register access)
   */
  enum command_reg {
    MODE1 = 0x00, /**< MODE1 register      */
    MODE2,        /**< MODE2 register      */
  };

protected:
  virtual float simple_exp(float refIn);
  uint8_t _i2cAddr;
  TwoWire *_i2cPort;
  boolean use_exponential;
  enum {
    AUTO_INCREMENT = 0x80,
  };

private:
  virtual void init() = 0;
  virtual uint8_t pwm_register_access(uint8_t port) = 0;
};

#endif
