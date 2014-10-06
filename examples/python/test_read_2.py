import sys

from ROOT import gSystem
gSystem.Load("libDatamodelExample")
from ROOT import PyEventStore

filename = sys.argv[1]
store = PyEventStore(filename)

for i in range(10):
    particles = store.get("GenParticle");
    part = particles.get(0)
    print 'particles'
    print part.ID(), part.P4().Pt
    
    jetparticles = store.get("GenJetParticle")
    assoc = jetparticles.get(0)
    print 'association'
    print assoc.Jet().P4().Pt, assoc.Particle().ID()
    store.endOfEvent()
