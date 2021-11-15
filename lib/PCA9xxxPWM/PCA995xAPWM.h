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

#ifndef _PCA995xA_PWM_H_
#define _PCA995xA_PWM_H_

#include "PCA9xxxPWM.h"

class PCA995xAPWM : public PCA9xxxPWM {
public:
#if DOXYGEN_ONLY
  /** PCA995xA pin names high-level API i.e. LedPwmOutCC */
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
#endif // DOXYGEN_ONLY

  enum error_flags {
    NO_ERROR = 0,
    SHORT_CIRCUIT,
    OPEN_CIRCUIT,
    CIRCUIT_ERROR = SHORT_CIRCUIT | OPEN_CIRCUIT,
    OVERTEMP = 0x10,
  };

  /** Constructor of this class
   *
   * @note This class is a pure virtual class. To instatiate use PCA9955APMW class
   * etc.
   */
  PCA995xAPWM(uint8_t i2cAddr, TwoWire *i2cPort);
  virtual ~PCA995xAPWM();

  boolean begin();

  /** Broadcast reset sequence to the I2C bus.
   *
   * @param i2cPort Arduino Wire object (default: Wire)
   * @note Sending this will reset all connected PCA995xA and
   * other devices that understand the reset sequence.
   */
  static void reset(TwoWire *i2cPort);
  void reset(void);
  
  /** Set the output current, specified as a percentage (float)
   *
   * @param port  Selecting output port
   *    'ALLPORTS' can be used to set all port duty-cycle same value.
   * @param v     A floating-point value representing the output current,
   *    specified as a percentage. The value should lie between
   *    0.0f (representing on 0%) and 1.0f (representing on 100%).
   *    Values outside this range will have undefined behavior.
   */
  void current(uint8_t port, float vp);

  /** Set output ports' current, specified as a percentage (array of float)
   *
   * @param vP    Array to floating-point values representing the output
   * current, specified as a percentage. The value should lie between 0.0f
   * (representing on 0%) and 1.0f (representing on 100%).
   *
   *  @note
   *    The array should have length of number_of_ports()
   */
  void current(float *vP);

  /** Get error status of a port
   *
   * @param port  Selecting output port
   *    'ALLPORTS' can be used to get any error status in any port other than NO_ERROR.
   * @returns ErrStatus. OVERTEMP and other error can occur same time.
   * @note Calling errflag() clears open or short-circuit status flag (not fixing the defect).
   *  This can hide errors of other ports.
   * @note Error status is tested and the flag is set when pwm value is between 8/255 and 254/255.
   */
  virtual uint8_t errflag(uint8_t port) = 0;
  
  virtual uint8_t number_of_ports(void) = 0;
  virtual String type_name(void) = 0;

  static boolean isMyDevice(uint8_t i2cAddr, TwoWire *i2cPort = &Wire);

protected:
  enum {
    AUTO_INCREMENT = 0x80,
    ADR_RESET = 0x00,
    ADR_ALLCALL = 0x70,
    ADR_SUBADR_DEFAULT = 0x76,
  };

private:
  virtual uint8_t current_register_access(uint8_t port) = 0;

};

#endif
