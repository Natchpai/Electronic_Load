# DC Electronic Loads Class Project

> A DC electronic load is a test instrument that simulates an electronic load to test the performance of a power source.

This project is **ESP32-Base** (I use esp32 because the processor's clock is fast than ATMega328 on Arduino Family)
ESP32 generating a PWM signal to controls the Gate-Source Voltage of MOSFET to drawn current from a power source, Measurement the currrent and feedback to control loop.

<img height="500" alt="Picture1" src="https://github.com/Natchpai/Electronic_Load/blob/main/IMG_8593.JPG" /> 

## Features
- Constant Current: Max 30A. *(I've only tested at 20A@12V. It works)*
- Constant Power
- Constant Resistance
- Digital Voltmeter(Add-on)

## 🛠️ How It Works
1. Sensing: A shunt resistor I use 0.1 Ohm. Measurement the current by read the voltage across this resister use Op-Amp (OP07)
2. Conversion: The Analog signal from Op-Amp is fed into an External ADC. This chip converts the voltage into a high-resolution digital value.
3. Control: ESP32 reads this digital value from the ADC. Compares this current to set-point and Find the difference between them. It called error.
   A PI controller algorithm running on the ESP32 calculates the exact adjustment needed to eliminate this error.
4. Result: The PI controller's output is a new PWN duty cycle. send to 2nd-order Low pass filter to give the dc level to Gate of MOSFET.

## 🛠️ Schematics
<img height="700" alt="sch1" src="https://github.com/Natchpai/Electronic_Load/blob/main/Electronic_Load_sch_color_page-0001.jpg" />
<img height="700" alt="sch2" src="https://github.com/Natchpai/Electronic_Load/blob/main/Electronic_Load_sch_color_page-0002.jpg" />

<img height="400" alt="Picture2" src="https://github.com/Natchpai/Electronic_Load/blob/main/IMG_8594-2.JPG" /><img height="300" alt="sch2" src="https://github.com/Natchpai/Electronic_Load/blob/main/PICTURE/DISPLAY%20LABEL%20-%20Const%20I.png" />
