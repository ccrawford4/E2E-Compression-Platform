#!/bin/bash

# Check if exactly one argument is given
if [ "$#" -ne 1 ]; then
    echo "Error: Incorrect number of arguments."
    echo "Usage: $0 {client|server}"
    exit 1
fi

mode=$1

if [ "$mode" == "client" ]; then
    # Navigate to the client directory and clean up
    cd client || { echo "Error: Failed to change directory to client."; exit 1; }
    make clean || { echo "Error: Make clean failed."; exit 1; }

elif [ "$mode" == "server" ]; then
    # Navigate to the server directory and clean up
    cd server || { echo "Error: Failed to change directory to server."; exit 1; }
    make clean || { echo "Error: Make clean failed."; exit 1; }

else
    # Error if the argument is neither "client" nor "server"
    echo "Error: Invalid mode specified. Use 'client' or 'server'."
    echo "Usage: $0 {client|server}"
    exit 1
fi

