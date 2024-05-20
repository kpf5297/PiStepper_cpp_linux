/*
    PiStepper.h

    Provides an interface for controlling a stepper motor using a Raspberry Pi and the libgpiod library.
    This class allows for setting motor speed, acceleration, microstepping, and supports operations like
    moving a specific number of steps or to a specific angle. It also includes homing functionality using
    limit switches.

    Author: Kevin Fox
    Date: May 20, 2024
    Location: Pittsburgh, PA
*/

#ifndef PiStepper_h
#define PiStepper_h

#include <gpiod.h>
#include <iostream>
#include <mutex>

// GPIO pin definitions for limit switches
#define LIMIT_SWITCH_BOTTOM_PIN 21
#define LIMIT_SWITCH_TOP_PIN 20

class PiStepper {
public:
    /**
     * Constructor for PiStepper.
     * Initializes the stepper motor control with specified GPIO pins and settings.
     *
     * @param stepPin The GPIO pin number for the step signal.
     * @param dirPin The GPIO pin number for the direction signal.
     * @param enablePin The GPIO pin number for the enable signal.
     * @param stepsPerRevolution The number of steps per full revolution of the motor (default is 200).
     * @param microstepping The microstepping value (default is 8).
     */
    PiStepper(int stepPin, int dirPin, int enablePin, int stepsPerRevolution = 200, int microstepping = 8);
    
    /**
     * Destructor for PiStepper.
     * Releases GPIO resources.
     */
    ~PiStepper();

    // Setters
    /**
     * Sets the speed of the stepper motor in RPM.
     *
     * @param speed The speed in RPM.
     */
    void setSpeed(float speed);

    /**
     * Sets the acceleration of the stepper motor in RPM/s.
     *
     * @param acceleration The acceleration in RPM/s.
     */
    void setAcceleration(float acceleration);

    /**
     * Sets the microstepping value for the stepper motor.
     *
     * @param microstepping The microstepping value.
     */
    void setMicrostepping(int microstepping);

    // Stepper control
    /**
     * Enables the stepper motor.
     */
    void enable();

    /**
     * Disables the stepper motor.
     */
    void disable();

    /**
     * Moves the stepper motor a specified number of steps in a specified direction.
     *
     * @param steps The number of steps to move.
     * @param direction The direction to move (0 or 1).
     */
    void moveSteps(int steps, int direction);

    /**
     * Moves the stepper motor a specified angle in a specified direction.
     *
     * @param angle The angle in degrees to move.
     * @param direction The direction to move (0 or 1).
     */
    void moveAngle(float angle, int direction);

    // Homing
    /**
     * Moves the motor towards a limit switch to define a home position.
     */
    void homeMotor();

    // Position tracking
    /**
     * Gets the current step count relative to the starting position.
     *
     * @return The current step count.
     */
    int getCurrentStepCount() const;

    /**
     * Moves the stepper motor a specified number of steps over a specified duration.
     *
     * @param steps The number of steps to move.
     * @param durationSeconds The duration in seconds over which to move the steps.
     */
    void moveStepsOverDuration(int steps, int durationSeconds);

private:
    // GPIO pin assignments
    int _stepPin;
    int _dirPin;
    int _enablePin;
    int _stepsPerRevolution;
    int _microstepping;
    float _speed;
    float _acceleration;
    int _currentStepCount; // Tracks the current step position relative to the starting point

    // GPIO chip and line pointers
    gpiod_chip *chip;
    gpiod_line *step_signal;
    gpiod_line *dir_signal;
    gpiod_line *enable_signal;
    gpiod_line *limit_switch_bottom;
    gpiod_line *limit_switch_top;

    // Private methods
    /**
     * Converts a number of steps to an angle.
     *
     * @param steps The number of steps.
     * @return The corresponding angle in degrees.
     */
    float stepsToAngle(int steps);

    std::mutex gpioMutex; // Mutex for thread-safe GPIO access
};

#endif // PiStepper_h
