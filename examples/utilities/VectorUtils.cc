#include "VectorUtils.h"

#include "TLorentzVector.h"
#include "LorentzVector.h"

namespace utils {

  TLorentzVector lvFromPOD(const LorentzVector& lv) {
    TLorentzVector tlv; // COLIN need to fill it
    tlv.SetPtEtaPhiM( lv.Pt, lv.Eta, lv.Phi, lv.Mass);
    return tlv;
  }

  LorentzVector lvToPOD(const TLorentzVector& tlv) {
    LorentzVector lv; // COLIN need to fill it
    lv.Pt = tlv.Pt();
    lv.Eta = tlv.Eta();
    lv.Phi = tlv.Phi();
    lv.Mass = tlv.M();
    return lv;
  }

} // namespace
