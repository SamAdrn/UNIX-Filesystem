/*
 * File: filesystem.h
 *
 * This file contains the main function prototypes used for any file system
 * abstractions
 * 
 * Author: Samuel Kosasih
 */

#include "filesystem-datastructure.h"

void mkfs(FileSystem *const filesystem);
int touch(FileSystem *const filesystem, const char name[]);
int mkdir(FileSystem *const filesystem, const char name[]);
int cd(FileSystem *const filesystem, const char name[]);
int ls(FileSystem *const filesystem, const char name[]);
void pwd(FileSystem *const filesystem);
void rmfs(FileSystem *const filesystem);
int rm(FileSystem *const filesystem, const char name[]);