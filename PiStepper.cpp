/*
    PiStepper.cpp - Library for controlling a stepper motor using a Raspberry Pi.
    Created by Kevin Fox
    20 May 2024

    Updated to include the same speed value used for opening and closing the valve in the home_motor 
    function. It now uses the last used _speed value in its calculation for step delay.
*/

#include "PiStepper.h"
#include <cmath>
#include <unistd.h>

PiStepper::PiStepper(int stepPin, int dirPin, int enablePin, int stepsPerRevolution, int microstepping)
    : _stepPin(stepPin), _dirPin(dirPin), _enablePin(enablePin),
      _stepsPerRevolution(stepsPerRevolution), _microstepping(microstepping),
      _speed(20), _acceleration(80), _currentStepCount(0), _isMoving(false) {
    // Initialize the GPIO chip and lines
    chip = gpiod_chip_open("/dev/gpiochip0");
    step_signal = gpiod_chip_get_line(chip, _stepPin);
    dir_signal = gpiod_chip_get_line(chip, _dirPin);
    enable_signal = gpiod_chip_get_line(chip, _enablePin);
    limit_switch_top = gpiod_chip_get_line(chip, LIMIT_SWITCH_TOP_PIN);
    limit_switch_bottom = gpiod_chip_get_line(chip, LIMIT_SWITCH_BOTTOM_PIN);

    // Configure GPIO pins
    gpiod_line_request_output(step_signal, "PiStepper_step", 0);
    gpiod_line_request_output(dir_signal, "PiStepper_dir", 0);
    gpiod_line_request_output(enable_signal, "PiStepper_enable", 1);
    gpiod_line_request_input(limit_switch_bottom, "PiStepper_limit_bottom");
    gpiod_line_request_input(limit_switch_top, "PiStepper_limit_top");

    disable(); // Start with the motor disabled
}

PiStepper::~PiStepper() {
    // Release GPIO lines and chip
    gpiod_line_release(step_signal);
    gpiod_line_release(dir_signal);
    gpiod_line_release(enable_signal);
    gpiod_line_release(limit_switch_top);
    gpiod_line_release(limit_switch_bottom);
    gpiod_chip_close(chip);
}

void PiStepper::setSpeed(float speed) {
    _speed = speed;
}

void PiStepper::setAcceleration(float acceleration) {
    _acceleration = acceleration;
}

void PiStepper::enable() {
    gpiod_line_set_value(enable_signal, 1); 
}

void PiStepper::disable() {
    gpiod_line_set_value(enable_signal, 0); 
}

void PiStepper::moveSteps(int steps, int direction) {
    std::lock_guard<std::mutex> lock(gpioMutex);
    _isMoving = true;
    internalMoveSteps(steps, direction);
    _isMoving = false;
}

void PiStepper::moveAngle(float angle, int direction) {
    int steps = std::round(angle * ((_stepsPerRevolution * _microstepping) / 360.0f));
    moveSteps(steps, direction);
    std::cout << "Calculated steps: " << steps << std::endl;
}

void PiStepper::homeMotor() {
    enable();
    const int direction = 0; // For closing the valve
    gpiod_line_set_value(dir_signal, direction);

    float stepDelay = 60.0 * 1000000 / (_speed * _stepsPerRevolution * _microstepping); // delay in microseconds

    // Move the motor towards the bottom limit switch
    while (gpiod_line_get_value(limit_switch_bottom) == 1) { // Assumes active high when triggered
        gpiod_line_set_value(step_signal, 1);
        usleep(stepDelay / 2); // Half delay for pulse high
        gpiod_line_set_value(step_signal, 0);
        usleep(stepDelay / 2); // Half delay for pulse low
    }
    _currentStepCount = 0; // Reset the step counter at the home position
    disable();
}

void PiStepper::setMicrostepping(int microstepping) {
    _microstepping = microstepping;
}

int PiStepper::getCurrentStepCount() const {
    return _currentStepCount;
}

float PiStepper::stepsToAngle(int steps) {
    return (static_cast<float>(steps) / (_stepsPerRevolution * _microstepping)) * 360.0f;
}

void PiStepper::moveStepsOverDuration(int steps, int durationSeconds) {
    float stepsPerSecond = steps / static_cast<float>(durationSeconds);
    float rpm = stepsPerSecond * 60 / (_stepsPerRevolution * _microstepping);

    setSpeed(rpm); // Set the calculated RPM
    moveSteps(steps, 1); // Move the motor
}

void PiStepper::internalMoveSteps(int steps, int direction) {
    enable();
    gpiod_line_set_value(dir_signal, direction);

    float stepDelay = 60.0 * 1000000 / (_speed * _stepsPerRevolution * _microstepping); // delay in microseconds

    for (int i = 0; i < steps; i++) {
        if (gpiod_line_get_value(limit_switch_top) == 0 && direction == 1) {
            std::cout << "Top limit switch triggered" << std::endl;
            break;
        }

        if (gpiod_line_get_value(limit_switch_bottom) == 0 && direction == 0) {
            std::cout << "Bottom limit switch triggered" << std::endl;
            break;
        }

        gpiod_line_set_value(step_signal, 1);
        usleep(stepDelay / 2); // Half delay for pulse high
        gpiod_line_set_value(step_signal, 0);
        usleep(stepDelay / 2); // Half delay for pulse low

        if (direction == 0) {
            _currentStepCount--;
        } else {
            _currentStepCount++;
        }
    }
    disable();
}

void PiStepper::moveStepsAsync(int steps, int direction, std::function<void()> callback) {
    std::thread([this, steps, direction, callback]() {
        moveSteps(steps, direction);
        if (callback) {
            callback();
        }
    }).detach();
}

void PiStepper::stopMovement() {
    std::lock_guard<std::mutex> lock(gpioMutex);
    _isMoving = false;
    disable();
}

void PiStepper::emergencyStop() {
    std::lock_guard<std::mutex> lock(gpioMutex);
    _isMoving = false;
    disable();
    _currentStepCount = 0; // Optionally reset step count
    std::cout << "Emergency Stop Activated!" << std::endl;
}

void PiStepper::calibrate() {
    // Example calibration routine
    homeMotor();
    // Move to top limit switch and determine the full range
    moveSteps(FULL_COUNT_RANGE, 1); // Move to the top limit switch
    std::cout << "Calibration complete. Full range: " << _currentStepCount << " steps." << std::endl;
}
