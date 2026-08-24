import os

def GetEventDone(l):
  w = l.split()[2]
  nums = w.split('/')

  if len(nums) < 2:
    print(nums)
    return "0:1"

  return str(nums[0]) + ':' + str(nums[1])

def GetJobID(logfiledir, cycle, jobnumber, hostname):
  jobid = ""
  return jobid

def GetLogLastLine(lines):
  index = -1
  for i in range(0, len(lines)):
    l = lines[len(lines) - 1 - i]
    if "LHAPDF" in l:
      continue
    elif "lhapdf" in l:
      continue
    elif "Eur.Phys.J." in l:
      continue
    else:
      return l

def IsIgnorableErrLine(line):
  stripped = line.strip()

  if stripped == "":
    return True

  ignorable_patterns = [
    "WARNING: Not mounting",
    "Following environment variables are going to be unset.",
    "PROJECT_MULTIARCH_TARGET",
    "IMPORTANT: Setting CMSSW environment to use 'x86-64-v2' target.",
  ]

  for pat in ignorable_patterns:
    if pat in line:
      return True

  return False

def CheckJobStatus(logfiledir, cycle, jobnumber, hostname):
  FinishString = "JOB FINISHED"

  path_log_e = ""
  path_log_o = ""

  if hostname == "KISTI" or hostname == "TAMSA1" or hostname == "TAMSA2" or hostname == "KNU":
    path_log_e = logfiledir + "/job_" + str(jobnumber) + ".err"
    path_log_o = logfiledir + "/job_" + str(jobnumber) + ".log"

  if (not os.path.exists(path_log_e)) or (not os.path.exists(path_log_o)):
    return "BATCH JOB NOT STARTED"

  with open(path_log_e) as f:
    log_e = f.readlines()

  filtered_log_e = []
  for e_l in log_e:
    if not IsIgnorableErrLine(e_l):
      filtered_log_e.append(e_l)

  if len(filtered_log_e) > 0:
    out = 'ERROR\n'
    out += '--------------------------------------\n'
    out += 'logfile : ' + path_log_o + '\n'
    out += '--------------------------------------\n'
    for l in filtered_log_e:
      out += l
    return out

  with open(path_log_o) as f:
    log_o = f.readlines()

  if len(log_o) == 0:
    return "BATCH LOG NOT CREATED"

  IsCycleRan = False
  for l in log_o:
    if "Processing " in l:
      IsCycleRan = True
      break

  if not IsCycleRan:
    return "ANALYZER NOT STARTED"

  LASTLINE = GetLogLastLine(log_o)

  if LASTLINE is None:
    return "BATCH LOG NOT CREATED"

  if "Processing run.C" in LASTLINE:
    return "EVENT NOT STARTED"

  if "Event Loop Started" in LASTLINE:
    return "EVENT NOT STARTED"

  line_JobStart = ""
  for l in log_o:
    if "Event Loop Started" in l:
      line_JobStart = l.replace("[SKFlatNtuple::Loop] Event Loop Started ", "")
      break

  ForTimeEst = LASTLINE

  if FinishString in LASTLINE:

    for i in range(0, len(log_o)):
      l = log_o[len(log_o) - 1 - i]
      if "[SKFlatNtuple::Loop RUNNING]" in l:
        ForTimeEst = l
        break

    line_JobFinished = LASTLINE.replace("[SKFlatNtuple::~SKFlatNtuple] JOB FINISHED ", "")
    EventDone = GetEventDone(ForTimeEst)
    return "FINISHED" + "\tEVDONE:" + EventDone + "\t" + line_JobStart + "\t" + line_JobFinished

  elif "Event Loop Started" in LASTLINE:
    return "RUNNING\t" + str(0) + "\tEVDONE:" + str(0) + "\t" + line_JobStart

  elif "[SKFlatNtuple::Loop RUNNING]" in LASTLINE:
    perct = LASTLINE.split()[3].strip('(')
    EventDone = GetEventDone(ForTimeEst)
    return "RUNNING\t" + perct + "\tEVDONE:" + EventDone + "\t" + line_JobStart

  else:
    for it_l in range(0, len(log_o)):
      l = log_o[len(log_o) - 1 - it_l]
      if ("[SKFlatNtuple::Loop RUNNING]" in l) and ("@" in l):
        perct = l.split()[3].strip('(')
        EventDone = GetEventDone(l)
        return "RUNNING\t" + perct + "\tEVDONE:" + EventDone + "\t" + line_JobStart

    return LASTLINE
