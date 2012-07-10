import zmq
import pickle
import sys
from raytrace import *

import signal

interrupted = False

def signal_handler(signum, frame):
    global interrupted
    interrupted = True

signal.signal(signal.SIGINT, signal_handler)

def do_work(message):
    (scene, basex, basey, localw, localh, totalw, totalh) = pickle.loads(message)

    print "Received work: %dx%d at %dx%d of total image %dx%d" % (localw, localh, basex, basey, totalw, totalh)

    c = Canvas(localw, localh, 'local_%dx%d_at_%dx%d' % (localw,localh,basex,basey))
    scene.render(c, basex, basey, totalw, totalh)
    c.save()

    print "Completed work, sending result to sink"

    return pickle.dumps((c, basex, basey))


context = zmq.Context()

# Socket to receive messages on
ventilator = context.socket(zmq.REQ)
ventilator.connect("tcp://%s:5557" % sys.argv[1])

# Socket to send messages to
sender = context.socket(zmq.PUSH)
sender.connect("tcp://%s:5558" % sys.argv[2])

# Process messages from both sockets
while True:

    if interrupted:
        print "Interrupted!"
        break

    ventilator.send("?")
    message = ventilator.recv()

    if message == "BYE":
        break;

    # Do the work
    result = do_work(message)

    # Send results to sink
    sender.send(result)
    

print "No more work, exiting worker..."
