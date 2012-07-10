import zmq
import random
import time
import pickle
import sys

from raytrace import *

context = zmq.Context()

# Socket to send messages on
workers = context.socket(zmq.REP)
workers.bind("tcp://*:5557")

# Socket with direct access to the sink: used to syncronize start of batch
sink = context.socket(zmq.REP)
sink.bind("tcp://*:5556")


# define the scene and rendering parameters
s = make_demo_scene()
(w, h, lw, lh) = map(int, sys.argv[1:])

print "Waiting for sink to connect..."
# Wait for sink to ask for parameters
sink.recv()

# Send parameters to sink
msg = pickle.dumps((w, h, (w/lw)*(h/lh)))
sink.send(msg)

print "Sink synchronized, sending to workers..."

# Send the tasks

for x in xrange(0, w, lw):
    for y in xrange(0, h, lh):
        
        # wait for a worker to be ready
        workers.recv()
        print "Sending work for %dx%d at %dx%d..." % (lw, lh, x, y)

        msg = pickle.dumps((s, x, y, lw, lh, w, h))
        workers.send(msg)

print "All work was sent."

while True:
    events = workers.poll(2000)
    if events == 0:
        break

    workers.recv()
    workers.send("BYE")

# Give 0MQ time to deliver
time.sleep(1)
