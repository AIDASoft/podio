#ifndef UTILS_VECTOR_H
#define UTILS_VECTOR_H

class LorentzVector;
class TLorentzVector;

namespace utils {

  /// Converts a LorentzVector POD to a TLorentzVector
  TLorentzVector lvFromPOD(const LorentzVector& lv);

  /// Converts a TLorentzVector to a LorentzVector POD
  LorentzVector lvToPOD(const TLorentzVector& tlv);

  /// difference eta1-eta2
  float deltaEta(const LorentzVector& lv1, const LorentzVector& lv2);

  /// difference phi1-phi2
  float deltaEta(const LorentzVector& lv1, const LorentzVector& lv2);

  /// dR2.
  ///
  /// Faster than deltaR because no call to sqrt
  float deltaR2(const LorentzVector& lv1, const LorentzVector& lv2);

  /// dR
  float deltaR(const LorentzVector& lv1, const LorentzVector& lv2);
}

#endif
