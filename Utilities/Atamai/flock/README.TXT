Copyright 2001 Atamai Inc. 

Flock of Birds portable C API.

--------------------------------------------
This API provides the following features:

- a better way of using the flock than banging the serial port

- automatic recovery from phase errors (i.e. serial communication
  errors), usually only a single record is lost

- multithreaded streaming - the extra thread is there to catch
  the data records as they stream in.  This guards against 
  serial buffer overflows, reduces lag, and increases overall 
  performance.

- units are millimetres and radians - real scientists don't use
  inches and degrees

- a high- and low-level interface to the flock

- portability: Windows NT/2000/95/98/Millenium/XP
               Linux, IRIX
               porting to other UNIX variants is very straightforward

--------------------------------------------
This API does not (yet) support:

- extended-range transmitters (anyone want to lend me one?)

