from AnalysisManager import AnalysisManager
import sys

currentPath = sys.path[0]+"/"

Ana=AnalysisManager(currentPath)
Ana.CreateOutTreePaths()
Ana.SetUpSamples()
Ana.SetQueHelper("NAFSL6")
Ana.PrepareTrees()
