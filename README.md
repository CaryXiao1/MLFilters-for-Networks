# What is 'MLFilters for Networks'? 
'Machine Learning Filters for Networks' is an Arduino-based embedded system designed to wirelessly send and receive information in disaster-relief scenarios. Aside from using 5mW red laser diodes to send information, this system uses Manchester Encoding and a Machine Learning algorithm to dramatically reduce error in messages relating to natural disaster relief efforts. Different parts of this project qualified for and were presented in the 2020 & 2021 Regeneron ISEF, respectively. 

2020 Project Abstract: https://abstracts.societyforscience.org/Home/FullAbstract?ProjectId=19642

2021 Project Abstract: https://abstracts.societyforscience.org/Home/FullAbstract?ProjectId=20665

This project uses the jSerialComm and Weka Java libraries to aid in its software-based error correction, and this repository primarily holds the code for the Arduino and error correction system of this project.

# How does it work?
To send information, each Arduino-based device toggles a 5mW red laser diode, beaming information to the custom Printed Circuit Board of another device. To receive information, each device records the change in light within a given time interval, converting the changes back into the sent message.

If interruptions to the message are detected, their positions are recorded, and  erroneous characters that create characters outside a valid character set are corrected. Using jSerialComm, a custom Java-based Serial Monitor then receives the updated messages and any uncorrected errors. These errors and their corresponding words are then sent through a Machine Learning algorithm, trained and implemented using Weka, to correct the majority of the remaining errors before the final, corrected message is shown to the user.
