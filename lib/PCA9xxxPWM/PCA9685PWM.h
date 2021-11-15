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

#ifndef _PCA9685_PWM_H_
#define _PCA9685_PWM_H_

#include "PCA9xxxPWM.h"

class PCA9685PWM : public PCA9xxxPWM {
public:

  /** PCA9685 pin names high-level API i.e. */
  typedef enum {
    L0,          /**< LED0 pin                               */
    L1,          /**< LED1 pin                               */
    L2,          /**< LED2 pin                               */
    L3,          /**< LED3 pin                               */
    L4,          /**< LED4 pin                               */
    L5,          /**< LED5 pin                               */
    L6,          /**< LED6 pin                               */
    L7,          /**< LED7 pin                               */
    L8,          /**< LED8 pin                               */
    L9,          /**< LED9 pin                               */
    L10,         /**< LED10 pin                              */
    L11,         /**< LED11 pin                              */
    L12,         /**< LED12 pin                              */
    L13,         /**< LED13 pin                              */
    L14,         /**< LED14 pin                              */
    L15,         /**< LED15 pin                              */
    L_NC = ~0x0L /**< for when the pin is left no-connection */
  } LedPinName;

  enum command_reg {
    MODE1,      /**< MODE1 register      */
    MODE2,      /**< MODE2 register      */
    SUBADR1,    /**< SUBADR1 register    */
    SUBADR2,    /**< SUBADR2 register    */
    SUBADR3,    /**< SUBADR3 register    */
    ALLCALLADR, /**< ALLCALLADR register */
    LED0_ON_L,  /**< LED0 output brightness cntl byte 0 */
    LED0_ON_H,  /**< LED0 output brightness cntl byte 1 */
    LED0_OFF_L, /**< LED0 output brightness cntl byte 2 */
    LED0_OFF_H, /**< LED0 output brightness cntl byte 3 */
    LED1_ON_L,  /**< LED1 output brightness cntl byte 0 */
    LED1_ON_H,  /**< LED1 output brightness cntl byte 1 */
    LED1_OFF_L, /**< LED1 output brightness cntl byte 2 */
    LED1_OFF_H, /**< LED1 output brightness cntl byte 3 */
    LED2_ON_L,  /**< LED2 output brightness cntl byte 0 */
    LED2_ON_H,  /**< LED2 output brightness cntl byte 1 */
    LED2_OFF_L, /**< LED2 output brightness cntl byte 2 */
    LED2_OFF_H, /**< LED2 output brightness cntl byte 3 */
    LED3_ON_L,  /**< LED3 output brightness cntl byte 0 */
    LED3_ON_H,  /**< LED3 output brightness cntl byte 1 */
    LED3_OFF_L, /**< LED3 output brightness cntl byte 2 */
    LED3_OFF_H, /**< LED3 output brightness cntl byte 3 */
    LED4_ON_L,  /**< LED4 output brightness cntl byte 0 */
    LED4_ON_H,  /**< LED4 output brightness cntl byte 1 */
    LED4_OFF_L, /**< LED4 output brightness cntl byte 2 */
    LED4_OFF_H, /**< LED4 output brightness cntl byte 3 */
    LED5_ON_L,  /**< LED5 output brightness cntl byte 0 */
    LED5_ON_H,  /**< LED5 output brightness cntl byte 1 */
    LED5_OFF_L, /**< LED5 output brightness cntl byte 2 */
    LED5_OFF_H, /**< LED5 output brightness cntl byte 3 */
    LED6_ON_L,  /**< LED6 output brightness cntl byte 0 */
    LED6_ON_H,  /**< LED6 output brightness cntl byte 1 */
    LED6_OFF_L, /**< LED6 output brightness cntl byte 2 */
    LED6_OFF_H, /**< LED6 output brightness cntl byte 3 */
    LED7_ON_L,  /**< LED7 output brightness cntl byte 0 */
    LED7_ON_H,  /**< LED7 output brightness cntl byte 1 */
    LED7_OFF_L, /**< LED7 output brightness cntl byte 2 */
    LED7_OFF_H, /**< LED7 output brightness cntl byte 3 */
    LED8_ON_L,  /**< LED8 output brightness cntl byte 0 */
    LED8_ON_H,  /**< LED8 output brightness cntl byte 1 */
    LED8_OFF_L, /**< LED8 output brightness cntl byte 2 */
    LED8_OFF_H, /**< LED8 output brightness cntl byte 3 */
    LED9_ON_L,  /**< LED9 output brightness cntl byte 0 */
    LED9_ON_H,  /**< LED9 output brightness cntl byte 1 */
    LED9_OFF_L, /**< LED9 output brightness cntl byte 2 */
    LED9_OFF_H, /**< LED9 output brightness cntl byte 3 */
    LED10_ON_L, /**< LED10 output brightness cntl byte 0 */
    LED10_ON_H, /**< LED10 output brightness cntl byte 1 */
    LED10_OFF_L,/**< LED10 output brightness cntl byte 2 */
    LED10_OFF_H,/**< LED10 output brightness cntl byte 3 */
    LED11_ON_L, /**< LED11 output brightness cntl byte 0 */
    LED11_ON_H, /**< LED11 output brightness cntl byte 1 */
    LED11_OFF_L,/**< LED11 output brightness cntl byte 2 */
    LED11_OFF_H,/**< LED11 output brightness cntl byte 3 */
    LED12_ON_L, /**< LED12 output brightness cntl byte 0 */
    LED12_ON_H, /**< LED12 output brightness cntl byte 1 */
    LED12_OFF_L,/**< LED12 output brightness cntl byte 2 */
    LED12_OFF_H,/**< LED12 output brightness cntl byte 3 */
    LED13_ON_L, /**< LED13 output brightness cntl byte 0 */
    LED13_ON_H, /**< LED13 output brightness cntl byte 1 */
    LED13_OFF_L,/**< LED13 output brightness cntl byte 2 */
    LED13_OFF_H,/**< LED13 output brightness cntl byte 3 */
    LED14_ON_L, /**< LED14 output brightness cntl byte 0 */
    LED14_ON_H, /**< LED14 output brightness cntl byte 1 */
    LED14_OFF_L,/**< LED14 output brightness cntl byte 2 */
    LED14_OFF_H,/**< LED14 output brightness cntl byte 3 */
    LED15_ON_L, /**< LED15 output brightness cntl byte 0 */
    LED15_ON_H, /**< LED15 output brightness cntl byte 1 */
    LED15_OFF_L,/**< LED15 output brightness cntl byte 2 */
    LED15_OFF_H,/**< LED15 output brightness cntl byte 3 */

    ALL_LED_ON_L = 0xFA, /**< Load all LED ON registers, byte 0 */
    ALL_LED_ON_H,        /**< Load all LED ON registers, byte 1 */
    ALL_LED_OFF_L,       /**< Load all LED OFF registers, byte 0 */
    ALL_LED_OFF_H,       /**< Load all LED OFF registers, byte 1 */
    PRE_SCALE,  /**< Prescaler for PWM output frequency */

    REGISTER_START = MODE1,
    LED_REGISTER_START = LED0_ON_L,
  };

  PCA9685PWM(uint8_t i2cAddr, TwoWire *i2cPort = &Wire);
  virtual ~PCA9685PWM();

  boolean begin();

  void pwm(uint8_t port, float v);
  void pwm(float *vp);

  void freq(float freq, float ext_clock = 0.0);
  float calc_freq(float ext_clock = 0.0);

  void totem_pole_outputs(boolean totem_pole);
  void invert_outputs(boolean invert);

  /** Broadcast reset sequence to the I2C bus.
   *
   * @param i2cPort Arduino Wire object (default: Wire)
   * @note Sending this will reset all connected PCA962x and
   * other devices that understand the reset sequence.
   */
  static void reset(TwoWire *i2cPort);
  void reset(void);

  uint8_t number_of_ports(void);
  String type_name(void);

  static boolean isMyDevice(uint8_t i2cAddr, TwoWire *i2cPort = &Wire);

  enum {
    MODE1_RESTART = 0x80,
    MODE1_EXTCLK = 0x40,
    MODE1_AI = 0x20,
    MODE1_SLEEP = 0x10,
    MODE1_SUB1 = 0x08,
    MODE1_SUB2 = 0x04,
    MODE1_SUB3 = 0x02,
    MODE1_ALLCALL = 0x01,
    MODE2_INVRT = 0x10,
    MODE2_OCH = 0x08,
    MODE2_OUTDRV = 0x04,
    AUTO_INCREMENT = MODE1_AI,
    PWMALL = 0xFF,
    ADR_RESET = 0x00,
    ADR_ALLCALL = 0xE0,
    ADR_SUB1 = 0xE2,
    ADR_SUB2 = 0xE4,
    ADR_SUB3 = 0xE8,
  };

private:
  void init(void);
  uint8_t pwm_register_access(uint8_t port);

  const uint8_t n_of_ports;
};

#endif
