# Client program

import socket, time, threading, sys
from optparse import OptionParser
from time import gmtime, strftime

# set command line option
parser = OptionParser()
parser.add_option("-f", "--file", dest="filename", help="read ascii job script file")
args = ["-f", "foo.txt"]
(options, args) = parser.parse_args()

print 'read job script',options.filename

# Set the socket parameters
port = 52907

jobstack = []

f = open(options.filename,'r')
for line in f:
        jobstack.append(line[:len(line)-1])

print 'job count: ', len(jobstack)

nleft = len(jobstack)

class TalkTo(threading.Thread):
        def __init__(self, hostname):
                self.hostname = hostname
                threading.Thread.__init__(self)
        def run(self):
                # Create socket
                s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
                try:
                        s.connect((self.hostname,port))     
                except socket.error, msg:
                        print 'server', self.hostname ,'offline, stop '
                        sys.exit()
                s.send('status')
                data = s.recv(1024)
                print 'get reply from',s.getpeername(), data

                global nleft

                # Send messages
                while (nleft > 0):
##                        data = raw_input('>> ')
##                        if not data:
##                                print 'empty input stop talk to', self.hostname
##                                break
##                        else:
                        s.send(jobstack[nleft-1])
##                      print 'sending ',jobstack[nleft-1]
                        try:
                                data = s.recv(1024)
                        except socket.error, msg:
                                print 'server lost, stop'
                                break
                        if data:
                                if data == 'roger':
					nleft -= 1
					print strftime("%a, %d %b %Y %H:%M:%S", gmtime())
                                        print jobstack[nleft],'sent to',self.hostname,',',nleft,'left'
                        	##else:
                        		##print data
                        time.sleep(9)

                # Close socket
                s.close()

f = open('./ajihost.txt','r')
for line in f:
        TalkTo(line[:len(line)-1]).start()
        time.sleep(1)

