# rp2040_balance_hid
Turns a Mettler Toledo balance with a serial port into a HID-device (keyboard)

Turns any Mettler Toledo scale with a serial port that follows [this](https://www.mt.com/dam/product_organizations/industry/apw/generic/11781363_La_MAN_RM_MTSICS-APW_en.pdf) protocol into a HID device.
With this tool you no longer need to type the weight into your computer, just press one of the three buttons to transfer the weight shown on the display to your computer.
One button sends just the weight, the other two send a TAB or ENTER key at the end of the transfer to make it even easier to enter the data into your preferred application.

The serial port settings are 9600 8N1.
