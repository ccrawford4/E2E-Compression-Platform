#!/bin/bash

# Check if exactly two arguments are given
if [ "$#" -ne 2 ]; then
    echo "Error: Incorrect number of arguments."
    echo "Usage: $0 {client|server} [config_file|port]"
    exit 1
fi

mode=$1
arg=$2

if [ "$mode" == "client" ]; then
    # Navigate to the client directory, build, execute, and clean up
    cd client || { echo "Error: Failed to change directory to client."; exit 1; }
    make || { echo "Error: Make failed."; exit 1; }
    ./compdetect_client "$arg" || { echo "Error: Failed to execute client."; exit 1; }
    make clean || { echo "Error: Make clean failed."; exit 1; }

elif [ "$mode" == "server" ]; then
    # Navigate to the server directory, build, execute, and clean up
    cd server || { echo "Error: Failed to change directory to server."; exit 1; }
    make || { echo "Error: Make failed."; exit 1; }
    ./compdetect_server "$arg" || { echo "Error: Failed to execute server."; exit 1; }
    make clean || { echo "Error: Make clean failed."; exit 1; }

else
    # Error if the first argument is neither "client" nor "server"
    echo "Error: Invalid mode specified. Use 'client' or 'server'."
    echo "Usage: $0 {client|server} [config_file|port]"
    exit 1
fi
