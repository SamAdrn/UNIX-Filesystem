# UNIX-Filesystem
## A UNIX Filesystem representation on C90

This project allows users to simulate a generic filesystem using some of the most basic UNIX commands. The whole program was written in ANSI C, as part of my university course that taught me how computer systems work.

This file system works just like any generic file system that you would find in any operating system (containing directories and files). It is based on a hierarchical structure, with the root directory being at the top of the pyramid. From the root, users are able to create new directories and files as much as the heap permits them to do so, with each directory containing more subdirectories and files. All directories can have an unlimited amount of descendants (although it is limited by the amount of memory you have), and all files are equipped with a timestamp. The hierarchy is maintained as a Tree structure, in which every node contains a linked list of both directories and files. 

A particular feature this representation offers is allowing the ability to create instances of more than one file system to exist during runtime. Before working on a filesystem, the `mkfs()` function must be called on the instantiated filesystem in order to initialize the required structure members and memory addresses before making any changes towards it. Each file system will be stored in its own individual virtual memory space, and performing any changes on one instance will not affect any others. Consequently, all functions must be called with a pointer to an instance of a file system as an argument. We use pointers here because it would be time consuming to create copies of every member within the file system everytime we call a function.

One limitation is that users must traverse through directories one-by-one (using the `cd()` function), unlike the UNIX implementation which allows traversal through multiple directories in one command. Aside from that, this implementation covers the rest of the major UNIX commands, such as `touch` (which is also used to make files), `mkdir`, `ls`, `pwd`, `rm`, and `rmfs`.

## Learning Points
- Enforced the understanding of **memory allocation**, since this project relies heavily on this concept. 
- Learned how to allocate memory efficiently, as well as deallocating them to **prevent memory leaks** when destroying a file system (since ANSI C does not have garbage collection).
- Gained a deeper understanding of the **hierarchical structure of a file system** (and how complicated it actually is versus how it seems).
- Getting to be more proficient in **using structures**, especially since I was in a situation where we can only access them through their memory addresses,
- Grasped the idea of **dereferencing** in ANSI C, as the program involves a lot of structure pointers that need to be passed through as arguments.

> Written [05/28/2022]
