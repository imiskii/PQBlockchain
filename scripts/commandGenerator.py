
# This script takes command script and replace transaction receivers with their address
# based on configuration file, which was created during configuration generation (see conf-generator.cpp)
# If command script include commands that are for nodes not presented in configuration file it ignores them.

import sys

# Generate commands from command script
def generate_commands(config_file, command_script):
    with open(config_file, 'r') as f:
        nodes = [line.split()[1] for line in f.readlines()[1:]]

    with open(command_script, 'r') as f:
        lines = f.readlines()
    
    commands = ""
    for idx, cmd in enumerate(lines, start=1):
        split = cmd.split()

        # Always add sleep command
        if split[0] == "sleep":
            commands += cmd
            continue

        # Filter command of nodes that are not in the configuration
        node_num = int(split[0][4:])
        if (node_num - 1) < len(nodes):
            # Replace node number with exact address
            if split[1] == "createTx":
                rec_node_num = int(split[3][4:])
                # Add only if the receiver is in configuration
                if (rec_node_num - 1) < len(nodes):
                    commands += (split[0]  + " " + split[1] + " " + split[2] + " " + nodes[(rec_node_num - 1)] + "\n")
            elif split[1] == "exit":
                commands += cmd
                if idx == len(lines):
                    commands += "\n"
                commands += "sleep 500\n" # put a little sleep after exit just do get time for statistics output (it looks better in terminal afterwards)
            else:
                commands += cmd

    return commands


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py <node_configuration.txt> <command_script.txt>")
        sys.exit(1)

    config_file = sys.argv[1]
    command_script = sys.argv[2]
    commands = generate_commands(config_file, command_script)
    print(commands)