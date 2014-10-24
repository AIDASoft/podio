#include "VectorUtils.h"

#include "TLorentzVector.h"
#include "LorentzVector.h"

#include <cmath>

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

  float deltaEta(const LorentzVector& lv1, const LorentzVector& lv2) {
    return lv1.Eta - lv2.Eta;
  }

  float deltaPhi(const LorentzVector& lv1, const LorentzVector& lv2) {
    double result = lv1.Phi - lv2.Phi;
    while (result > M_PI) result -= 2*M_PI;
    while (result <= -M_PI) result += 2*M_PI;
    return result;
  }

  float deltaR2(const LorentzVector& lv1, const LorentzVector& lv2) {
    float dphi = deltaPhi(lv1, lv2);
    float deta = deltaEta(lv1, lv2);
    return dphi*dphi + deta*deta;
  }

  float deltaR(const LorentzVector& lv1, const LorentzVector& lv2) {
    return sqrt( deltaR2(lv1, lv2) );
  }







} // namespace
