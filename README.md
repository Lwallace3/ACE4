# ACE4

Your shell should support the following functionality:
1. Execution of external system programs including their parameters.

2. The following built-in commands:
a. cd – change working directory
b. getpath – print system path
c. setpath – set system path
d. history – print history contents (numbered list of commands in history
including their parameters in ascending order from least to most recent)
e. !! – invoke the last command from history (e.g. if your last command
was ‘ls –lF’ then ‘!!’ should execute ‘ls –lF’)
f. !<no> - invoke command with number <no> from history (e.g. !5 will
execute the command with number 5 from the history)
g. !-<no> - invoke the command with number the number of the current
command minus <no> (e.g. !-3 if the current command number is 5 will
execute the command with number 2, or !-1 will execute again the last
command)
h. alias – print all set aliases (alias plus aliased command)
i. alias <name> <command> - alias name to be the command. Note that
the command may also include any number of parameters, while (any
number of) command parameters should work correctly with aliasing
(e.g. if I alias la to be ls –la then when I type la . the shell should execute
ls –la .). Note also that aliasing should also work correctly with history
(e.g. !5 will execute the command with number 5 from the history if this
command is an alias like the la above then !5 will execute ls –la). In the
enhanced form of the alias, it should be possible to alias history
invocations (e.g. if I alias ‘five’ to be !5 then when I type ‘five’ the shell
should execute the command with number 5 from the history). It should
also be possible to alias aliases (e.g. If I alias l to be ls and then I alias la
to be l –a, then when I type la the shell should execute ls –a).
j. unalias <command> - remove any associated alias
  
3. Persistent history of user commands (save history in a file and load it when you run
the shell again)

4. Persistent aliases (save aliases in a file and load it when you run the shell again)
