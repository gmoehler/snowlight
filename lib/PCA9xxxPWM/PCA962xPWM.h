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

/** PCA9626 PWM control LED driver
 *
 *  An operation sample of PCA9626 24-channel Fm+ I2C-bus 100mA/40 V LED driver.
 *  mbed accesses the PCA9626 registers through I2C.
 *
 *  @class   PCA9626
 *  @author  Akifumi (Tedd) OKANO, NXP Semiconductors
 *  @version 0.6
 *  @date    04-Mar-2015
 *
 *  Released under the Apache 2 license License
 *
 *  About PCA9626:
 *    http://www.nxp.com/products/lighting_driver_and_controller_ics/i2c_led_display_control/series/PCA9626.html
 */

#ifndef _PCA962x_PWM_H_
#define _PCA962x_PWM_H_

#include "PCA9xxxPWM.h"

class PCA962xPWM : public PCA9xxxPWM {
   public:
#if DOXIGEN_ONLY
    /** PCA962x pin names high-level API i.e. LedPwmOut */
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
        L16,         /**< LED16 pin                              */
        L17,         /**< LED17 pin                              */
        L18,         /**< LED18 pin                              */
        L19,         /**< LED19 pin                              */
        L20,         /**< LED20 pin                              */
        L21,         /**< LED21 pin                              */
        L22,         /**< LED22 pin                              */
        L23,         /**< LED23 pin                              */
        L_NC = ~0x0L /**< for when the pin is left no-connection */
    } LedPinName;
#endif  // DOXIGEN_ONLY

    PCA962xPWM(uint8_t i2cAddr, TwoWire *i2cPort);
    virtual ~PCA962xPWM();

    boolean begin();

    /** Broadcast reset sequence to the I2C bus.
     *
     * @param i2cPort Arduino Wire object (default: Wire)
     * @note Sending this will reset all connected PCA962x and
     * other devices that understand the reset sequence.
     */
    static void reset(TwoWire *i2cPort);
    void reset(void);

    virtual uint8_t number_of_ports(void) = 0;
    virtual String type_name(void) = 0;

    static boolean isMyDevice(uint8_t i2cAddr, TwoWire *i2cPort = &Wire);

   protected:
    enum {
        AUTO_INCREMENT = 0x80,
        PWMALL = 0xFF,
        ADR_RESET = 0x06,
        ADR_ALLCALL = 0xE0,
    };
};

#endif
