# Client program

import socket, time, threading
from optparse import OptionParser

# set command line option
parser = OptionParser()
parser.add_option("-f", "--file", dest="filename", help="read ascii job script file")
args = ["-f", "foo.txt"]
(options, args) = parser.parse_args()

print 'read job script',options.filename

# Set the socket parameters
host = '127.0.0.1'
port = 52907

# Create socket
#s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
#try:
#        s.connect((host,port))     
#except socket.error, msg:
#        print 'server', host ,'offline, stop '
#        sys.exit(1)
#s.send('Hello, world')
#data = s.recv(1024)
#print 'get reply ', data

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
                        sys.exit(1)
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
                        print 'sending ',jobstack[nleft-1]
                        try:
                                data = s.recv(1024)
                        except socket.error, msg:
                                print 'server lost, stop'
                                break
                        if data:
                                if data == 'roger':
                                        nleft -= 1
                                        print 'sent!',nleft,'left'
                                
##                        else:
##                                print "no reply"
                        time.sleep(9)

                # Close socket
                s.close()


def hello():
        print 'oh my, not again'

class Listen(threading.Thread):
        def __init__(self):
                threading.Thread.__init__(self)
        def run(self):
                t = threading.Timer(3.0, hello)
                t.start()

talk = TalkTo('127.0.0.1')
talk.start()

#listen = Listen()
#listen.start()

# Send messages
##while (1):
##	data = raw_input('>> ')
##	if not data:
##		break
##	else:
##		if(s.send(data)):
##			print "Sending message '",data,"'....."
##			try:
##                                data = s.recv(1024)
##                        except socket.error, msg:
##                                print 'server lost, stop'
##                                break
##			if data:
##                                print "\n received msg ", data
##                                
##                        else:
##                                print "no reply"
##
### Close socket
##s.close()

