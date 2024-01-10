# LLM_Controlled_Car
Project files for my chatbot controlled Arduino car


# This ultilizes the following:

KoboldCPP API: https://github.com/LostRuins/koboldcpp  
KoboldCPP model: https://huggingface.co/TheBloke/Toppy-M-7B-GGUF?not-for-all-audiences=true  
Python: https://www.python.org/downloads/  
Arduino (C++): https://www.arduino.cc/en/software  

# How it works:

There are two Arduino microcontrollers. Both connected to a NRF24L01 wireless module. One on the robot and one connected to my PC.  
The robot will drive forwards until it is met with a wall. Once met a wall it will determine if there are walls, on both sides, or whether just their left or right.  
The robot will then transmitt this information regarding its sorroundings to the PC connected arduino.  
Then the PC connected arduino will give the information to the PC. And the PC has a python script which awaits for this message where it will feed it into a prompt for the KoboldAI chatbot.  
The KoboldAI chatbot has a specific prompt so it gives us the responses that are ideal. Such as "Turn Left" or "Turn Right", which once if a proper response is generated it will transmit these instructions back to the robot.  

# This is a proof of concept and is very barebones but shows that the concept of "What if you used ChatGPT as a brain for a robot" is posssible.  
