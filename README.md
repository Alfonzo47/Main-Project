# Main-Project


# Project Ironman

This project focoses on developing a small prototype of an isokinetic dynamometer specifically designed for a single finger joint to facilitate the collection of relevant data and 
validate the practical functionality of the design. The idea of our device is to measure muscle strength under a constant angular velocity or extension. The core goal of this
prototype is to strip down the functionality of the original, full-scale clinical machine to just a few essential components. This makes the device significantly cheaper and easier
to implement in various smaller clinics or hospitals. Our intended user is primarily physiotherapists and researchers who require the data for the rehabilitation of 
patients with joint related injuries. Our project aims to widen the access to this type of machine.

# Components
- Steppermoter: Nema17
- Oled-Screen: 0.96 tum, I20
- Amplifyer: HX711
- Loadcell
- Driver: A4988
- Arduino microcontroller: r4 wifi
- Micro SD card
- Magnetic Encoder: AS5600

# Technical Approach

The system's core consists of an Arduino microcontroller integrated with an AS5600 magnetic encoder. The control logic is developed using the C language, ensuring quick execution 
for real-time control and analysis.
The physical motion simulates finger flexion/extension using a specially 3D designed gear-driven system. A Load Cell is mounted on one of the gears, acting as the axis where the
user's finger applies force.

The key to achieving Isokinetic Control is:
- Encoder Measurement: The AS5600 encoder continuously measures the angular position and, critically, the velocity of the motor shaft.
- Software: This code integrates a position to track the total absolute rotation (in degrees) and a sensor to measure force. Both angle and weight values ​​are updated and displayed in real time, while the code continuously drives the motor. It also includes a logic to scale and calculate full revolutions from the magnetic sensor.  
- Data Collection: Simultaneously, the Load Cell accurately measures the applied force.

Both angle and weight values ​​are updated and displayed in real time, while the code continuously drives the motor. It also includes a logic to scale and calculate full
revolutions from the magnetic sensor.



# User Interaction

The user interacts with the prototype through a simple and intuitive setup. The user places their finger on the load cell axis, where the mechanical system simulates controlled
finger extension. 
The interaction is primarily physical, as the user experiences a resistive motion that mimics the operation of a professional isokinetic dynamometer. During this process, the data
from the Arduino microcontroller is transmitted to the software and visualized through a C-based interface and provides real-time readings of the angle and applied force during 
the test. The user experience is designed to be straightforward, allowing for easy setup and clear feedback without the complexity of a full clinical device.

# Start up the Arduino-system

https://curiousscientist.tech/blog/as5600-magnetic-position-encoder

# Designs 

<img width="432" height="387" alt="Screenshot 2025-10-14 at 16 40 22" src="https://github.com/user-attachments/assets/31dae4d6-c804-4b0d-a5cf-5ecdf489f2e1" />

<img width="432" height="387" alt="Screenshot 2025-10-14 at 16 42 54" src="https://github.com/user-attachments/assets/5bd28a4c-82df-4446-8a31-6e5548ac6913" />




The figure shows a geardesign to combine the motor to the loadcell







<img width="474" height="408" alt="Screenshot 2025-10-15 at 08 58 10" src="https://github.com/user-attachments/assets/45f6744c-144c-441d-9d65-eb200f2815b5" />

<img width="474" height="406" alt="Screenshot 2025-10-15 at 09 00 52" src="https://github.com/user-attachments/assets/385f7e22-bf46-49b4-a12b-85f68a9a61f9" />




The figure shows a piece to combine the magnetic encoder and the motor 








<img width="818" height="643" alt="Screenshot 2025-10-15 at 09 02 26" src="https://github.com/user-attachments/assets/0b2da7e0-1742-4b0b-a57b-604b6a48d17b" />

<img width="818" height="643" alt="Screenshot 2025-10-15 at 09 02 44" src="https://github.com/user-attachments/assets/fa7162a4-f618-416f-9c74-889293c1cc2a" />




The figure shows the intended design for the prototype. This model offers the user to rest the palm while doing the test. The cylindrical shape in the design represents one of the gears







<img width="801" height="558" alt="Screenshot 2025-10-15 at 14 39 43" src="https://github.com/user-attachments/assets/a615b686-281e-4fea-8bc2-d54c5c33b065" />



The figure shows the schematic for the electrical circuit made in KiCad





<img width="801" height="558" alt="Screenshot 2025-10-15 at 14 39 55" src="https://github.com/user-attachments/assets/25f01901-ced0-400f-a348-0c3fbeebc275" />

<img width="801" height="558" alt="Screenshot 2025-10-15 at 14 40 08" src="https://github.com/user-attachments/assets/5f6a40a1-ea9a-4163-9bf4-20fcfe5a33c2" />




The figures shows the KiCad circuit on a PCB board and a 3D design of the PCB board to print for later use 






