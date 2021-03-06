log = './log.txt'
time_gap = 86400

def sholog(path):
        a = strftime("%a, %d %b %Y %H:%M:%S ", time.localtime(os.path.getmtime(path))) + path
        print a
        f = open(log, 'a')
        f.write(a +'\n')
        f.close()

def hassubdir(path):
        a = os.listdir(path)
        for x in a:
                if os.path.isdir(path+'/'+x):
                        return 1
        return 0
	
def lookforfiles(path):
        if time.time() - os.path.getmtime(path) < time_gap:
                if hassubdir(path):
                        a = os.listdir(path)
                        for x in a:
                                if os.path.isdir(path+'/'+x):
                                        lookforfiles(path+'/'+x)
                else:
                        sholog(path)

	
if __name__ == "__main__":
    import os, sys, time
from time import gmtime, strftime
p = '.'	
f = open(log, 'w')
f.write('Update from ' + strftime("%a, %d %b %Y %H:%M:%S ", time.localtime(time.time() - time_gap)) + ' to ' + strftime("%a, %d %b %Y %H:%M:%S ", time.localtime(time.time())) + '\n')
f.close()
print 'Update since last 24 hours:'
lookforfiles(p)
	
