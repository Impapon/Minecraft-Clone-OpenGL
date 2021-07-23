import socket
import threading as th
import time
import sys

PORT = 5050
TICK = 8
FORMAT = 'utf-8'
SERVER = socket.gethostbyname(socket.gethostname())
ADDR = (SERVER, PORT)

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server.bind(ADDR) 

status = {}
listeners = []

verbose = "-verbose" in sys.argv[1:]
def on_tick():
    start = time.time()
    while True:
        end = time.time()
        if end-start >= 1/TICK:
            if verbose:
                print(f"Active connections: {th.active_count() - 2}")
                print(f"Ids = {len(status)}")

            #send game status 
            for i in status.keys():
                pos = status[i]
                for conn in listeners:
                    tos = " ".join(str(x) for x in pos)
                    print(f"[+]About to send: '{tos}'")
                    conn.send(tos.encode(FORMAT))
                                  

            start = time.time()

def handle_connection(conn, addr):
    print(f"[SERVER] New connection from {addr}")
    print(f"Connections: {len(status)}")

    first = conn.recv(64, 0).decode(FORMAT)
    conn.send("ok".encode(FORMAT))
    if first.startswith('play'): 
        conn.send(f'{len(status)+1}'.encode(FORMAT))
        while True:
            msg = conn.recv(64, 0).decode(FORMAT)
            if msg:
                conn.send("[+]".encode(FORMAT))
                if msg.startswith("-"):
                    print(f"[{addr}] says {msg[1:]}")
                    conn.close()
                    break
                else: 
                    info = msg[1:].split()
                    ID = int(info[0])
                    status[ID] = [ID] + [float(x) for x in info[1:]] 
                    #print(f"[{ID}] Pos: {status[ID]}")
        conn.close()
    elif first.startswith('listen'):
        listeners.append(conn)



    return


th.Thread(target=on_tick).start()
server.listen()
print(f"[SERVER] Listening on: {SERVER}")
while True:
    conn, addr = server.accept()
    th.Thread(target=handle_connection, args=(conn, addr)).start()
