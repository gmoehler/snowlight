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

/*
  This software also works for PCA995xB. See
  https://os.mbed.com/components/PCA9955A-PCA9956A-16-24-channel-constant/

  Part of this source code uses
  https://os.mbed.com/users/nxp_ip/code/PCA995xA/
  Original header is below. Note it's Apache 2 License.
*/

/** PCA9955A constant current LED driver
 *
 *  An operation sample of PCA9955A 16-channel Fm+ I2C-bus 57mA/20V constant
 * current LED driver. mbed accesses PCA9955A registers via I2C.
 *
 *  @class   PCA9955A
 *  @author  Akifumi (Tedd) OKANO, NXP Semiconductors
 *  @version 0.6
 *  @date    19-Mar-2015
 *
 *  Released under the Apache 2 license
 *
 *  About PCA9955A:
 *    http://www.nxp.com/products/interface_and_connectivity/i2c/i2c_led_display_control/PCA9955ATW.html
 */

#ifndef _PCA9955A_PWM_H_
#define _PCA9955A_PWM_H_

#include "PCA995xAPWM.h"

class PCA9955APWM : public PCA995xAPWM {
public:
  /** Name of the PCA9955A registers (for direct register access) */
  enum command_reg {
    MODE1 = 0x00,          /**< MODE1 register      */
    MODE2,                 /**< MODE2 register      */
    LEDOUT0,               /**< LEDOUT0 register    */
    LEDOUT1,               /**< LEDOUT1 register    */
    LEDOUT2,               /**< LEDOUT2 register    */
    LEDOUT3,               /**< LEDOUT3 register    */
    GRPPWM = 0x06,         /**< GRPPWM register     */
    GRPFREQ,               /**< GRPFREQ register    */
    PWM0,                  /**< PWM0 register       */
    PWM1,                  /**< PWM1 register       */
    PWM2,                  /**< PWM2 register       */
    PWM3,                  /**< PWM3 register       */
    PWM4,                  /**< PWM4 register       */
    PWM5,                  /**< PWM5 register       */
    PWM6,                  /**< PWM6 register       */
    PWM7,                  /**< PWM7 register       */
    PWM8,                  /**< PWM8 register       */
    PWM9,                  /**< PWM9 register       */
    PWM10,                 /**< PWM10 register      */
    PWM11,                 /**< PWM11 register      */
    PWM12,                 /**< PWM12 register      */
    PWM13,                 /**< PWM13 register      */
    PWM14,                 /**< PWM14 register      */
    PWM15,                 /**< PWM15 register      */
    IREF0,                 /**< IREF0 register      */
    IREF1,                 /**< IREF1 register      */
    IREF2,                 /**< IREF2 register      */
    IREF3,                 /**< IREF3 register      */
    IREF4,                 /**< IREF4 register      */
    IREF5,                 /**< IREF5 register      */
    IREF6,                 /**< IREF6 register      */
    IREF7,                 /**< IREF7 register      */
    IREF8,                 /**< IREF8 register      */
    IREF9,                 /**< IREF9 register      */
    IREF10,                /**< IREF10 register     */
    IREF11,                /**< IREF11 register     */
    IREF12,                /**< IREF12 register     */
    IREF13,                /**< IREF13 register     */
    IREF14,                /**< IREF14 register     */
    IREF15,                /**< IREF15 register     */
    RAMP_RATE_GRP0,        /**< RAMP_RATE_GRP0 register */
    STEP_TIME_GRP0,        /**< STEP_TIME_GRP0 register */
    HOLD_CNTL_GRP0,        /**< HOLD_CNTL_GRP0 register */
    IREF_GRP0,             /**< IREF_GRP0 register      */
    RAMP_RATE_GRP1,        /**< RAMP_RATE_GRP1 register */
    STEP_TIME_GRP1,        /**< STEP_TIME_GRP1 register */
    HOLD_CNTL_GRP1,        /**< HOLD_CNTL_GRP1 register */
    IREF_GRP1,             /**< IREF_GRP1 register      */
    RAMP_RATE_GRP2,        /**< RAMP_RATE_GRP2 register */
    STEP_TIME_GRP2,        /**< STEP_TIME_GRP2 register */
    HOLD_CNTL_GRP2,        /**< HOLD_CNTL_GRP2 register */
    IREF_GRP2,             /**< IREF_GRP2 register      */
    RAMP_RATE_GRP3,        /**< RAMP_RATE_GRP3 register */
    STEP_TIME_GRP3,        /**< STEP_TIME_GRP3 register */
    HOLD_CNTL_GRP3,        /**< HOLD_CNTL_GRP3 register */
    IREF_GRP3,             /**< IREF_GRP3 register      */
    GRAD_MODE_SEL0 = 0x38, /**< GRAD_MODE_SEL0 register */
    GRAD_MODE_SEL1,        /**< GRAD_MODE_SEL1 register */
    GRAD_GRP_SEL0,         /**< GRAD_GRP_SEL0 register  */
    GRAD_GRP_SEL1,         /**< GRAD_GRP_SEL1 register  */
    GRAD_GRP_SEL2,         /**< GRAD_GRP_SEL2 register  */
    GRAD_GRP_SEL3,         /**< GRAD_GRP_SEL3 register  */
    GRAD_CNTL,             /**< GRAD_CNTL register      */
    OFFSET = 0x3F,         /**< OFFSET register     */
    SUBADR1,               /**< SUBADR1 register    */
    SUBADR2,               /**< SUBADR2 register    */
    SUBADR3,               /**< SUBADR3 register    */
    ALLCALLADR,            /**< ALLCALLADR register */
    PWMALL,                /**< PWMALL register     */
    IREFALL,               /**< IREFALL register    */
    EFLAG0,                /**< EFLAG0 register     */
    EFLAG1,                /**< EFLAG1 register     */
    EFLAG2,                /**< EFLAG2 register     */
    EFLAG3,                /**< EFLAG3 register     */

    REGISTER_START = MODE1,
    LEDOUT_REGISTER_START = LEDOUT0,
    PWM_REGISTER_START = PWM0,
    IREF_REGISTER_START = IREF0,
    ERR_REGISTER_START = EFLAG0,
    GRAD_GROUP_OFFSET = RAMP_RATE_GRP1 - RAMP_RATE_GRP0
  };
                   
  PCA9955APWM(uint8_t i2cAddr, TwoWire *i2cPort = &Wire);
  ~PCA9955APWM();

  String type_name(void);

  uint8_t number_of_ports(void);

  uint8_t errflag(uint8_t port);

  /** Overrides the method in the base class, because this device has its own
   * h/w function.
   *
   * @param exp_on Turn on/off exponential scale.
   */
  void exponential_adjustment(boolean exp_on);

  static boolean isMyDevice(uint8_t i2cAddr, TwoWire *i2cPort = &Wire);

  enum {
    MODE1_AIF = 0x80,
    MODE1_AI1 = 0x40,
    MODE1_AI0 = 0x20,
    MODE1_SLEEP = 0x10,
    MODE1_SUB1 = 0x08,
    MODE1_SUB2 = 0x04,
    MODE1_SUB3 = 0x02,
    MODE1_ALLCALL = 0x01,
    MODE2_OVERTEMP = 0x80,
    MODE2_ERROR = 0x40,
    MODE2_DMBLNK = 0x20,
    MODE2_CLRERR = 0x10,
    MODE2_OCH = 0x08,
    MODE2_EXP_EN = 0x04,
  };

private:
  void init(void);
  uint8_t pwm_register_access(uint8_t port);
  uint8_t current_register_access(uint8_t port);
  float simple_exp(float refIn);
  static boolean _isMyDevice(uint8_t i2cAddr, TwoWire *i2cPort = &Wire);

  const uint8_t n_of_ports;
};

#endif
