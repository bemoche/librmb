
#pragma once

#include <memory>

#include <units/angle.h>
#include <units/angular_velocity.h>
#include <units/length.h>
#include <units/math.h>

namespace rmb {

class LinearVelocityController;

/**
 * Interface for controlling a mechanism's angular velocity used by wrappers of
 * device specific APIs.
 */
class AngularVelocityController {
public:
  //***************
  // Motor Control
  //***************

  /**
   * Common interface for setting the target angular velocity.
   *
   * @param velocity The target angular velocity in radians per second.
   */
  virtual void setVelocity(units::radians_per_second_t velocity) = 0;

  /**
   * Common interface for getting the <b>target</b> angular velocity.
   *
   * @return The <b>target</b> angular velocity in radians per second.
   */
  virtual units::radians_per_second_t getTargetVelocity() const = 0;

  /**
   * Common interface for setting a mechanism's raw power output.
   */
  virtual void setPower(double power) = 0;

  /**
   * Common interface for disabling a mechanism.
   */
  virtual void disable() = 0;

  /**
   * Common interface to stop the mechanism until `setPosition` is called again.
   */
  virtual void stop() = 0;

  //**********
  // Feedback
  //**********

  /**
   * Common interface for returning the angular velocity of an encoder.
   *
   * @return The velocity of the encoder in radians per second.
   */
  virtual units::radians_per_second_t getVelocity() const = 0;

  /**
   * Common interface for returning the angular position of an encoder.
   *
   * @return The position of the encoder in radians.
   */
  virtual units::radian_t getPosition() const = 0;

  /**
   * Common interface for zeroing the anguklar positon an encoder so the current
   * position is set to the offset.
   *
   * @param offset the offset from the current angular position at which to
   *               set the zero position.
   */
  virtual void zeroPosition(units::radian_t offset = 0_rad) = 0;

  /**
   * Common interface for getting a controllers tolerance
   */
  virtual units::radians_per_second_t getTolerance() const = 0;

  /**
   * Common interface for getting the error between the velocities controllers
   * target velocity and the actual velocity measured by the encoder.
   *
   * @return position error in radians per second.
   */
  virtual units::radians_per_second_t getError() const {
    return getVelocity() - getTargetVelocity();
  }

  /**
   * Common interface for getting whether the mechanism has achived it's
   * target velocity.
   *
   * @return true is the controller has achived the target velocity.
   */
  virtual bool atTarget() const {
    return units::math::abs(getError()) < getTolerance();
  }

  //*************
  // Conversions
  //*************

  using ConversionUnit =
      units::compound_unit<units::meters, units::inverse<units::radians>>;
  using ConversionUnit_t = units::unit_t<ConversionUnit>;
};

/**
 * Generates a `linearVelocityController` from a `AngularVelocityController`
 * via a proportional conversion factor. The new controller takes ownership
 * over the old one.
 *
 * @param angularController Controller in angular units form which to create a
 *                          linear controller
 * @param conversion        conversion factor from linear to angular units such
 *                          as a wheel diameter.
 */
std::unique_ptr<LinearVelocityController>
asLinear(std::unique_ptr<AngularVelocityController> angularController,
         AngularVelocityController::ConversionUnit_t conversion);
} // namespace rmb
