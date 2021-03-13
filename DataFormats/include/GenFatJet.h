#ifndef GenFatJet_h
#define GenFatJet_h

#include "Particle.h"

class GenFatJet: public Particle {
public:

  GenFatJet();
  ~GenFatJet();

  //void SetArea(double area);

private:

  //double  j_area;

  ClassDef(GenFatJet,1)
};

#endif
