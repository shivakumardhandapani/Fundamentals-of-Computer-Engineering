# Mobile Cloud Computing Task Scheduler

This project is a part of the course EECE7205 from Northeastern University by Professor Xue "Shelley" Lin in grad class of Fall 2023 and this project implements an energy-efficient and performance-aware task scheduling algorithm for mobile cloud computing (MCC) environments.

## Overview

The task scheduler aims to optimize the execution of applications on mobile devices by selectively offloading tasks to the cloud. It balances two key objectives:

1. Minimizing energy consumption on the mobile device
2. Meeting application completion time constraints

The algorithm works with task graphs representing applications and makes intelligent decisions about which tasks to execute locally vs offload to the cloud.

## Key Features

- Two-step scheduling algorithm:
  1. Initial scheduling to minimize completion time
  2. Task migration to reduce energy consumption
- Handles heterogeneous local cores and cloud resources
- Accounts for wireless communication overhead when offloading
- Linear-time rescheduling for efficient task migration
- Preserves task precedence requirements

## Algorithm Details

### Initial Scheduling

1. Primary assignment - Classify tasks as local or cloud based on execution time
2. Task prioritization - Sort tasks based on dependencies and weights  
3. Execution unit selection - Schedule tasks on cores/cloud to minimize finish time

### Task Migration 

- Iteratively moves tasks between cores or to cloud to reduce energy
- Uses efficient linear-time rescheduling algorithm
- Maintains completion time constraint

## Usage

The scheduler takes as input:

- Task graph with execution times on different cores
- Number of local cores
- Cloud parameters (transmission times, etc.)
- Maximum allowed completion time

It outputs:

- Final task schedule (local core assignments and cloud offloading)
- Total energy consumption
- Application completion time

## Results

Simulation results show the algorithm can reduce energy consumption by up to 3.1x compared to baseline approaches while meeting timing constraints.

## References

Lin, X., Wang, Y., Xie, Q., & Pedram, M. (2014). Energy and Performance-Aware Task Scheduling in a Mobile Cloud Computing Environment. 2014 IEEE 7th International Conference on Cloud Computing, 192-199.

# Note

This repository is for educational purposes and to showcase the work completed during the course.
