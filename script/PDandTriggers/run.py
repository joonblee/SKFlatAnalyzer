import os

triggers = open('TriggersForCheck.txt').readlines()

PDs = ["SingleElectron", "SinglePhoton", "DoubleEG", "SingleMuon", "DoubleMuon"]

for trigger in triggers:
  trigger = trigger.strip('\n')

  print trigger+" : ",

  out = []
  for PD in PDs:
    lines = open(PD+'.txt').readlines()
    for line in lines:
      if trigger in line:
        out.append(PD)
        break

  print out