import requests
import serial.tools.list_ports

ports = serial.tools.list_ports.comports()
serial_inst = serial.Serial()

ports_list = []

for port in ports:
    ports_list.append(str(port))
    print(str(port))

val: str = input('Select Port: COM')

for i in range(len(ports_list)):
    if ports_list[i].startswith(f'COM{val}'):
        port_var = f'COM{val}'
        print(port_var)

serial_inst.baudrate = 115200
serial_inst.port = port_var
serial_inst.open()

# while True:
#     command = input("Ardunio Command: (ON/OFF): ")
#     # Direction = "1"
#     serial_inst.write(command.encode('utf-8'))

#     if command == 'exit':
#         exit()

def remove_chars_and_words(input_str):

    input_str = input_str.lower()

    please_send_help = ["user", "brain", ":", "[", "]", ".", "?", "!", "}", "{", "(", ")", ";", "'", '"']

    for text in please_send_help:
        input_str = input_str.replace(text, "")

    return input_str

# def trim_string(input_string, max_length):
#     if len(input_string) > max_length:
#         trimmed_string = input_string[:max_length]
#         return trimmed_string
#     else:
#         return input_string

# max_length = 14

def filter_input(input_str):

    input_str = input_str.lower()

    # valid_commands = ['move back', 'move forward', 'turn left', 'turn right']
    valid_commands = ['turn around', 'turn left', 'turn right']
    please_send_help = ["turn", "move"]
    
    # Iterate through the valid commands
    for command in valid_commands:
        # Check if the command is in the input string
        if command in input_str:

            for text in please_send_help:
                command = command.replace(text, "")

                return command  # Return the first detected command
    
    return 'halt'  # Return an empty string if no valid command is found


first_prompt = True
can_receive = True

if __name__ == "__main__":
    while True:

        if serial_inst.in_waiting and can_receive:
            packet = serial_inst.readline()
            print(packet.decode('utf').rstrip('\n'))


            if "wall" in packet.decode('utf').rstrip('\n').lower(): 
                can_receive = False

                # user_message = input("Scenario message: ")
                user_message = packet.decode('utf').rstrip('\n').lower()

                # if "exit" in user_message.lower():
                #     break

                if first_prompt:
                    chatbot_previous = ""
                    first_prompt = False

                chatbot_prompt = (
                    f" The following is the current scenario prompt: [ {user_message} ] "
                    + " Brain's response choice: "
                )

                chatbot_headers = {
                    "Accept": "application/json",
                    "Content-Type": "application/json",
                }

                chatbot_memory_description = " The following is a discussion between a user and a chatbot named Brain. Brain will not repeat itself. Brain will not speak as or for the user. Brain will only speak as itself and for itself. Brain will only respond with words not any other characters. User is expected to provide scenario prompts to Brain whereas Brain has to replay to these scenario prompts in a single sentence and never more. Example: User: There is a wall to your left: what do you do? Brain: Turn Right. Therefore Brain will only respond in a few words that provide a direction or action to perform. If Brain is prompted with a message other than that of a scenario; Brain must respond exactly with: 'Ask again'. Otherwise if Brain is correctly prompted; Brain will only choose one direction/movement option to each scenario prompt. Brain's choice is based on the desire to navigate around a wall they encounter. Rules for choosing options: 1. If there is nothing around Brain; Brain must move forwards until facing a wall. Brain will not move directly into a wall infront of them. 2. If Brain is too close to a wall infront of them they must move backwards. 3. If Brain is facing a wall but is able to turn they must turn in the direction without a wall. 4. If there is a wall to both Brain's left AND right; Brain will not turn but will only drive in the direction there is not a wall directly infront of them. The only options Brain is allowed to respond with are the following: 1. 'turn left', 3. 'turn around', and 2. 'turn right'. Brain will not respond with any other options. "

                chatbot_memory = (
                    " Chatbot information: [ "
                    + f" Chatbot description: [ {chatbot_memory_description} ] "
                    + " ] "
                )

                chatbot_data = {
                    "prompt": (chatbot_memory + chatbot_previous + chatbot_prompt),
                    "use_story": False,
                    "use_memory": False,
                    "use_authors_note": False,
                    "use_world_info": False,
                    "max_context_length": 256,
                    "max_length": 40,
                    "rep_pen": 1.12,
                    "rep_pen_range": 256,
                    "rep_pen_slope": 0.72,
                    "temperature": 0.70,
                    "use_default_badwordsids": False,
                    "tfs": 0.97,
                    "top_a": 0.8,
                    "top_k": 0,
                    "top_p": 0.6,
                    "typical": 1,
                    "singleline": False,
                    "frmttriminc": True,
                    "frmtrmblln": True,
                    "sampler_order": [6, 0, 1, 3, 4, 2, 5],
                    "stop_sequence": [
                        "user:",
                        "User:",
                        "user",
                        "User",
                        "user's",
                        "User's",
                        "users",
                        "Users",
                        "users:",
                        "Users:",
                        "user's:",
                        "User's:",
                    ],
                }

                try:
                    response = requests.post(
                        "http://localhost:5001/api/v1/generate",
                        headers=chatbot_headers,
                        json=chatbot_data,
                    )

                    if response.ok:
                        parsed_data = response.json()
                        if (
                            parsed_data.get("results")
                            and parsed_data["results"][0].get("text")
                        ):
                            chatbot_message = parsed_data["results"][0]["text"]

                            # new_chatbot_message_str = remove_chars_and_words(chatbot_message)

                            # final_result = trim_string(new_chatbot_message_str, max_length)

                            # chatbot_previous = f" Previous scenario information: [ Previous scenario prompt: [ {user_message} ] Previous Brain's response choice: [ {final_result} ] ] "

                            # Filter the input string
                            filtered_string = filter_input(remove_chars_and_words(chatbot_message))

                            print(f"Brain's response: {filtered_string}")
                            serial_inst.write(filtered_string.encode('utf-8'))
                            can_receive = True
                            # Perform actions with Chatbot_message as needed
                        else:
                            can_receive = True
                            print("Error: Unexpected JSON structure in response")
                            print("Error processing response! Ask something else.")
                    else:
                        can_receive = True
                        print("Error: Network response was not OK")
                        print("Please wait till I am done replying.")
                except Exception as error:
                    can_receive = True
                    print("Error:", error)
                    print("Error processing response! Ask something else.")

