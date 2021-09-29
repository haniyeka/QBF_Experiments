# -*- coding: utf-8 -*-
"""
Created on Mon Jun  1 16:39:54 2020
@author: Haniye Kashgarani
"""

#imports
import glob
import subprocess
import sys
import datetime
import os

#Version 3
#Haniye Kashgarani
'''
#sequential Test/ One Solver One Instance at a time
This program runs a sets of processes and reports the time they took to finish.
This program requires the following input file in order to work!

FILE FORMAT::
Solver Path
PartitionName
Directory of Test Instances
Name of instances
File Extension
Command Line Arguments for the solver
'''

def SubmitJob(SolverPath,SolverShort,Partition,Node,TestInstances,Path,Argument,InstanceName):
    '''
    Submits a test set to the cluster given the following inputs:
    SatSolver - What SatSolver to use
    Partition - What Partition to use
    Node - Specified nodes #Depricated
    TestInstances - A list of Instances to run tests on
    Path - Path to output folder
    Argument - Arguments to run with the given solver
    InstanceName - SAT2016-instances
    '''
    returnString = []

    pool = 0
    TestString = ""
    StartInstance = 0
    for i in range(len(TestInstances)):
        TestString = TestInstances[i]
        currentInstance = TestString.split('/')
        currentInstanceShort = currentInstance[len(currentInstance)-1][:-8] 
        print("Solver:"+SolverShort)
        print("CurrentInstance:"+currentInstanceShort)
        with  open (Path + "/jobs/" + "{}__{}.job".format(SolverShort,currentInstanceShort),"w") as file:
                  file.write('#!/bin/bash'+'\n')
                  file.write('#SBATCH --chdir="{}"'.format(os.getcwd() + '/' + Path)+ '\n')
                  file.write('#SBATCH --account=mallet'+'\n')
                  file.write('#SBATCH --time=0-01:23:20'+'\n')
                  file.write('#SBATCH --partition={}'.format(Partition)+'\n')
                  file.write('#SBATCH --job-name={}__{}'.format(SolverShort,currentInstanceShort)+ '\n')
                  file.write('#SBATCH --output=./{}__{}.output'.format(SolverShort,currentInstanceShort) + '\n')
                  file.write('#SBATCH --error=./{}__{}.error'.format(SolverShort,currentInstanceShort) + '\n')
                  file.write('#SBATCH --exclusive' + '\n')
                  file.write('module load gcc' + '\n')
                  file.write('module load parallel' + '\n')
                  file.write('time {} {} | tail -n 200'.format(SolverPath, TestString))
                  
        pool = 0
        TestString = "{ "
        subprocess.run(['sbatch', '{}/jobs/{}__{}.job'.format(Path,SolverShort,currentInstanceShort)])
        returnString.append("{NumOfNodes},{Instance},{TestNumber}".format(NumOfNodes=1,Instance=TestInstances[i],TestNumber=i))
        #print(returnString)
    return '\n'.join(returnString)

ConfigurationFileName = sys.argv[1]
Nodes = []

print("Starting up sat solvers based on the following configureation file {}".format(ConfigurationFileName))

#Get Data from the file provided at input
Data = ""
with open(ConfigurationFileName, "r") as file:
    Data = file.read()
    print(Data)

Data = Data.split('\n')

SolverPath = Data[0]
SolverShort = SolverPath.split("/")[-1]
Partition = Data[1]
InstancesDirectory = Data[2]
InstanceName = Data[3]
FileExtensions = Data[4].split(',')
Arguments = ""
print("Solver:", SolverPath)
print("Partition:", Partition)
print("Instance Directory:", InstancesDirectory)
print("Nodes :", Nodes)
print("CWD: ",os.getcwd())
for i in FileExtensions:
    TestInstances = glob.glob(InstancesDirectory+ "*" + i)
date = datetime.datetime.now()

log = []
Path = '{}-Partition-{}-Solver-{}-{}-NormalTest'.format(Partition,SolverShort,InstanceName,date.strftime("%B-%d-%Y")).replace('.','').replace('/','')
os.mkdir(Path)
os.mkdir(Path + "/jobs")

log.append(SubmitJob(SolverPath,SolverShort,Partition,1,TestInstances,Path,Arguments,InstanceName))


with open('RunLog-{}-Partition-{}-Solver-{}-{}-NormalTests.txt'.format(Partition,SolverShort,InstanceName,date.strftime("%B-%d-%Y")),"w") as file:
    file.write('\n'.join(log))
