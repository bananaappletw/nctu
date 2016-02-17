import socket, sys 
ip = ''
port = 12345
 
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
 
sock.bind((ip, port))
 
sock.listen(5)
while True:
    (csock, adr) = sock.accept()
    print "Client Info: ", csock, adr
    msg = csock.recv(1024)
    if not msg:
        pass
    else:
        print "Client send: " + msg
        csock.send("Hello I'm Server.\r\n")
    csock.close()
