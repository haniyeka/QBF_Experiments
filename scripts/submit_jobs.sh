#!/bin/bash
#SBATCH --chdir="directory path you want to use"
#SBATCH --account=mallet
#SBATCH --time=0-02:00:00
#SBATCH --job-name=createfiles
#SBATCH --output=./files.out
#SBATCH --error=./files.error
#SBATCH --partition=teton

module load miniconda3
module load python3-common

python sequential_test_v3-qbf.py Configuration_qbfexample
