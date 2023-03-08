# AC current meter using ATMega8

This is a source code of simple AC current measurement system based on ATMega8. 

## How does it work?

The system uses split-core current transformer. Thus, it can measure only alternating current. I am using 50A to 1V transformer with shunt resistor built-in. 
Analog signal comes from CT to ADC pin of ATMega8 via op-amp. 
Analog signal has "virtual ground" made from resistor ladder between +5V and ground, so it can deal with positive and negative half of sine wave.

The MCU gets data from analog input every 1 ms and:
* calculates True RMS value every one second and converts to miliamps
* calculates and updates "virtual zero" as a midpoint of min and max value of last 128 measurements. 
* returns RMS output via serial port.

## Calculating value to mA ratio

RMS calculating is based on direct analog input reading, represented as 10-bit value, where 0 means 0V and 1023 means reference voltage (5V supply voltage). 
Just after RMS is computed, it is multiplying by constant ratio defined in TO_MILIAMPS to achieve final value in mA. 

The formula to calculate TO_MILIAMPS is:

```
TO_MILIAMPS = V_ref * CT_ratio / (1.024 * OA_gain) 
```

Where:
* `TO_MILIAMPS` - ratio (miliamps per 1 unit)
* `V_ref` - ADC Reference voltage [V]
* `CT_ratio` - Current transformer ratio [A/V]
* `OA_gain` - Op-amp gain (ratio, not in dB)

`TO_MILIAMPS` value must be an integer. 

# TODO

* Schematics (prototyping needs to be finished
* Use internal/external voltage reference (instead of 5V supply)
* Calibration procedure
* Impulse LED (pulses every some mAh, like DSO energy meters do)

