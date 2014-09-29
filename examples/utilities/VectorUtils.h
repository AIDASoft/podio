#ifndef UTILS_VECTOR_H
#define UTILS_VECTOR_H

class LorentzVector;
class TLorentzVector;

namespace utils {

  /// Converts a LorentzVector POD to a TLorentzVector
  TLorentzVector lvFromPOD(const LorentzVector& lv);

  /// Converts a TLorentzVector to a LorentzVector POD
  LorentzVector lvToPOD(const TLorentzVector& tlv);


}

#endif
