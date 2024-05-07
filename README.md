# PQBlockchain

This application is part of my Bachelor's thesis. For in-depth details, see the [technical report](/doc/thesis/Thesis.pdf).

This application works as a blockchain client. The idea was to create a blockchain that would use the new post-quantum cryptography and be resistant to quantum attacks. The main goal was to test the performance of post-quantum algorithms within a blockchain.


## Repository contents

In other folders of this repository are additional README files, which provide a more in-depth explanation of their content.

|**Folder** | **Content**|
|---|---|
| _./doc_ | Doxygen, documentation, technical report, LaTeX sources, images, logo, poster |
| _./src_ | Application source code |
| _./extern_ | External libraries |
| _./tests_ | Application unit tests|
| _./scripts_ | Scripts for automatic testing |


## Requirments

The application was tested on Linux Ubuntu 22.04

+ gcc compiler
+ C++ 20
+ CMake 3.28
+ Python 3.10 (for testing)
+ Docker (for testing)

## External Libraries

|**Library** | **Licence** | **Usage** |
|---|---|---|
| Crypto++ | [BSD 3-Clause License](./extern/cryptopp-cmake/LICENSE) | SHA-512, Ed25519, and ECDSA algorithms |
| GoogleTests | [BSD 3-Clause License](./extern/googletest/LICENSE) | Unit testing |
| nlohmann JSON | [MIT](./extern/json/LICENSE.MIT) | JSON file parsing |
| LevelDB | [BSD 3-Clause License](./extern/LevelDB/LICENSE) | Embeded key-value database |
| PQClean | Custom | Post-quantum algorithms for digiral signatures |
| SPD log | [MIT](./extern/spdlog/LICENSE) | Management of application logs |


## Build

**WARNING: Make sure that all commands are executed from the root folder of this repository (The project and applications are using relative paths!).**

### make commands

|**Command** | **Action**|
|---|---|
| _setup_ | Create `tmp` and build `directories` |
| _make_ | Do configuration and compilation (might be required to be run twice) |
| _configure_ | Generate CMake configuration (might be required to be run twice) |
| _compile_ | Compile the project and external libraries |
| _run_ | Run the main application. Requires arguments for the program `make run ARGS=<program_arguments>` |
| _test_ | Run one test suit. Requires name of test suit as argument `make test ARGS=<test_suit_name>` |
| _testall_ | Run all test suits |
| _clean_ | Remove temporary files created by main application/tests and generated documentation |
| _cleanall_ | Remove whole build and generate temporary files |
| _doc_ | Generates Doxygen documentation |

### Compile

For configuration and compilation run: `make` (It is possible that the first run of `make` will fail. In that case, it is necessary to run the `make` command again a second time.)

To clean all generated files, use the command `make cleanall`


## Usage

The compiled application is located in `./build/src/App/pqb`. The program help can be printed to the console with `./build/src/App/pqb -h/--help` or `make run ARGS="-h/--help"`

There is one required argument:

+ `-s/--sig <signature_algorithm>` - Name of the signature algorithm to use. There are implemented algorithms: falcon512, falcon1024, dilithium2, dilithium3, dilithium5, ed25519 and ecdsa.

Only one optional argument is:

+ `-c/--conf <path_to_wallet_configuration_file>` - Select the wallet configuration file. If not used, the default path is in the local directory `tmp/conf.json`.


### Examples:

Note that before running the application `tmp` folder has to be created in the repository root folder. The guid how to establish multiple nodes at once with Docker is described [here](./scripts/README.md).

**Run the application**

`make run ARGS="-s falcon512"`  
`./build/src/App/pqb -s falcon512`

**Run the application with the configuration file**

`make run ARGS="-s falcon512 -c tmp/conf1.json"`  
`./build/src/App/pqb -s falcon512 -c tmp/conf1.json`


### Consol interface

After the application starts, it opens the databases and wallet configuration file (if they do not exist, it creates them). Then the application establishes connections with its peer on the UNL list (see [technical report](/doc/thesis/Thesis.pdf)) and commands for the application can be input to the console.

Application supports this commands:

|**Command** | **Arguments** | **Action**|
|---|---|---|
| _createTx_ | `<amount>` `<receiver_address>` | Create a transaction |
| _whoami_ | None | Print address of local wallet |
| _walletTxs_ | None | Print transactions made with the local wallet or received by this wallet |
| _blocks_ | None | Print list of blocks in the database |
| _blockTxs_ | `<block_id>` | Print transactions in block with given `block_id` |
| _accs_ | None | Print list of accounts in the database |
| _chain_ | None | Print the chain of blocks |
| _conns_ | None | Print currently established connections |
| _echo_ | `<string>` | Just return the imputed string (for testing purposes) |
| _exit_ | None | Close the application |