#!/bin/bash
#SBATCH --job-name=graphCrawler_bench
#SBATCH --error=graphCrawler_%j.err
#SBATCH --time=05:00:00
#SBATCH --partition=Centaurus
#SBATCH --mem=10G

srun $HOMEGraphCrawler/graphCrawler Tom \Hanks 2
srun $HOMEGraphCrawler/graphCrawler Tom \Hanks 3
srun $HOMEGraphCrawler/graphCrawler Tom \Hanks 4
