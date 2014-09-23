#ifndef EventInfo_H 
#define EventInfo_H


class EventInfoCollection;

class EventInfo {
  friend EventInfoCollection;

public:
  const int& Number() const { return m_Number;};

  void setNumber(int& value){ m_Number = value;};


private:
  int m_Number;

};

#endif