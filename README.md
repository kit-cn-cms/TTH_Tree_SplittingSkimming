# TTH_Tree_SplittingSkimming
script to skim, remove branches and split our Trees
Only tested on NAF with sl6

Do:
1) create a list of branches to be kept in the output ntuples
Use getBranchList.py for that

2) Edit line https://github.com/kit-cn-cms/TTH_Analysis_Chain/blob/master/ParallelChain/Splitter.C#L83
   To point to your branchlist. If you put in an empty string it will do all branches
   And this is bad programming. I am Sorry

3) Edit Definitions.py
   Hopefully the infile comments suffice

4) Do 
   python OnlyPrepareTrees.py
