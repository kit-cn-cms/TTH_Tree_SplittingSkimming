import subprocess
from subprocess import call
from subprocess import check_output
import os
import sys


class QueHelper:
  def __init__(self,RunSystem):
    self.RunSystem=RunSystem
    self.ExecLines=[]
    self.RunLines=[]
    
    # change if you want to use a different CMSSW Version
    cmssw=os.environ["CMSSW_BASE"]
    self.CMSSWBASE=cmssw
    print("\n----------------------------------------\n")
    print "using $CMSSW_BASE ",self.CMSSWBASE
    print "might not be the right choice, depending on your linux, target linux and CMSSW Version"
    print "changeable in QueHelper.py"
    
    if RunSystem=="EKPSL5":
      thisPortal=os.environ["HOSTNAME"]
      if thisPortal=="ekpcms5":
        thisque=os.environ["SGE_CLUSTER_NAME"]
        if thisque!="p_ogs1111":
          print "you need to setup the sl5 que first"
          print "source /opt/sge62/ekpcluster/common/settings.sh"
          exit(1)
      elif thisPortal=="ekpcms6":
        print "using sl5 que on ekpcms6 might lead to problems"
        thisque=os.environ["SGE_CLUSTER_NAME"]
        if thisque!="p_ogs1111":
          print "you need to setup the sl5 que first"
          print "source /opt/sge62/ekpcluster/common/settings.sh"
          exit(1)
      else:
        print "dont know this portal"
        exit(1)
      self.ExecLines=[
          "export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch\n",
          "export SCRAM_ARCH=slc5_amd64_gcc462\n",
          "source $VO_CMS_SW_DIR/cmsset_default.sh\n",
          "cd "+self.CMSSWBASE+"/src\n",
          "eval `scram runtime -sh`\n"
          ]
      self.RunLines=[
          "qsub -cwd -S /bin/bash -o INSERTPATHHERE/logs/\$JOB_NAME.o\$JOB_ID -e INSERTPATHHERE/logs/\$JOB_NAME.e\$JOB_ID -q 'medium' INSERTEXECSCRIPTHERE\n"
          ] 
    elif RunSystem=="EKPSL6":
      thisPortal=os.environ["HOSTNAME"]
      #print thisPortal
      if thisPortal!="ekpcms6":
        print "WARNING"
        print "you try to start jobs on the ekp SL6 que from ekpcms5"
        print "do it from ekpcms or manually change QueHelper.py"
      thisque=os.environ["SGE_CLUSTER_NAME"]
      if thisque!="p_ogs1111_sl6":
          print "you need to setup the sl6 que first"
          print "source /opt/ogs_sl6/ekpclusterSL6/common/settings.sh"
          exit(1)
      self.ExecLines=[
          "export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch\n",
          "export SCRAM_ARCH=slc6_amd64_gcc481\n",
          "source $VO_CMS_SW_DIR/cmsset_default.sh\n",
          "cd "+self.CMSSWBASE+"/src\n",
          "eval `scram runtime -sh`\n"
          ]
      self.RunLines=[
          "qsub -cwd -S /bin/bash -o INSERTPATHHERE/logs/\$JOB_NAME.o\$JOB_ID -e INSERTPATHHERE/logs/\$JOB_NAME.e\$JOB_ID -q 'medium' INSERTEXECSCRIPTHERE\n"
          ] 
    elif RunSystem=="NAFSL6":
      self.ExecLines=[
          "#!/bin/bash\n",
          ". /etc/profile.d/modules.sh\n",
          "module use -a /afs/desy.de/group/cms/modulefiles/\n",
          "module load cmssw/slc6_amd64_gcc491\n",
          "export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch\n",
          "export SCRAM_ARCH=slc6_amd64_gcc491\n",
          "source $VO_CMS_SW_DIR/cmsset_default.sh\n",
          "cd "+self.CMSSWBASE+"/src\n",
          "eval `scram runtime -sh`\n"
          ]
      self.RunLines=[
          "qsub -l h=bird* -hard -l os=sld6 -l h_vmem=4000M -l s_vmem=4000M -cwd -S /bin/bash -o INSERTPATHHERE/logs/\$JOB_NAME.o\$JOB_ID -e INSERTPATHHERE/logs/\$JOB_NAME.e\$JOB_ID -q 'default.q' INSERTEXECSCRIPTHERE\n"
          ] 
    elif RunSystem=="NAFSL5":
      self.ExecLines=[
          "#!/bin/bash",
          ". /etc/profile.d/modules.sh\n",
          "module use -a /afs/desy.de/group/cms/modulefiles/\n",
          "module load cmssw/slc5_amd64_gcc462\n",
          "export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch\n",
          "export SCRAM_ARCH=slc5_amd64_gcc462\n",
          "source $VO_CMS_SW_DIR/cmsset_default.sh\n",
          "cd "+self.CMSSWBASE+"/src\n",
          "eval `scram runtime -sh`\n"
          ]
      self.RunLines=[
          "qsub -l h=bird* -hard -l os=sld5 -l h_vmem=2000M -l s_vmem=2000M -cwd -S /bin/bash -o INSERTPATHHERE/logs/\$JOB_NAME.o\$JOB_ID -e INSERTPATHHERE/logs/\$JOB_NAME.e\$JOB_ID -q 'default.q' INSERTEXECSCRIPTHERE\n"
          ] 
    
    else:
      print "could not set up the batch system ", self.RunSystem
      print "check QueHelper.py"
      exit(1)
    print "set up QueHelper\n"
    
  def GetExecLines(self):
    return self.ExecLines
  
  def GetRunLines(self):
    return self.RunLines
  
  def StartJob(self,runScript):
    res=""
    try:
      res=check_output([runScript],shell=True)
    except (subprocess.CalledProcessError, OSError) :
      print "could not submit the job"
      exit(1)
      
    res=res.split()
    jid=0
    for r in res:
      if r.isdigit():
        jid=int(r)
        break
    return jid
    
  def GetIsJobRunning(self, jobID):
    bufferfile=open("tmp.txt","w")
    res=""
    try:
      res=check_output(["qstat","-j",str(jobID)],stderr=bufferfile)
    except (subprocess.CalledProcessError):
     res=""
    except:
      print "ERROR during qstat"
      print sys.exc_info()[0]
      exit(1)
    bufferfile.close()
    #print res
    if res!="":
      return True
    else:
      return False