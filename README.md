EpServerEngine
=======================
#### Visual C++ WinSock Server/Client Engine ####


DESCRIPTION
-----------

EpServerEngine is a Visual C++ software framework that supports the development of server/client application un a [MIT License](http://opensource.org/licenses/mit-license.php).
It handles all the initialize/usage WinSock library, which is time consuming task. 
EpServerEngine supports Visual C++ 2005/2008/2010/2012.
Source can be downloaded at [EpServerEngine](http://github.com/juhgiyo/epserverengine)


FEATURES
--------

* Easy to build server/client application.
  - Just implement Packet Structure and Packet Parsing routine for your server/client, 
       and EpServerEngine will do rest for you.
  - No more nasty initialization of WinSock and painful Winsock operation 
       to build server/client application.
  - No need to spend the time for handling thread synchronization.
* Supports variadic length packets using Packet Container
* Supports full ASCII/Unicode development environment


What is in the EpServerEngine?
------------------------------

* General
  1. Base Packet Parser
  2. Packet
  3. Packet Container
  4. Server Object List

* Client Side
  1. TCP
     * Base Client Class
  2. UDP
     * Base UDP Client Class

* Server Side
  1. TCP
     * Base Server
     * Base Server Worker
  2. UDP
     * Base UDP Server
     * Base UDP Server Worker


DOCUMENTATION
-------------

For detailed documentation, please download the full document [EpServerEngine Documentation.chm](https://s3.amazonaws.com/elasticbeanstalk-us-east-1-052504677345/Downloads/EpServerEngine/EpServerEngine+Documentation.chm).

If the documentation file is not visible after finishing download,
* Solution 1.
  1. Double-click the .chm file.
  2. In the Open File-Security Warning dialog box, click to clear the Always ask before opening this file check box.
  3. Click Open.
* Solution 2.
  1. Right-click the CHM file, and then click Properties.
  2. Click Unblock.
  3. Double-click the .chm file to open the file.


USAGE
-----

To find the usage examples, please see the [wiki page](https://github.com/juhgiyo/EpServerEngine/wiki)


WARNING
-------

Warning: This framework is not free from the 10K problem, since it is creating one thread per connection. (There are many frameworks out there which use IOCP to avoid the 10K problem.) This may be sufficient for small network where performance is NOT very important( which was most of my case).  


REFERENCE
---------
* [EpLibrary 2.0](https://github.com/juhgiyo/EpLibrary)


LICENSE
-------

[The MIT License](http://opensource.org/licenses/mit-license.php)

Copyright (c) 2012-2013 Woong Gyu La <[juhgiyo@gmail.com](mailto:juhgiyo@gmail.com)>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
