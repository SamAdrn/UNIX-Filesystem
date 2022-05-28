/*
 * File: filesystem-datastructure.h
 *
 * This file contains the structure declarations necessary to create the file
 * system representation.
 * 
 * It takes advantage of the Linked List Data Structure model to create a list
 * of directories and files that would be assembeled together to create a 
 * Tree-like structure as the whole file system.
 * 
 * Author: Samuel Kosasih
 */

/*
 * These nodes are used to create a Linked List of Files
 */
typedef struct file_node
{

    /* The file's name */
    char *name;

    /* The timestamp of the file */
    int timestamp;

    /* A pointer to the next file in the list */
    struct file_node *next_file;

} File_node;

/*
 * These nodes are used to create a Linked List of Directories
 */
typedef struct dir_node
{

    /* The name of directory */
    char *name;

    /* String of the directory path */
    char *path;

    /* Head node of the file list */
    File_node *file_list;

    /* Head node of the subdirectory list */
    struct dir_node *subdir_list;

    /* A pointer to the next directory in the list */
    struct dir_node *next_dir;

    /* A pointer to the parent directory */
    struct dir_node *par_dir;

} Dir_node;

/*
 * These structures are used to create instances of a file system
 */
typedef struct FileSystem
{

    /* A pointer to keep a reference to the root of the filesystem */
    Dir_node *root;

    /* A pointer to keep a reference to the current directory */
    Dir_node *cur_dir;

} FileSystem;

/*
 * These structures are used to create linked lists of names.
 * Used for the purpose of sorting the names of files and subdirectories
 * of a directory for printing.
 */
typedef struct name_node
{
    /* The name to be stored in the node */
    char *name;

    /* The next node in the list */
    struct name_node *next_name;

} Name_node;