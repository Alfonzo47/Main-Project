from machine import Pin, PWM, ADC
import time
from as5600 import AS5600

# --- Configuration ---
adcpin = 28  # ADC pin connected to the potentiometer
pot = ADC(adcpin)

I2C_BUS_ID = 0
I2C_SDA_PIN = 4
I2C_SCL_PIN = 5
I2C_FREQ = 400000
DATA_FILE = "angle_data.txt"
LED_PIN = 25  # Onboard LED pin for Raspberry Pi Pico

# Function to save data
def save_data(angle_data, filename=DATA_FILE):
    """
    Appends the angle data to a specified file.

    Parameters:
    angle_data (float): The angle data to save.
    filename (str): The name of the file to save the data to.
    """
    try:
        with open(filename, "a") as file:
            file.write(f"{angle_data}\n")
        print(f"Data saved: {angle_data} to {filename}")
    except Exception as e:
        print(f"Error saving data: {e}")

# Function to extract saved data
def extract_data(filename=DATA_FILE):
    """
    Reads and returns the saved angle data from the specified file.

    Parameters:
    filename (str): The name of the file to read the data from.

    Returns:
    list: A list of saved angle data as floats.
    """
    try:
        with open(filename, "r") as file:
            data = [float(line.strip()) for line in file.readlines()]
        print(f"Data extracted from {filename}: {data}")
        return data
    except Exception as e:
        print(f"Error extracting data: {e}")
        return []

# Temporary function to clear the data file
def clear_data(filename=DATA_FILE):
    """
    Clears all data in the specified file.

    Parameters:
    filename (str): The name of the file to clear.
    """
    try:
        with open(filename, "w") as file:
            file.write("")  # Overwrite the file with an empty string
        print(f"Data cleared from {filename}")
    except Exception as e:
        print(f"Error clearing data: {e}")

def setup_pwm_output(pin_number, frequency=1000, duty_cycle=50):
    """
    Configures a GPIO pin to output a PWM signal.

    Parameters:
    pin_number (int): The GPIO pin number to use for PWM output.
    frequency (int): The frequency of the PWM signal in Hz (default: 1000 Hz).
    duty_cycle (int): The duty cycle of the PWM signal as a percentage (0-100, default: 50%).

    Returns:
    PWM: The configured PWM object.
    """
    try:
        pwm_pin = PWM(Pin(pin_number))  # Initialize the pin as a PWM pin
        pwm_pin.freq(frequency)  # Set the frequency
        pwm_pin.duty_u16(int((duty_cycle / 100) * 65535))  # Set the duty cycle (0-65535)
        print(f"PWM output configured on pin {pin_number} with frequency {frequency} Hz and duty cycle {duty_cycle}%.")
        return pwm_pin
    except Exception as e:
        print(f"Error setting up PWM output: {e}")
        return None

# Initialize I2C and AS5600
print("Initializing I2C...")
encoder = AS5600(I2C_BUS_ID, I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQ)
print("AS5600 initialized.")

led = Pin(LED_PIN, Pin.OUT)
led.on()  # Turn on the LED to indicate the program is running
print("LED is ON (Program is running).")

# Clear the data file (temporary usage)
clear_data()

# Main loop
try:
    while True:
        # Read the angle from the AS5600
        angle_in_degrees = encoder.read_angle()

        # Read the potentiometer value
        adc_value = pot.read_u16()  # Read ADC value (0–65535)
        voltage = (3.3 / 65535) * adc_value  # Convert to voltage
        print(f"Potentiometer Voltage: {voltage:.2f} V")
        
        duty_cycle1 = (adc_value / 65535) * 100  # Scale to 0–100%
        duty_cycle1 = round(duty_cycle1, 2)  # Round to 2 decimal places
        

        # Calculate the inverse duty cycle for the second PWM signal
        duty_cycle2 = 100 - duty_cycle1

        # Update the PWM signals' duty cycles
        pwm_signal1 = setup_pwm_output(pin_number=18, frequency=1000, duty_cycle=(int((duty_cycle1))) ) # Update duty cycle for PWM1
        pwm_signal2 = setup_pwm_output(pin_number=16, frequency=1000, duty_cycle=(int((duty_cycle2))) ) # Update duty cycle for PWM1
        print(f"Potentiometer ADC Value: {adc_value}")

        if angle_in_degrees is not None:
            # Save the angle data
            save_data(angle_in_degrees)
           
            # Print the angle
            print(f"Angle: {angle_in_degrees:.2f}")
        else:
            print("Error reading angle.")

        # Wait before the next reading
        time.sleep(1)  # Short delay for smoother updates
except KeyboardInterrupt:
    print("Program stopped. Extracting saved data...")
 
    # Turn off the LED when the program stops
    led.off()
    print("LED is OFF (Program stopped).")

    # Deinitialize the PWM signals
    if pwm_signal1:
        pwm_signal1.deinit()
    if pwm_signal2:
        pwm_signal2.deinit()

    # Extract and print saved data when the program is stopped
    saved_data = extract_data()
    print("Saved Data:", saved_data)