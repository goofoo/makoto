# Server program

import socket, time, threading, os, commands
from optparse import OptionParser
from time import gmtime, strftime

# set command line option
parser = OptionParser()
parser.add_option("-p", "--processor", dest="mxproc", type = "int", help="set max number of threads")
args = ["-p", "1"]
(options, args) = parser.parse_args()

max_active_proc = options.mxproc
print 'max processor ',max_active_proc

class NilProc(threading.Thread):
        def __init__(self, scmd):
                self.scmd = scmd
                threading.Thread.__init__(self)
        def run(self):
                print 'start exec',self.scmd
		commands.getoutput(self.scmd)
                print 'end'

# Set the socket parameters
host = ''
port = 52907
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((host, port))
print 'waiting for client'
s.listen(1)
conn, addr = s.accept()
print 'connected by', addr

# Receive messages
while 1:
	data = conn.recv(1024)
	if not data:
		print 'client',addr,'lost, waiting for connection'
		s.listen(1)
                conn, addr = s.accept()
                print 'connected by', addr
	else:
##              print "\nReceived message '", data,"'"
		if data[:4] == 'file':
# check status here
                        if threading.activeCount() > max_active_proc:
                                conn.send('busy')
                        else:
# start process here
                                print strftime("%a, %d %b %Y %H:%M:%S", gmtime())
                                conn.send('roger')
                                NilProc(data[5:]).start()
                     
                else:
                        conn.send('default')
# Close socket
conn.close()

