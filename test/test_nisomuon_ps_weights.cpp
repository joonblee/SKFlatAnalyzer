// Standalone check: g++ -std=c++11 -IAnalyzers/include
//   test/test_nisomuon_ps_weights.cpp -o /tmp/test_ps && /tmp/test_ps
#include "NIsoMuonPSWeights.h"
#include <cassert>
#include <limits>

int main() {
  using namespace NIsoMuonPSWeights;
  // Independent expected order from the UL18 TTToSemiLeptonic producer log.
  assert(Index("_Syst_PSFSRMuRDown") == 0);
  assert(Index("_Syst_PSFSRMuRUp") == 1);
  assert(Index("_Syst_PSISRMuRDown") == 2);
  assert(Index("_Syst_PSISRMuRUp") == 3);
  assert(Index("") == -1);
  assert(Index("_Syst_PSNominal") == -1);
  assert(Index("_Syst_BTagHFUncorrDown") == -1);

  // Large, zero and signed finite values are not clipped or renormalised.
  Validate({0.f, -1.f, 1.849f, 50.f});
  for(const auto &bad : std::vector<std::vector<float>>{
        {}, {1.f}, {1.f, 1.f, 1.f}, {1.f, 1.f, 1.f, 1.f, 1.f},
        {1.f, 1.f, std::numeric_limits<float>::quiet_NaN(), 1.f},
        {1.f, 1.f, 1.f, std::numeric_limits<float>::infinity()}}) {
    bool rejected = false;
    try { Validate(bad); }
    catch(const std::runtime_error &) { rejected = true; }
    assert(rejected);
  }
}
