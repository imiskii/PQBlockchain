
## Scripts for automatic testing with multiple nodes using Docker


Four things are needed to run full automatic tests with a predefined script scenario:

1. Create a configuration file for nodes
2. Create a script file with commands that will be executed for each node
3. Run the `./scripts/setup.sh`
4. Clean after the test `./scripts/clean.sh`

**IMPORTANT: Note that these scripts have to be run from the root folder of the repository**

### 1. Configuration file for nodes

There are already prepared configuration files in the [testSets](./testSets/) folder for 3, 5, 10, 15, and 20 node scenarios. The required structure of the configuration file is like this:

+ Each line represents one node
+ The values are separated by a single space
+ The first value is the IP address of the node. It has to be in subnet `10.5.0.0/24`.
+ The second value is the balance on which the node's account will be preinitialized
+ The third value is the node's UNL. Nodes are represented by their line number. Each node on UNL is separated by a single space. The node cannot contain itself in its UNL. 

**Example:**

Notice that the first node includes the second and third node in its UNL, the second node includes the first and third node, and the third node includes the first and second node. This is 100% overlap between the nodes' UNLs.


```
10.5.0.2 50000 2 3
10.5.0.3 50000 1 3
10.5.0.4 50000 1 2
```

### 2. Script file with commands

This script is more simple. It consists of application commands and a special command `sleep <duration>` that causes a delay in command executions. The `duration` is in milliseconds.

The required structure of the command script file is like this:

+ Each line is one command for the application
+ The first value is the node for which the command is targeted.
+ The second value is the application command with its arguments
+ In case of `createTx` command, the receiver address is replaced with a node number (see example)

**Example:**

It is good to let some initial time for establishing the connections and some more time before exiting the nodes so nodes can reach consensus.

```
sleep 10000
node1 createTx 10 node2
node2 createTx 10 node3
node3 createTx 10 node1
sleep 2000
node1 createTx 10 node2
node2 createTx 10 node3
node3 createTx 10 node1
sleep 30000
node1 accs
node1 exit
node2 exit
node3 exit
```

### 3. `setup.sh` script

This script creates the Docker image, sets up Docker containers, and runs them based on the given configuration and script files. It requires 3 arguments:

1. `<algorithm_name>` - Name of the digital signature algorithm that will be used
2. `<generation_conf>` - Configuration file for generating wallet files
3. `<command_script>` - Script with commands that will be run by nodes

There is not much of argument checking, so be careful when executing it.

**Examples:**

`./scripts/setup.sh falcon1024 scripts/testSets/3conf.txt scripts/testSets/3cmdScript.txt`
`./scripts/setup.sh ed25519 scripts/testSets/20conf.txt scripts/testSets/20cmdScript.txt`

### 4. `clean.sh` script

This script just clean up everything after testing. It deletes Docker image, containers and remove created temporary files.

**Example:**

`./scripts/clean.sh`