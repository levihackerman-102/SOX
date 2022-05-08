gcc server/server.c -o server/server
gcc client.c -o client

gnome-terminal --tab --title=client -- bash -c "./server/server -p 3005"

./client -p 3005
