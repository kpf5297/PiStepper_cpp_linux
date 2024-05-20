/*
    PiStepper.h

    Provides an interface for controlling a stepper motor using a Raspberry Pi and the libgpiod library.
    This class allows for setting motor speed, acceleration, microstepping, and supports operations like
    moving a specific number of steps or to a specific angle. It also includes homing functionality using
    limit switches, emergency stop, and non-blocking move operations.

    Author: Kevin Fox
    Date: May 20, 2024
    Location: Pittsburgh, PA
*/

#ifndef PiStepper_h
#define PiStepper_h

#include <gpiod.h>
#include <iostream>
#include <mutex>
#include <thread>
#include <functional>

// GPIO pin definitions for limit switches
#define LIMIT_SWITCH_BOTTOM_PIN 21
#define LIMIT_SWITCH_TOP_PIN 20

// Define the full range of steps for your stepper motor
#define FULL_COUNT_RANGE 1700

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
    void setSpeed(float speed); // Set the speed of the stepper motor in RPM
    void setAcceleration(float acceleration); // Set the acceleration of the stepper motor in RPM/s
    void setMicrostepping(int microstepping); // Set the microstepping value for the stepper motor

    // Stepper control
    void enable(); // Enable the stepper motor
    void disable(); // Disable the stepper motor
    void moveSteps(int steps, int direction); // Move the stepper motor a specified number of steps in a specified direction
    void moveAngle(float angle, int direction); // Move the stepper motor a specified angle in a specified direction
    
    // Homing
    void homeMotor(); // Move the motor towards a limit switch to define a home position

    // Position tracking
    int getCurrentStepCount() const; // Get the current step count relative to the starting position

    // Asynchronous operation
    void moveStepsAsync(int steps, int direction, std::function<void()> callback = nullptr); // Move steps asynchronously
    void stopMovement(); // Stop any ongoing movement

    // Safety
    void emergencyStop(); // Immediately stop the motor and disable it

    // Calibration
    void calibrate(); // Calibrate the motor by moving to both limit switches

    void moveStepsOverDuration(int steps, int durationSeconds); // Move the stepper motor a specified number of steps over a specified duration

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
    bool _isMoving; // Tracks if the motor is currently moving

    // GPIO chip and line pointers
    gpiod_chip *chip;
    gpiod_line *step_signal;
    gpiod_line *dir_signal;
    gpiod_line *enable_signal;
    gpiod_line *limit_switch_bottom;
    gpiod_line *limit_switch_top;

    // Private methods
    float stepsToAngle(int steps); // Convert steps to angle
    void internalMoveSteps(int steps, int direction); // Internal method to handle moving steps
    std::mutex gpioMutex; // Mutex for thread-safe GPIO access
};

#endif // PiStepper_h
