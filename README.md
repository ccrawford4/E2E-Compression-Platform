# E2E-Compression-Platform
Source code for the server and client topology to test compression over a network
### Getting Started
1. Clone the repository
```bash
$ git clone git@github.com:ccrawford4/E2E-Compression-Platform.git
```
2. Navigate to the root directory
```bash
$ cd E2E-Compression-Platform
```
3. Configure the JSON parser by running the configure script (yes sudo privlidges are required)
```bash
$ sudo ./configure.sh
```
4. Navigate to the shared directory
```bash
$ cd shared
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
$ cd ..
```
7. Start the server program using the run.sh script
```bash
# Make sure that the port number is the same as the 
# one specified in the myconfig.json file under "TCP_PREPROB_post_number"

# Usage:
# ./run.sh server <port_number>

# Example:
$ ./run.sh server 7777
```
8. To test it comprehensively you can navigate to a separate VM and run the client program using the run.sh script
```bash
# Make sure to include the name of your JSON config file 
# located in the 'shared' directory for the 
# second command line argument

# Usage: 
# ./run.sh client <config file name>.json

# Example:
$ ./run.sh client myconfig.json
```
### Result
