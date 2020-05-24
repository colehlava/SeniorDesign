# File name:   streamVideo.py
# Description: Streams video from remote to local through TCP socket.

import cv2
import os
import json
import time
import pickle
import signal
import socket
import threading
import argparse

def readSocket(connection, messageSize):
    message = b""

    while len(message) < messageSize:
        try:
            message += connection.recv(messageSize)
        except ValueError:
            raise SystemExit
        except socket.error:
            print("Error while receiving message")
            return b""

    return message


def sendData(s):
    cap = cv2.VideoCapture(0)

    while True:
        # Capture frame
        ret, frame = cap.read()

        try:
            # Pickle the frame and send it over the socket
            sendable = pickle.dumps(frame)
            s.send(sendable)
        except socket.error:
            print("Failed to send message")

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # Release capture on program termination
    cap.release()


def receiveData(connection):
    messageSize = 921764

    while True:
        # Receive frame
        pickledFrame = readSocket(connection, messageSize)
        frame = pickle.loads(pickledFrame)

        # Format frame into image
        #image = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY) # grayscale image
        image = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV) # HSV image

        # Display the resulting frame
        cv2.imshow('frame', image)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # Destroy window on program termination
    cv2.destroyAllWindows()


# ====================================================================
# main
# ====================================================================
def main():
    portNumber = 63008

    try:
        # Create socket with ipV4 addressing and TCP protocol
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        print("Socket created")
    except socket.error:
        print("Error while creating socket")
        raise SystemExit

    # Handles SIGINT (ctrl+C) by first closing socket then exiting application
    def handle_sigint(frame_, signal_):
        print("\nClosing application...\n")
        s.close()
        os._exit(1)

    # Start signal handler
    signal.signal(signal.SIGINT, handle_sigint)

    # Parse command line arguments
    parser = argparse.ArgumentParser()
    parser.add_argument("-b", "--boss", help="Run in boss mode (create listening socket)", action="store_true")
    args = parser.parse_args()

    # Boss mode
    if args.boss:
        # Bind the socket to local machine and begin listening
        s.bind(('', portNumber))
        s.listen(4)

        # Wait until a connection is established with a client
        connection, clientCredentials = s.accept()
        print("Connected to " + clientCredentials[0] + "\n")

        # Start receiving thread
        receiveThread = threading.Thread(target=receiveData, args=(connection,))
        receiveThread.start()

    # Client mode
    else:
        # Get IP address
        try: 
            host_name = socket.gethostname() 
            ip_address = socket.gethostbyname(host_name) 
            #ip_address = '169.233.136.70'
            print("IP = " + ip_address)
        except: 
            print("Unable to get host IP")

        # Uncomment to enter IP manually
        #ip_address = input('Enter recipients IP address: ') 
        print("About to connect")

        # Attempt to connect to host
        while True:
            try:
                s.connect((ip_address, portNumber))
                print("Established connection with host\n")
                break

            except:
                print("Connection refused. Attempting to reconnect...")
                time.sleep(5)

        # Start sending thread
        sendThread = threading.Thread(target=sendData, args=(s,))
        sendThread.start()


    # Allow threads to run until SIGINT is received
    while True:
        pass


# Begin program
if __name__ == '__main__':
    main()

