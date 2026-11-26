#ifndef NIsoMuonCutflow_h
#define NIsoMuonCutflow_h

#include <algorithm>
#include <cstddef>

namespace NIsoMuonCutflow {
// Return the last cumulative offline step of the jet-first cutflow.
template <typename Jets, typename Muons>
int CutflowStep(const Jets &jets, const Jets &tags, const Muons &muons,
                double tagDR, double muonDR, double leadPt, double pairMass,
                bool oppositeSign) {
  int step = 1;  // Trigger and event quality
  for (const auto &jet : jets) {
    for (const auto &other : jets) {
      if (jet.DeltaR(other) > tagDR) step = std::max(step, 2);
    }
    for (const auto &tag : tags) {
      if (!(jet.DeltaR(tag) > tagDR)) continue;
      step = std::max(step, 3);
      for (std::size_t i = 0; i < muons.size(); ++i) {
        if (!(muons[i].Pt() > leadPt)) continue;
        for (std::size_t j = i + 1; j < muons.size(); ++j) {
          if (oppositeSign && muons[i].Charge() * muons[j].Charge() >= 0) continue;
          step = std::max(step, 4);
          if (jet.DeltaR(muons[i]) < muonDR && jet.DeltaR(muons[j]) < muonDR &&
              (muons[i] + muons[j]).M() > pairMass) return 5;
        }
      }
    }
  }
  return step;
}
}

#endif
