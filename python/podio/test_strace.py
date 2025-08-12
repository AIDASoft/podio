#!/usr/bin/env python3
"""Create objects and collections to test strace output"""


from ROOT import ExampleHitCollection, ExampleHit, TestLink, TestLinkCollection, nsp
import podio

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

link_coll = TestLinkCollection()
link_coll.create()
link_coll[0].clone()
link = TestLink()
