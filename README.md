## File pre-allocation for Node.js

Reserves space for a file to guarantee that subsequent writes will not fail due to lack of disk space and to reduce fragmentation.

Please be aware of what this module actually does on your OS and file system to know if it fits your use case.

#### Windows with NTFS

`SetFileInformationByHandle` is used to set the EndOfFile position. That guarantees that subsequent writes will not fail due to lack of disk space.

NTFS does not zero out data when calling this; the function returns immediately. Instead, when writing to a file at an offset, unused bytes between the current [valid data length](https://docs.microsoft.com/en-us/windows/desktop/api/fileapi/nf-fileapi-setfilevaliddata) and the offset are zeroed. That means that if you write the last byte of a file, and then write all of the beggining bytes of that file, the first write will also cause the file system to zero out the beginning of the file, i.e. it will be slower than if you had written the file sequentially. See [this article](https://blogs.msdn.microsoft.com/oldnewthing/20110922-00/?p=9573) for more information. There are three solutions proposed in that article:

  1. Manually change the valid data length. This is risky from a security perspective (uninitialized data exposure) but highly efficient. Requires a C interface. An equivalent feature was proposed for `fallocate(2)` and appears to have been accepted without being documented, despite a later attempt to revert it (https://lwn.net/Articles/492959/, https://lwn.net/Articles/528107/).
  2. Write a zero-byte at the end to move the delay, hopefully to a less time-sensitive part of your code. This is doable with Node.js' built-in `fs` module.
  3. Make the file sparse. This defeats both goals of this module.

I think this function can be used to shrink a file.

#### POSIX/Linux

[posix_fallocate(3)](http://man7.org/linux/man-pages/man3/posix_fallocate.3.html) is used. With file systems that support the `fallocate` operation (includes **ext4**, **xfs**, **btrfs**, **gfs2**, **ocfs2**), this efficiently reserves space without initializing the blocks. With file systems that do not, this slowly zeros out the space. (One way to check if your file system supports it is to run a test program with `strace`.)

I don't know exactly how these file systems compare to NTFS as far as behavior when writing to uninitialized space. There's info on ext4 in O_DIRECT mode [here](https://ext4.wiki.kernel.org/index.php/Clarifying_Direct_IO%27s_Semantics#Writes_into_preallocated_space). There's brief info on unwritten extents in xfs [here](http://xfs.org/docs/xfsdocs-xml-dev/XFS_User_Guide/tmp/en-US/html/ch02s07.html).

By comparison, `ftruncate` (which is available through Node.js' `fs` module) does not reserve disk space; subsequent write operations to that file are thus not guaranteed to not fail due to lack of disk space.

This function cannot be used to shrink a file.

#### OS X/Darwin

No support yet.

#### Other notes:

* Discussion about including fallocate in Node.js: https://groups.google.com/forum/#!topic/nodejs/sELcU_OTZLw

* Mozilla's OS-specific versions are reproduced here: 
https://stackoverflow.com/questions/11497567/fallocate-command-equivalent-in-os-x

* https://stackoverflow.com/a/23508718/1218408
