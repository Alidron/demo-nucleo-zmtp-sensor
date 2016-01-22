import time
import zmq
from Queue import Queue, Empty
from threading import Thread

# s.connect('tcp://fdfd::ff:fe00:10:9999')

ctx = zmq.Context()

pub = ctx.socket(zmq.PUB)
pub.setsockopt(zmq.IPV6, 1)
pub.connect('tcp://aaaa::600:fbff:a2df:5d20:8888')

time.sleep(2)

sub = ctx.socket(zmq.SUB)
sub.setsockopt(zmq.IPV6, 1)
sub.setsockopt(zmq.SUBSCRIBE, '')
sub.connect('tcp://aaaa::600:fbff:a2df:5d20:9999')

poller = zmq.Poller()
poller.register(sub, zmq.POLLIN)

def read_input(queue):
    while(True):
        data = raw_input('> ')
        queue.put(data)

q = Queue()
t = Thread(target=read_input, args=(q,))
t.daemon = True
t.start()

while(True):
    socks = dict(poller.poll(50))
    if sub in socks and socks[sub] == zmq.POLLIN:
        print '< ', sub.recv_multipart()

    try:
        data = q.get_nowait()
        pub.send_multipart(data.split(' '))
    except Empty:
        pass
