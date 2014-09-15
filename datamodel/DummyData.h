#ifndef DummyData_H 
#define DummyData_H


class DummyDataCollection;

class DummyData {
  friend DummyDataCollection;

public:
  const int& Number() const { return m_Number;};

  void setNumber(int& value){ m_Number = value;};


private:
  int m_Number;

};

#endif