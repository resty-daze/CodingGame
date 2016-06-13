# coding: utf-8

import sys
from subprocess import PIPE, Popen
from threading  import Thread
from Queue import Queue

ON_POSIX = 'posix' in sys.builtin_module_names

def enqueue_output(stream, queue):
    for line in iter(stream.readline, b''):
        queue.put(line)
    stream.close()

class StreamWrapper:
    def __init__(self, stream):
        self.stream = stream
        self.queue = Queue()
        self.thread = Thread(target=enqueue_output, args=(stream, self.queue))
#        self.thread.daemon = True
        self.thread.start()
    
    def getLine(self, timeout):
        return self.queue.get(timeout=timeout)

    def finish(self):
        self.thread.join()

class IOPlayer:
    def __init__(self, process_cmd, name):
        self.name = name

        self.process = Popen(process_cmd, stdin=PIPE, stdout=PIPE, stderr=PIPE, bufsize=1, close_fds=ON_POSIX)
        self.stdout = StreamWrapper(self.process.stdout)
        self.stderr = StreamWrapper(self.process.stderr)
        self.dump = []
        self.errorCounter = 0
        self.finished = False

    def writeToBot(self, line):
        if not self.finished:
            try:
                self.process.stdin.write(line + '\n')
                self.process.stdin.flush()
            except Exception, e:
                print e
                raise e
        self.dump.append(line)

    def getResponse(self, timeout):
        return self.stdout.getLine(timeout)
        

    def finish(self):
        self.finished = True
        self.process.terminate()
        self.stdout.finish()
        self.stderr.finish()
