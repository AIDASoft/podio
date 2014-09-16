#ifndef ReferencingData_H 
#define ReferencingData_H
#include "DummyDataHandle.h"

class ReferencingDataCollection;

class ReferencingData {
  friend ReferencingDataCollection;

public:
  const DummyDataHandle& DummyData() const { return m_DummyData;};

  void setDummyData(DummyDataHandle& value){ m_DummyData = value;};


private:
  DummyDataHandle m_DummyData;

};

#endif