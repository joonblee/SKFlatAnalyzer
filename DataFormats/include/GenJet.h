#ifndef GenJet_h
#define GenJet_h

#include "Particle.h"

class GenJet: public Particle {
public:

  GenJet();
  ~GenJet();

  //void SetArea(double area);

private:

  //double  j_area;

  ClassDef(GenJet,1)
};

#endif
