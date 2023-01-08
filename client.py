import argparse

import sys
import socket

def main():

    parser = argparse.ArgumentParser(description='Socket Error Examples')
    parser.add_argument('--host', action="store", dest="host", required=False)
    parser.add_argument('--port', action="store", dest="port", type=int, required=False)
    parser.add_argument('--file', action="store", dest="file", required=False)
    given_args = parser.parse_args()
    host = given_args.host
    port = given_args.port
    filename = given_args.file

    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error as e:
        print("Error creating socket: %s" % e)
        sys.exit(1)
    
    try:
        s.connect((host, port))
    except socket.gaierror as e:
        print("Address-related error connecting to server: %s" % e)
        sys.exit(1)
    except socket.error as e:
        print("Connection error: %s" % e)
        sys.exit(1) 
    
    fd = s.makefile('rw', 0)
    fd.write("GET %s HTTP/1.0\r" % filename)
    fd.write("Host: %s\r" % host)
    fd.write("\r")

    for line in fd.readlines():
        print(line)
    
if __name__ == '__main__':
    main()
