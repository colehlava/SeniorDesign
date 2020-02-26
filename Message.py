# File name:   Message.py
# Description: Runs messaging application.

import os
import json
import time
import signal
import socket
import threading
import argparse

def readSocket(connection, messageSize):
    message = ""

    while len(message) < messageSize:
        try:
            message += connection.recv(messageSize).decode()

        except ValueError:
            raise SystemExit
        except socket.error:
            print("Error while receiving message")
            return ""

    return message


def sendData(s):
    while True:
        message = input('')
        messageSize = len(message)
        messageSizeStr = "%04d" % messageSize

        try:
            s.send(messageSizeStr.encode())
            s.send(message.encode())
            time.sleep(1)

        except socket.error:
            print("Failed to send message")


def receiveData(connection):
    headerSize = 4

    while True:
        messageSize = int( readSocket(connection, headerSize) )
        message = readSocket(connection, messageSize)
        print("\n\n>>> " + message + "\n")


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

        # Start sending thread
        sendThread = threading.Thread(target=sendData, args=(connection,))
        sendThread.start()

    # Client mode
    else:
        # Get IP address
        try: 
            host_name = socket.gethostname() 
            ip_address = socket.gethostbyname(host_name) 
            print("IP = " + ip_address)
        except: 
            print("Unable to get host IP")

        # Uncomment to enter IP manually
        ip_address = input('Enter recipients IP address: ') 

        # Attempt to connect to host
        while True:
            try:
                s.connect((ip_address, portNumber))
                print("Established connection with host\n")
                break

            except:
                print("Connection refused. Attempting to reconnect...")
                time.sleep(5)

        # Start receiving thread
        receiveThread = threading.Thread(target=receiveData, args=(s,))
        receiveThread.start()

        # Start sending thread
        sendThread = threading.Thread(target=sendData, args=(s,))
        sendThread.start()


    # Allow threads to run until SIGINT is received
    while True:
        pass


# Begin program
if __name__ == '__main__':
    main()

