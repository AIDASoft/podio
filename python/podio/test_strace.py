import podio
from ROOT import ExampleHitCollection, ExampleHit, nsp

coll = ExampleHitCollection()
coll.create()
coll[0].clone()
ExampleHit()
udc = podio.podio.UserDataCollection["int"]()
udc.create()

ncoll = nsp.EnergyInNamespaceCollection()
ncoll.create()
nhit = nsp.EnergyInNamespace()
nhit.clone()

