import ROOT
from array import array
from subprocess import call
from QueHelper import QueHelper
import time as timer
import sys
import json
import Definitions
import os
import Sample
from subprocess import check_output
import stat
import glob


class AnalysisManager:
    def __init__(self,currentPath):
      self.Path=currentPath
      self.Verbose=True
      self.Categories=Definitions.Categories
      self.OutputDirectoryForPreparedTrees=Definitions.OutputDirectoryForPreparedTrees
      
      self.MCSamplesDefinitions=Definitions.MCSamples
      self.DATASamplesDefinitions=Definitions.DataSamples
      self.MCSamples=[]
      self.DataSamples=[]
      self.MCinputDirectory=Definitions.MCinputDirectory
      self.DATAinputDirectory=Definitions.DATAinputDirectory
      self.JobIDList=[]
      self.JobScriptList=[]
      self.QueHelper=""
      self.SystematicTreeNames=Definitions.SystematicTreeNames

    def CreateOutTreePaths(self):
      if not os.path.exists(self.OutputDirectoryForPreparedTrees):
	os.makedirs(self.OutputDirectoryForPreparedTrees)
      for cat in self.Categories:
	catname=cat[0]
	if not os.path.exists(self.OutputDirectoryForPreparedTrees+'/Category_'+catname):
	  os.makedirs(self.OutputDirectoryForPreparedTrees+'/Category_'+catname)
	for split in ["Even","Odd"]:
	  if not os.path.exists(self.OutputDirectoryForPreparedTrees+'/Category_'+catname+'/'+split):
	    os.makedirs(self.OutputDirectoryForPreparedTrees+'/Category_'+catname+'/'+split)
      print "created directories for prepared Trees"
      
    def SetUpSamples(self):
      for mc in self.MCSamplesDefinitions:
	newsample=Sample.Sample()
	newsample.ProcessName=mc[0]
	newsample.SplitMode=mc[2]
	newsample.UseFlavorSplitting=mc[3]
	newsample.RateSystematics=mc[4]
	newsample.ShapeSystematics=mc[5]
	for ss in mc[1]:
	  inpath=self.MCinputDirectory+"/"+ss
	  #print ss, inpath
	  #indir=check_output(["dir","-1",inpath]).splitlines()
          #for rf in indir:
	    #if ".root" in rf and "nominal" in rf:
	      #newsample.ListOfRawInputTrees.append(inpath+"/"+rf)
	  newsample.ListOfRawInputTrees+=glob.glob(inpath)    
        print newsample.ListOfRawInputTrees
        raw_input()
        self.MCSamples.append(newsample)
        
      #for data in self.DATASamplesDefinitions:
	##print data
	#newsample=Sample.Sample()
	#newsample.ProcessName=data[0]
	#for ss in data[1]:
	  #inpath=self.DATAinputDirectory+"/"+ss
	  ##print inpath
	  #indir=check_output(["dir","-1",inpath]).splitlines()
          #for rf in indir:
	    ##if ".root" in rf and "nominal":
	    #if ".root" in rf:
	      #newsample.ListOfRawInputTrees.append(inpath+"/"+rf)
        ##print newsample.ListOfRawInputTrees
        #self.DataSamples.append(newsample)
      print "sat up intrees"
      
      
    def  SetQueHelper(self,quesystem):
      self.QueHelper=QueHelper(quesystem)
      
    def PrepareTrees(self):
      self.CompileAndSetupClientExecutable()
      mainrunline=self.QueHelper.GetRunLines()
      runlines=[]
      if not os.path.exists("PreparationScripts"):
	os.makedirs("PreparationScripts")
      testcount=0
      for sample in self.MCSamples:
	#print sample.ProcessName
	#print sample.ListOfRawInputTrees
	for i,f in enumerate(sample.ListOfRawInputTrees):
	  for syss in self.SystematicTreeNames:
	    if syss in f:
	      thisSys=syss
	  #testcount+=1
	  #if testcount>3:
	  #  break
	  outname=sample.ProcessName
	  joblines=[]
	  #print joblines
	  jj=self.QueHelper.GetExecLines()
	  for jjj in jj:
	    joblines.append(jjj)
  	  #print joblines
	  joblines.append("cd "+self.Path+"/PreparationScripts\n")
	  joblines.append("export myCONFIGFILE=PrepConfig_"+sample.ProcessName+"_"+str(i)+".txt\n")
	  joblines.append("./Splitter")
	  #print joblines
	  outfile=open("PreparationScripts/"+sample.ProcessName+"_"+str(i)+".sh","w")
	  for line in joblines:
	    outfile.write(line)
	  outfile.close()
	  st = os.stat("PreparationScripts/"+sample.ProcessName+"_"+str(i)+".sh")
	  os.chmod("PreparationScripts/"+sample.ProcessName+"_"+str(i)+".sh", st.st_mode | stat.S_IEXEC)
	  configlines=[]
	  configlines.append("INNAME "+f+"\n")
	  configlines.append("OUTNAME "+sample.ProcessName+"_"+str(i)+"\n")
	  configlines.append("OUTDIR "+self.OutputDirectoryForPreparedTrees+"\n")
	  for cat in self.Categories:
	    configlines.append("CATEGORY "+cat[0]+" "+cat[1]+"\n")
	  configlines.append("SPLITMODE "+sample.SplitMode+"\n")
	  configlines.append("FLAVORSPLITTING "+sample.UseFlavorSplitting+"\n")
	  configlines.append("SYSTEMATIC "+thisSys+"\n")

	  conf=open(self.Path+"/PreparationScripts/PrepConfig_"+sample.ProcessName+"_"+str(i)+".txt","w")
	  for ll in configlines:
	    conf.write(ll)
	  conf.close()
	  runlines=[]
	  thisrl=mainrunline[0]
	  runlines.append(thisrl)
	  #print runlines[-1]
	  runlines[-1]=runlines[-1].replace("INSERTPATHHERE",self.Path)
	  runlines[-1]=runlines[-1].replace("INSERTEXECSCRIPTHERE","PreparationScripts/"+sample.ProcessName+"_"+str(i)+".sh")
          runfile=open("runPrep.sh","w")
          for rl in runlines:
	    runfile.write(rl)
          runfile.close()
          st = os.stat("runPrep.sh")
          os.chmod("runPrep.sh", st.st_mode | stat.S_IEXEC)
          thisID=self.QueHelper.StartJob("./runPrep.sh")
          self.JobIDList.append(thisID)
          print "submitted ", "PreparationScripts/"+sample.ProcessName+"_"+str(i)+".sh"
        
        #now check the que until its finished
      JobsStillRunning=True
      nJobsStillRunning=len(self.JobIDList)
      while(JobsStillRunning):
	timer.sleep(30)
	nJobsStillRunning=0
	for job in self.JobIDList:
	  if self.QueHelper.GetIsJobRunning(job):
	    nJobsStillRunning+=1
	if nJobsStillRunning>0:
	  JobsStillRunning=True
	else:
	  JobsStillRunning=False
	print nJobsStillRunning, " jobs still running"
	  
        
	  
    def CompileAndSetupClientExecutable(self):
      print "Freshly Compiling Splitter.C" 
      call(["g++ -o PreparationScripts/Splitter Splitter.C `root-config --cflags --glibs` -lTMVA -lTreePlayer"],shell=True)
      print "done"
      
