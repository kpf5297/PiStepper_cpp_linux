/**
 * @file PiStepperDriver.cpp
 * @brief Driver application for demonstrating the usage of the PiStepper class.
 *
 * This driver application provides a command-line interface to control a stepper motor
 * using the PiStepper class. It allows the user to set motor parameters, move the motor,
 * and home the motor interactively.
 *
 * Author: Kevin Fox
 * Date: May 20, 2024
 * Location: Pittsburgh, PA
 *
 * Compilation:
 * This file should be compiled with the PiStepper class implementation and linked
 * against the libgpiod library. Example GCC command:
 * g++ -o PiStepperDriver PiStepperDriver.cpp PiStepper.cpp -lgpiod
 *
 * Run:
 * ./PiStepperDriver
 *
 * This program is dependent on the libgpiod library for GPIO manipulation. Ensure
 * libgpiod and its development headers are installed on your system.
 */

#include "PiStepper.h"
#include <iostream>
#include <string>

int main() {
    std::cout << "PiStepper Driver Application" << std::endl;

    try {
        // Initialize the stepper motor with specified GPIO pins
        PiStepper stepper(27, 17, 22, 200, 8);

        // Set initial motor parameters
        stepper.setSpeed(60); // 60 RPM
        stepper.setAcceleration(100); // 100 RPM/s

        std::cout << "Setup complete. Enter 'm' to move, 'h' to home the motor, 'q' to quit." << std::endl;

        char command;
        bool running = true;

        while (running) {
            std::cout << "Enter command: ";
            std::cin >> command;

            switch (command) {
                case 'm': {
                    int steps, direction;
                    std::cout << "Enter number of steps: ";
                    std::cin >> steps;
                    std::cout << "Enter direction (0 for backward, 1 for forward): ";
                    std::cin >> direction;

                    stepper.moveSteps(steps, direction);
                    std::cout << "Moved " << steps << " steps in direction " << direction << "." << std::endl;
                    std::cout << "Current step count: " << stepper.getCurrentStepCount() << std::endl;
                    break;
                }
                case 'h':
                    std::cout << "Homing motor..." << std::endl;
                    stepper.homeMotor();
                    std::cout << "Motor homed. Current step count: " << stepper.getCurrentStepCount() << std::endl;
                    break;
                case 'q':
                    running = false;
                    break;
                default:
                    std::cout << "Invalid command. Use 'm' to move, 'h' to home, 'q' to quit." << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    std::cout << "Exiting PiStepper Driver Application." << std::endl;
    return 0;
}
