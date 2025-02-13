# FightBot_Simulation User Guide

## Overview

The FightBot_Simulation project allows two bots, one red and one blue, to compete based on their trained Q-tables. These Q-tables contain the strategies the bots use to make decisions in a game simulation. The following instructions will guide you through changing the Q-table filenames and configuring the simulation to load them correctly.

## Steps to Modify Q-table Filenames

### 1. Modify the Q-table Filenames in `Battle_Field.c`

To change the Q-tables that the simulation uses for the bots, locate the following lines in `Battle_Field.c`:

```c
const char* filename = "Q_table_blue_last.csv";
```

This line specifies the filename for the blue bot's Q-table. Change it to the desired Q-table file for the blue bot. For example:

```c
const char* filename = "Q_table_blue_custom.csv";
```

Next, locate the line for the red bot's Q-table:

```c
const char* filename1 = "Q_table_red_test2.csv";
```

Change this filename to the one you want for the red bot's Q-table:

```c
const char* filename1 = "Q_table_red_custom.csv";
```

### 2. Load the Q-tables

After changing the filenames, ensure that the Q-tables are loaded correctly. The Q-table loading function is likely already defined in your code, such as:

```c
loadQTableFromFile(filename);
```

This function should be used to load the Q-tables for both the red and blue bots. Make sure it is called after the filenames are set:

```c
loadQTableFromFile(filename);  // Blue bot
loadQTableFromFile(filename1); // Red bot
```

### 3. Save and Compile

After making the necessary changes, save your modifications in `Battle_Field.c`. Then, compile the project to ensure that the Q-tables are correctly loaded and used by the simulation.

Run the compilation command (example for GCC):

```bash
gcc -o FightBot_Simulation Battle_Field.c -lm
```

After compiling, execute the program and verify that the correct Q-tables are being used by observing the bot behaviors or through debug messages if available.

