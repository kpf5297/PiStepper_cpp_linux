Certainly! Below is a professional `README.md` for the `PiStepper` class that includes an overview, installation instructions, usage examples, and other relevant information.

### README.md


# PiStepper Class for Stepper Motor Control

## Overview
The `PiStepper` class provides a robust interface for controlling a stepper motor using a Raspberry Pi and the libgpiod library. It supports precise movement with configurable speed, acceleration, and microstepping, and includes features for position tracking, homing with limit switches, and thread-safe operations.

## Features
- **Precise Movement Control**: Configure speed, acceleration, and microstepping.
- **Position Tracking**: Track the current step position relative to the starting point.
- **Homing**: Move the motor towards limit switches to define home positions.
- **Thread Safety**: Ensure safe operations in multi-threaded applications.
- **Error Handling**: Robust error handling with clear exception messages.

## Dependencies
This class requires the libgpiod library. Follow these steps to install the necessary dependencies on a Linux system:

```bash
sudo apt-get update
sudo apt-get install gpiod libgpiod-dev
```

## Installation
To use the `PiStepper` class in your projects, clone this repository to your local machine:

```bash
git clone https://github.com/kpf5297/PiStepper_cpp_linux.git
```

Navigate to the repository directory:

```bash
cd PiStepper
```

Include the `PiStepper.h` and `PiStepper.cpp` files in your project.

## Usage
To initialize and use the `PiStepper` class, include the header in your project and create an instance of the class:

```cpp
#include "PiStepper.h"

// Create a PiStepper object with specified GPIO pins
PiStepper stepper(27, 17, 22, 200, 8);

// Set speed and acceleration
stepper.setSpeed(60);  // 60 RPM
stepper.setAcceleration(100);  // 100 RPM/s

// Move the motor
stepper.moveSteps(400, 1);  // Move 400 steps in the specified direction

// Home the motor
stepper.homeMotor();
```

### Example Driver
Below is an example driver application to demonstrate the usage of the `PiStepper` class:

```cpp
#include "PiStepper.h"
#include <iostream>

int main() {
    try {
        PiStepper stepper(27, 17, 22);

        stepper.setSpeed(60); // 60 RPM
        stepper.setAcceleration(100); // 100 RPM/s

        std::cout << "Moving motor 200 steps forward..." << std::endl;
        stepper.moveSteps(200, 1);

        std::cout << "Current step count: " << stepper.getCurrentStepCount() << std::endl;

        std::cout << "Homing motor..." << std::endl;
        stepper.homeMotor();

        std::cout << "Motor homed. Current step count: " << stepper.getCurrentStepCount() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
```

### Compilation
To compile the example driver, ensure that the `PiStepper.h` and `PiStepper.cpp` files are included, and link against the libgpiod library:

```bash
g++ -o PiStepperDriver PiStepperDriver.cpp PiStepper.cpp -lgpiod
```

Run the driver:

```bash
./PiStepperDriver
```