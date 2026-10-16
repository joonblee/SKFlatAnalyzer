#ifndef NIsoMuonPSWeights_h
#define NIsoMuonPSWeights_h

#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

namespace NIsoMuonPSWeights {

struct Variation {
  unsigned int index;
  const char *suffix;
};

// CMSSNU/SKFlatMaker parseLog.py selects FSR(0.5, 2), ISR(0.5, 2).
// In UL18 TTToSemiLeptonic these are GenEventInfo indices 4,5,26,27.
// SKFlatMaker divides each by gen_weight before writing weight_PSSyst.
// Up/Down below explicitly refers to mu_R, not to the radiation or yield.
static const Variation variations[] = {
  {0, "_Syst_PSFSRMuRDown"},
  {1, "_Syst_PSFSRMuRUp"},
  {2, "_Syst_PSISRMuRDown"},
  {3, "_Syst_PSISRMuRUp"}
};

inline void Validate(const std::vector<float> &weights) {
  if(weights.size() != 4)
    throw std::runtime_error("NIsoMuon: expected four weight_PSSyst entries");
  for(float weight : weights) {
    if(!std::isfinite(weight))
      throw std::runtime_error("NIsoMuon: non-finite weight_PSSyst entry");
  }
}

inline int Index(const std::string &suffix) {
  for(const auto &variation : variations) {
    if(suffix == variation.suffix) return variation.index;
  }
  return -1;
}

} // namespace NIsoMuonPSWeights
#endif
