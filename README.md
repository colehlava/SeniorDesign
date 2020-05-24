# Senior Design 2020 - Remote Inspection Robotic Arm

## Collaborators
Cole Hlava   
Jacob Lambert   
William Shaddix   

## Overview
We are students at University of California, Santa Cruz in the CSE 123 Senior Design class. This repository stores all code for our senior design project. Our project consists of building a robotic arm that can provide live video feed and sensor readings from its environment to a remote operator. This device could be ideal for performing maintenence and inspections in  harsh environments. We are using two Raspberry Pi's to transmit and receive controls and sensor and video data. The remote Pi hosts a web application that displays live video feed from the web cam and accepts user input to control the robotic arm. We are also using a Cypress PSoC 5 microcontroller to read the sensors and operate the motors on the robotic arm. The PSoC 5 receives user input controls and sends sensor readings to the Raspberry Pi through USB data transfers. The goal of our project is to reduce the danger associated with maintenence and inspections of hazardous sites by allowing technicians to perform these tasks remotely. 
