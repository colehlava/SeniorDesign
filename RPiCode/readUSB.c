// readUSB.c
// Reads USB data and writes it to a file so python script can access it.

#include <stdio.h>
#include <libusb.h>

#define MESSAGE_SIZE 1

int main (int argc, char * argv[]) {

  // Pin configurations
  wiringPiSetup (); // @TODO what is this for?
  
  libusb_device_handle* dev; // Pointer to data structure representing USB device

  char rx_data[MESSAGE_SIZE]; // Receive data block
  int rcvd_bytes; // Bytes received
  int return_val;

  libusb_init(NULL); // Initialize the LIBUSB library

  // Open the USB device (the Cypress device has
  // Vendor ID = 0x04B4 and Product ID = 0x8051)
  dev = libusb_open_device_with_vid_pid(NULL, 0x04B4, 0x8051);

  if (dev == NULL) {
    perror("Device not found\n");
    return -1;
  }

  // Reset the USB device.
  // This step is not always needed, but clears any residual state from
  // previous invocations of the program.
  if (libusb_reset_device(dev) != 0) {
    perror("Device reset failed\n");
    return -1;
  } 

  // Set configuration of USB device
  if (libusb_set_configuration(dev, 1) != 0) {
    perror("Set configuration failed\n");
    return -1;
  } 


  // Claim the interface. This step is needed before any I/Os can be
  // issued to the USB device.
  if (libusb_claim_interface(dev, 0) !=0) {
      perror("Cannot claim interface");
      return -1;
    }

  // Write received data to this file 
  FILE* fp;
  fp = fopen("usbdata.log", "a");

  // Main loop
  for (;;) { 
    // Perform the IN transfer (from device to host: PSoC to Pi).
    // This will read the data back from the device.
    return_val = libusb_bulk_transfer(dev, // Handle for the USB device
                    (0x01 | 0x80), // Address of the Endpoint in USB device
                    // MS bit nust be 1 for IN transfers
                    rx_data, // address of receive data buffer
                    MESSAGE_SIZE, // Size of data buffer
                    &rcvd_bytes, // Number of bytes actually received
                    0 // Timeout in milliseconds (0 to disable timeout)
                    );

    // Write to file if read was successful, otherwise throw an error
    if (return_val == 0) {
        fputc(rx_data, fp); // use fputs() if MESSAGE_SIZE > 1
    }
    else {
      perror("Bulk transfer failed");
      return -1;
    }
  }

  // Cleanup on program termination
  libusb_close(dev);
  fclose(fp);
}

