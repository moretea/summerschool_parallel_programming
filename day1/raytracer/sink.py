import zmq
import time
import pickle
import sys

from raytrace import *

context = zmq.Context()

# Socket to negotiate env with the ventilator
ventilator = context.socket(zmq.REQ)
ventilator.connect("tcp://%s:5556" % sys.argv[1])

# Socket to receive results on
receiver = context.socket(zmq.PULL)
receiver.bind("tcp://*:5558")

# Ask for start of batch
ventilator.send("?")
params = ventilator.recv()
(totalw, totalh, nitems) = pickle.loads(params)

canvas = Canvas(totalw, totalh, "result_%dx%d" % (totalw, totalh))
canvas.save()

print "Client ready to receive %d blocks to fill %dx%d pixels." % (nitems, totalw, totalh)

# Start our clock now
tstart = time.time()

tprev = tstart
for i in xrange(nitems):
    msg = receiver.recv()

    (c, basex, basey) = pickle.loads(msg)

    print "Received %dx%d result at %dx%d." % (c.width, c.height, basex, basey)

    canvas.blit(c, basex, basey)

    tcur = time.time()
    if tcur - tprev > 10:
        canvas.save()
        currentfraction = float(i) / nitems
        print "%d%% complete" % (currentfraction * 100)
        tprev = tcur

# Calculate and report duration of batch
tend = time.time()
tdiff = tend - tstart
total_msec = tdiff * 1000
print "Total elapsed time: %d msec" % total_msec

print "Done, saving..."
canvas.save()

# Finished
time.sleep(1) # Give 0MQ time to deliver
