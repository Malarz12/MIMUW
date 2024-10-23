
# Public transport simulation

This app was developed as an assigment during OOP couse as a part of the Computer Science Bachelor's Studies at the Faculty of Mathematics, Computer Science, and Mechanics, University of Warsaw.
In this project the main goal is simulating days in a public transport system.

## Overview
### How to run simulation
- download everything from this folder
- compile the project (with Intelij for example)
- run main

### Inputs
Data for the simulation will be read from standard input. The format of the data (text in angle brackets describes subsequent data, while text in parentheses describes their types):

```
<Number of simulation days (int)>
<Capacity of the stop (int)>
<Number of stops (int)>

#Now for each stop:
<Name of the stop (String)>
<Number of passengers (int)>
<Tram capacity (int)>
<Number of tram lines (int)>

#Now for each line (the lines are numbered with consecutive natural numbers):
<Number of trams on this line, route length (int, int)>

#Then the route in the form of pairs:
<stop name, travel time (string, int)>.
```
Example inputs can be found in the original task description [here](./Public-transport-task-description.pdf)

### Output
The simulation will print a description of every action and at the end there will be statistics regarding the whole simulation
