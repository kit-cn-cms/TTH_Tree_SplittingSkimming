qsub -l h=bird* -hard -l os=sld6 -l h_vmem=4000M -l s_vmem=4000M -cwd -S /bin/bash -o /nfs/dust/cms/user/kelmorab/splittingSpring17/TTH_Analysis_Chain/ParallelChain//logs/\$JOB_NAME.o\$JOB_ID -e /nfs/dust/cms/user/kelmorab/splittingSpring17/TTH_Analysis_Chain/ParallelChain//logs/\$JOB_NAME.e\$JOB_ID -q 'default.q' PreparationScripts/ttHbb_15.sh
