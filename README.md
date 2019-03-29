# i-Light
An intelligent street light which brightness decreases when no movement is detected. It's for power saving and also it will help some creatures like "Fireflies" to grow their generations and also improve plants life cycle.


Implemented it using STM32f407VGT.
PIR sensor for person detection
Piezo-electric sensor for vehicle detection. (By reading the output voltage through PA1 pin (ADC2 cha1)
Trigger the system at the night time only by reading the voltage across LDR throgh PA0 pin(ADC1 ch0).
Gradually increase or decrease of brightness using PWM.
