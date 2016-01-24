import time
import zmq
from Queue import Queue, Empty
from threading import Thread

# s.connect('tcp://fdfd::ff:fe00:10:9999')

ctx = zmq.Context()

push = ctx.socket(zmq.PUB)
push.setsockopt(zmq.IPV6, 1)
push.connect('tcp://aaaa::600:fbff:a2df:5d20:8888')
time.sleep(1)

pull = ctx.socket(zmq.SUB)
pull.setsockopt(zmq.IPV6, 1)
pull.setsockopt(zmq.SUBSCRIBE, '')
pull.connect('tcp://aaaa::600:fbff:a2df:5d20:9999')

poller = zmq.Poller()
poller.register(pull, zmq.POLLIN)

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
    if pull in socks and socks[pull] == zmq.POLLIN:
        data = pull.recv().split('\0')
        print '< ', data
        # if (data[0] == 'sensor://nucleo-sensor-demo/button') and (data[1] == 'pressed'):
        #     push.send('action://nucleo-sensor-demo/led/blue/toggle')

    try:
        data = q.get_nowait()
        # data = q.get()
        push.send_multipart(data.split(' '))
    except Empty:
        pass
