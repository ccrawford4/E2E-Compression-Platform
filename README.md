# E2E-Compression-Platform
Follow these steps to set up and run the applications for both the client and server in order to detect compression over a network
## Overview
### Objective
The goal of this project is to create applications that can be used by a client and server machine to facilitate communication via TCP/UDP and enable the server to perform the calculations necessary to detect if compression has occured over the network.
### How It Works
The client sends a stream of UDP packets with low entropy data, waits a given amount of seconds, and then sends a stream of high entropy UDP packets. The server will receive the two streams, calculate how long it took to receive them, and then will use the difference between these stream times to determine if compression has been detected over the network.
#### Steps
1. Server will listen for incoming TCP packets
2. Client will establish a TCP handshake and send the contents of the JSON config file
3. Client will send a stream of low entropy UDP packets
4. Server will receive the stream and calculate the total stream time
5. The client will wait "measurement_time" seconds (as specificed in the config file)
6. After the wait time is over steps 3 and 4 will repeat but this time with high entropy data
7. The server will use the difference between the two stream times to determine if compression has been detected
8. The client will establish another TCP handshake which the server will respond to with the results calculated in step 7
9. The client will print out the results received from the server
## Using The Program
### Getting Started
1. Clone the repository
```bash
git clone git@github.com:ccrawford4/E2E-Compression-Platform.git
```
2. Navigate to the root directory
```bash
cd E2E-Compression-Platform
```
3. Configure the JSON parser by running the configure script (yes sudo privlidges are required)
```bash
sudo ./configure.sh
```
Note: it may throw an error saying some packages are missing on the system but feel free to ignore those messages as they do not indicate anything wrong with the program.
4. Navigate to the shared directory
```bash
cd shared
```
5. Change the myconfig.json file to ensure that it has the correct server IP address and other desired values. Here is an example of how it should look like
```json
{
  "server_ip": "192.168.80.4",
  "UDP_src_port_number": "9876",
  "UDP_dest_port_number": "8765",
  "TCP_HEADSYN_dest_port_number": "9999",
  "TCP_TAILSYN_dest_port_number": "8888",
  "TCP_PREPROB_port_number": "7777",
  "TCP_POSTPROB_port_number": "6666",
  "UDP_payload_size": "1000B",
  "measurement_time": "15",
  "UDP_packet_train_size": "6000",
  "UDP_packet_TTL": "225",
  "server_wait_time": "5"
}
```
6. Go back to the root directory
```bash
cd ..
```
7. [From the server machine] start the server program using the run.sh script
```bash
# Make sure that the port number is the same as the 
# one specified in the myconfig.json file under "TCP_PREPROB_post_number"

# Usage:
# ./run.sh server <port_number>

# Example:
./run.sh server 7777
```
8. [From the client machine] run the client program using the run.sh script
```bash
# Make sure to include the name of your JSON config file 
# located in the 'shared' directory for the 
# second command line argument

# Usage: 
# ./run.sh client <config file name>.json

# Example:
./run.sh client myconfig.json
```
### Result
The client will receive the network compression results from the server and print them out like so:
```bash
No Compression Detected!
```
Note: If compression is detected over the network the client will print this out accordingly
### Removing Object Files
The source code includes a script that allows you to clean up all the object files and executables produced during the compilation of the program
```bash
# To clean up the client object files and executables:
./clean.sh client

# To clean up the server object files and executables:
./clean.sh server
```
