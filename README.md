# PIC_CLOCK_THERMOMETER

This project involves creating a digital clock and thermometer using a PIC16F876A microcontroller. The project includes the design for two printed circuit boards (PCBs): one for the main board and one for the push buttons.

The system is implemented using MPLAB X IDE and XC8 v2.46 compiler.

Features:
- 7-segment display with 4 digits to display time and/or temperature in Celsius;
- Programmable time-of-day alarm with a buzzer;
- 2 LEDs: one indicating the alarm state (on/off) and one indicating temperature signal (on for negative temperature, off for positive temperature).
- PCB: Two power supply possibility: 5V DC (VCC_USB) or DC up to 20V regulated by a LM7805. The power supply is selected via VCC_SEL pin header. In-Circuit Serial Programming (ICSP) compatible.

Peripherals:
- Temperature sensor: DS18B20;
- Real-time clock (RTC): DS3231;
- 7-segment display: TM1637.

Interaction:
The system can be interacted using 3 push buttons:
- STEP: Press STEP button during running mode to activate the updating mode, which blinks the selected field. Each press cycles through different fields in the following sequence:
    - 1: Clock hour (range: 0-23);
    - 2: Clock minute (range: 0-59);
    - 3: Alarm mode ('A' at first digit) (states: on/off);
    - 4: Alarm hour (range: 0-23);
    - 5: Alarm minute (range: 0-59);
    - 6: Temperature mode ('t' at first digit) (states: F(float)/I(integer));
    - 7: Display Brightness ('b' at first digit) (range: 0-7).
- INC: Changes the selected field's value to its next value;
- SET: Press SET button during updating mode to apply the current value to its corresponding field. In running mode, pressing SET will change the displayed data (clock, temperature, alternating clock/temperature). When the buzzer is active due to an alarm, pressing SET will disable it. Otherwise, the buzzer will be disabled automatically after about 30 seconds.
