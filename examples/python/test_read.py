import sys

from ROOT import TFile, TTree, gSystem

gSystem.Load("libDatamodelExample")
rootfile = TFile(sys.argv[1])

tree = rootfile.Get('events')
meta = rootfile.Get('metadata')
rootfile.ls()

class Registry(object):
    def __init__(self, metatree):
        self.entries = dict()
        for entry in meta:
            for id, name in zip(entry.m_collectionIDs, entry.m_names):
                self.entries[id] = name
        print self.entries


class Events(object):
    def __init__(self, tree):
        self.tree = tree

    def __iter__(self):
        iterator = iter(self.tree)
        print 'iterating'
        return iterator

registry = Registry(meta)

events = Events(tree)
for index, event in enumerate(events):
    if index > 10: break
    print index, event.EventInfo[0].Number


#for index, event in enumerate(tree):
#    import pdb; pdb.set_trace()
