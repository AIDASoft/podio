

from __future__ import absolute_import, unicode_literals, print_function

import unittest
from EventStore import EventStore

if __name__ == '__main__':

    filename = 'example.root'
    store = EventStore([filename])
    for i, event in enumerate(store):
        if i % 1000 == 0:
            print('reading event', i)
        evinfo = store.get("info")[0]
        clusters = store.get("clusters")
        for cluster in clusters:
            for ihit in range(cluster.Hits_size()):
                hit = cluster.Hits(ihit)
                print(' Referenced hit has an energy of', hit.energy())
