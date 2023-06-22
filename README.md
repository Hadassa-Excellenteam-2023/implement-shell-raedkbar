# Shell Program in C++

This is a simple shell program implemented in C++ that mimics the functionality of a standard Linux shell. It allows users to enter commands and arguments, which are then executed as subprocesses.

## Features

- Executes commands and arguments as subprocesses using `fork` and `exec` family of commands.
- Searches for the software in the paths listed in the PATH environment variable.
- Handles situations where the software is not found or if the user enters the full path of the software.
- Passes the arguments from the user to the software being executed.
- Supports running programs in the background by using the '&' symbol at the end of the command.
- Provides a built-in command `myjobs` to view details of background processes.
- Adjusts certain commands for Windows systems.

## Usage

1. Launch the shell program.
2. Enter commands and arguments as required.
3. Press Enter to execute the command.
4. To run a command in the background, append the '&' symbol at the end.
5. Use the `myjobs` command to view details of background processes.

## Examples

<pre>
<code>
$ shell
shell&gt; ls
&lt;output of the ls command&gt;
shell&gt; echo Hello, world!
Hello, world!
shell&gt; cat myfile.txt
&lt;contents of the myfile.txt&gt;
shell&gt; sleep 10 &amp;
[Background process started]
shell&gt; myjobs
PID: 1234 (sleep 10)
</code>
</pre>
