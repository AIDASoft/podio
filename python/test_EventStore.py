"""Unit tests for the EventStore class"""

from EventStore import EventStore

#pylint: disable=no-member # pylint does not realize that the store is populated by inheriting classes

class EventStoreBaseTestCase:
  """EventStore unit tests

  These define some tests that should work regardless of the backend that is
  used. In order to not have to duplicate this functionality for each backend,
  this base class defines the common tests and inheriting classes define a
  corresponding setUp method that sets up the correct EventStore and potentially
  additional backend specific functionality
  """
  def test_eventloop(self):
    self.assertTrue(len(self.store) >= 0)
    self.assertEqual(self.store.current_store.getEntries(),
                     len(self.store))
    for iev, event in enumerate(self.store):
      self.assertTrue(event is not None)
      if iev > 5:
        break

  def test_navigation(self):
    event0 = self.store[0]
    self.assertEqual(event0.__class__, self.store.__class__)

  def test_collections(self):
    evinfo = self.store.get("info")
    self.assertTrue(len(evinfo) > 0)
    particles = self.store.get("CollectionNotThere")
    self.assertFalse(particles)

  def test_read_only(self):
    hits = self.store.get("hits")
    # testing that one can't modify attributes in
    # read-only pods
    self.assertEqual(hits[0].energy(), 23.)
    hits[0].energy(10)
    self.assertEqual(hits[0].energy(), 10)  # oops

  def test_one_to_many(self):
    clusters = self.store.get("clusters")
    ref_hits = []
    # testing that cluster hits can be accessed and make sense
    for cluster in clusters:
      sume = 0
      for ihit in range(cluster.Hits_size()):
        hit = cluster.Hits(ihit)
        ref_hits.append(hit)
        sume += hit.energy()
      self.assertEqual(cluster.energy(), sume)
    hits = self.store.get("hits")
    # testing that the hits stored as a one to many relation
    # in the cluster can be found in the hit collection
    for hit in ref_hits:
      self.assertTrue(hit in hits)

  def test_relation_range(self):
    """Test that the RelationRange functionality is also accessible in python"""
    clusters = self.store.get("clusters")
    hits = self.store.get("hits")

    for cluster in clusters:
      sume = 0
      for hit in cluster.Hits():
        self.assertTrue(hit in hits)
        sume += hit.energy()
      self.assertEqual(cluster.energy(), sume)

  def test_hash(self):
    clusters = self.store.get("clusters")
    ref_hits = []
    # testing that cluster hits can be accessed and make sense
    for cluster in clusters:
      sume = 0
      for ihit in range(cluster.Hits_size()):
        hit = cluster.Hits(ihit)
        ref_hits.append(hit)
        sume += hit.energy()
      self.assertEqual(cluster.energy(), sume)
    hits = self.store.get("hits")
    if hits[0] == ref_hits[0]:
      self.assertEqual(hits[0].getObjectID().index,
                       ref_hits[0].getObjectID().index)
      self.assertEqual(hits[0].getObjectID().collectionID,
                       ref_hits[0].getObjectID().collectionID)
      self.assertEqual(hits[0].getObjectID(), ref_hits[0].getObjectID())
    # testing that the hits stored as a one to many relation
    # import pdb; pdb.set_trace()

  def test_context_managers(self):
    with EventStore([self.filename]) as store:
      self.assertTrue(len(store) >= 0)
      self.assertTrue(store.isValid())

# pylint: enable=no-member
