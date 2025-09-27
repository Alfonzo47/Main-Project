from machine import I2C, Pin

class AS5600:
    def __init__(self, i2c_bus_id, sda_pin, scl_pin, frequency=400000):
        self.i2c = I2C(i2c_bus_id, sda=Pin(sda_pin), scl=Pin(scl_pin), freq=frequency)
        self.address = 0x36  # Default I2C address for AS5600
        self.raw_angle_reg = 0x0C  # Register for raw angle
        self.angle_reg = 0x0E  # Register for processed angle

    def read_raw_angle(self):
        """Reads the raw angle value from the AS5600."""
        try:
            self.i2c.writeto(self.address, bytearray([self.raw_angle_reg]))
            data = self.i2c.readfrom(self.address, 2)
            raw_angle = (data[0] << 8 | data[1]) & 0x0FFF  # 12-bit value
            return raw_angle
        except Exception as e:
            print(f"Error reading raw angle: {e}")
            return None

    def raw_to_degrees(self, raw_value):
        """Converts the raw angle value to degrees."""
        return (raw_value / 4096) * 360

    def read_angle(self):
        """Reads the angle in degrees from the AS5600."""
        raw_value = self.read_raw_angle()
        if raw_value is not None:
            return self.raw_to_degrees(raw_value)
        return None