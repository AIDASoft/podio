from eventstore import EventStore
import sys

if len(sys.argv)<2:
    print 'please specify input example rootfile from albers'
    sys.exit(1)
fnam = sys.argv[1]
store = EventStore(fnam)
nmax = 5
for iev, ev in enumerate(store):
    if iev==nmax:
        print "loop interrupted before end of file upon request."
        break
    evinfo = ev.get("EventInfo")
    print 'event', evinfo[0].Number()
    jets = ev.get("GenJet")
    assocs = ev.get("GenJetParticle")
    for jet in jets:
        print 'jet', jet.P4().Pt, jet.P4().Eta, jet.P4().Phi
        for assoc in assocs:
            jh = assoc.Jet()
            if jh == jet:
                ptc = assoc.Particle()
                print '\tparticle', ptc.ID(), ptc.P4().Pt, ptc.P4().Eta, ptc.P4().Phi
