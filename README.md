# SafeBoxArduino
Safe Box using Arduino Mega and Arduino Uno.
Arduino Mega validates PIN code from user, received from a keypad, and sends open command to Arduino Uno if the PIN is correct or close command by pressing key C. If the PIN is introduced wrong 3 times, safe is blocked for 1 minute. The user must firstly configure PIN introducing by pressing * and validate the input by pressing #, also he can delete one character by pressing D. A phone connected to HC 05 Bluetooth module can send L command to Arduino Mega which blocks the safe completely.
Arduino Uno opens and closes the safe using a servomotor, depending on the information received from Arduino Mega.
The two boards are communicating using Inter-Integrated Circuit Protocol(I2C).
