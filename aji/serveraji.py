# Server program

import socket, time, threading

class NilProc(threading.Thread):
        def __init__(self):
                threading.Thread.__init__(self)
        def run(self):
                print 'hilstart'
                for n in range(300):
                        for u in range(300):
                                print n, ':', u
                print 'nilend'

# Set the socket parameters
host = ''
port = 52907
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((host, port))
print 'waiting for client'
s.listen(1)
conn, addr = s.accept()
print 'connected by', addr

max_active_proc = 2

# Receive messages
while 1:
	data = conn.recv(1024)
	if not data:
		print 'client',addr,'lost, waiting for connection'
		s.listen(1)
                conn, addr = s.accept()
                print 'connected by', addr
	else:
                print "\nReceived message '", data,"'"
		if data[:4] == 'file':
# check status here
                        if threading.activeCount() > max_active_proc:
                                conn.send('busy')
                        else:
                                conn.send('roger')
                                NilProc().start()
# start process here
                                
                else:
                        conn.send('default')
# Close socket
conn.close()

