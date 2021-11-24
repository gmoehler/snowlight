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
  Part of this source code uses
  https://os.mbed.com/users/nxp_ip/code/PCA962x/
  Original header is below. Note it's Apache 2 License.
*/

/** PCA9622 PWM control LED driver
 *
 *  An operation sample of PCA9622 16-channel Fm+ I2C-bus 100mA/40 V LED driver.
 *  mbed accesses the PCA9622 registers through I2C.
 *
 *  @class   PCA9622 (based on PCA9626)
 *  @author  Akifumi (Tedd) OKANO, NXP Semiconductors
 *  @version 0.6
 *  @date    04-Mar-2015
 *
 *  Released under the Apache 2 license License
 *
 *  About PCA9622:
 *    http://www.nxp.com/products/lighting_driver_and_controller_ics/i2c_led_display_control/series/PCA9622.html
 */

#ifndef _PCA9622_PWM_H_
#define _PCA9622_PWM_H_

#include "PCA962xPWM.h"

class PCA9622PWM : public PCA962xPWM {
   public:
    enum command_reg {
        MODE1,      /**< MODE1 register      */
        MODE2,      /**< MODE2 register      */
        PWM0,       /**< PWM0 register       */
        PWM1,       /**< PWM1 register       */
        PWM2,       /**< PWM2 register       */
        PWM3,       /**< PWM3 register       */
        PWM4,       /**< PWM4 register       */
        PWM5,       /**< PWM5 register       */
        PWM6,       /**< PWM6 register       */
        PWM7,       /**< PWM7 register       */
        PWM8,       /**< PWM8 register       */
        PWM9,       /**< PWM9 register       */
        PWM10,      /**< PWM10 register      */
        PWM11,      /**< PWM11 register      */
        PWM12,      /**< PWM12 register      */
        PWM13,      /**< PWM13 register      */
        PWM14,      /**< PWM14 register      */
        PWM15,      /**< PWM15 register      */
        GRPPWM,     /**< GRPPWM register     */
        GRPFREQ,    /**< GRPFREQ register    */
        LEDOUT0,    /**< LEDOUT0 register    */
        LEDOUT1,    /**< LEDOUT1 register    */
        LEDOUT2,    /**< LEDOUT2 register    */
        LEDOUT3,    /**< LEDOUT3 register    */
        SUBADR1,    /**< SUBADR1 register    */
        SUBADR2,    /**< SUBADR2 register    */
        SUBADR3,    /**< SUBADR3 register    */
        ALLCALLADR, /**< ALLCALLADR register */

        REGISTER_START = MODE1,
        LEDOUT_REGISTER_START = LEDOUT0,
        PWM_REGISTER_START = PWM0,
    };

    PCA9622PWM(uint8_t i2cAddr, TwoWire *i2cPort = &Wire);
    virtual ~PCA9622PWM();

    String type_name(void);

    uint8_t number_of_ports(void);

    static boolean isMyDevice(uint8_t i2cAddr, TwoWire *i2cPort = &Wire);

   private:
    void init(void);
    uint8_t pwm_register_access(uint8_t port);
    static boolean _isMyDevice(uint8_t i2cAddr, TwoWire *i2cPort = &Wire);

    const uint8_t n_of_ports;
};

#endif
