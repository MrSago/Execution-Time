
# Get Execution Time

#### Syntax usage
> _et.exe -r [path to run interpreter] (optional) -f [path to file] -w [waiting time in ms]_

#### Examples
- Run python script with maximum waiting time is 1000 ms.
    ⚠️ Python interpreter must to be in _%PATH%_ and _greetings.py_ in _cwd_.
    >   _et.exe -r python -f greetings.py -w 1000_


- Run exe file, time limit is 500 ms.
    >   _et.exe -f calculate.exe -w 500_

🛑 At the moment you can't enter arguments in _your_ executable file (work in progess).

