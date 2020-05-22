# SeniorDesign2020 UCSC: Hands Free Inspection

## Collaborators
Cole Hlava   
Jacob Lambert   
William Shaddix   

## Overview
We are a group in UCSC's CSE123 Senior Design class. This repository stores all of our code, as well as gives an insight on our project and our research that went into it. As of right now our design project is building a robotic arm which can remotely provide live video feed, live Geiger Counter, humidity sensor, and temperature readings. We are using 2 Raspberry Pi's for transmitting and receiving data, and a Cypress PSoC 5 to read all of the data. There is a great risk associated with inspections at nuclear plants and sites, our goal is to reduce the danger of said inspections by allowing vision and radiation readings remotely. 


The code here allows for networking over wifi between the Raspberry Pi's which enable a video feed.  The PSoC5 implements temperature and humidity sensors. Both sensors use a voltage divider set up to gather the readings.
