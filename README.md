# Main-Project


# Project Ironman

This project focoses on developing a small prototype of an isokinetic dynamometer specifically designed for a single finger joint to facilitate the collection of relevant data and 
validate the practical functionality of the design. The idea of our device is to measure muscle strength under a constant angular velocity or extension. The core goal of this
prototype is to strip down the functionality of the original, full-scale clinical machine to just a few essential components. This makes the device significantly cheaper and easier
to implement in various smaller clinics or hospitals. Our intended user is primarily physiotherapists and researchers who require the data for the rehabilitation of 
patients with joint related injuries. Our project aims to widen the access to this type of machine.

# Components
- Steppermoter
- Oled-Screen: 0.96 tum, I20
- Amplifyer: HX711
- Loadcell
- Driver: A4988
- Arduino microcontroller: r4 wifi
- Micro SD card
- Magnetic Encoder: AS5600

# Technical Approach



# User Interaction

The user interacts with the prototype through a simple and intuitive setup. The user places their finger on the load cell axis, where the mechanical system simulates controlled
finger extension. 
The interaction is primarily physical, as the user experiences a resistive motion that mimics the operation of a professional isokinetic dynamometer. During this process, the data
from the Arduino microcontroller is transmitted to the software and visualized through a C-based interface and provides real-time readings of the angle and applied force during 
the test. The user experience is designed to be straightforward, allowing for easy setup and clear feedback without the complexity of a full clinical device

# Designs 

<img width="432" height="365" alt="Screenshot 2025-10-14 at 16 40 22" src="https://github.com/user-attachments/assets/31dae4d6-c804-4b0d-a5cf-5ecdf489f2e1" />
This is a geardesign to combine the motor to the loadcell



