/*
 * File: filesystem.c
 *
 * This file contains the source code that simulates the UNIX file system using
 * C code. Function prototypes are included from filesystem.h, which also
 * includes the required structure declarations in filesystem-datastructure.h.
 * 
 * Author: Samuel Kosasih
 */

/* -------------------- Include files -------------------- */
#include "filesystem.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* -------------------- Function Prototypes -------------------- */
static File_node *search_file(Dir_node *const dir, const char name[]);
static Dir_node *search_subdir(Dir_node *const dir, const char name[]);
static void print_whole_dir(Dir_node *const dir);
static Name_node *insert_name(Name_node *head, char *name, int is_dir);
static int search_and_remove_dir(Dir_node *const cur_dir, const char name[]);
static void remove_dir(Dir_node *dir);
static int search_and_remove_file(Dir_node *const cur_dir, const char name[]);
static void remove_file(File_node *file);

/* -------------------- Function Definitions -------------------- */

/*
 * Initializes the FileSystem parameter filesystem.
 * This allows more than one file systems to coexist, as each of them are
 * initialized to their own unique memory addresses.
 */
void mkfs(FileSystem *const filesystem)
{
    /* Create and initialize root directory */
    Dir_node *root = malloc(sizeof(*root));

    root->name = "root";
    root->path = "";
    root->file_list = NULL;
    root->subdir_list = NULL;
    root->next_dir = NULL;
    root->par_dir = NULL;

    /* Assign root directory to the filesystem */
    filesystem->root = root;
    filesystem->cur_dir = root;
}

/*
 * Creates a file with the specified name in the file system's current
 * directory.
 * - If a file with the same name already exists, then it simply
 *   updates the timestamp by incrementing it by 1.
 * - If a subdirectory with the same name exists, then it will not make any
 *   modifications.
 */
int touch(FileSystem *const filesystem, const char name[])
{
    File_node *cur, *prev = NULL, *new_file;
    char *new_name;
    int result = 0;

    /* Checks if parameters are valid, and also whether
    name consists of a forward-slash character */
    if (filesystem != NULL && name != NULL && strlen(name) != 0 
        && strchr(name, '/') == NULL)
    {
        result = 1;

        /* These names would cause the function to have no
        effects, but are not classified as error cases */
        if (strcmp(name, ".") != 0 && strcmp(name, "..") != 0 
            && strcmp(name, "/") != 0)
        {
            /* Searches subdirectories with the same name.
            If there is no directory with the same name, continue. */
            if (search_subdir(filesystem->cur_dir, name) == NULL)
            {
                /* Set cur to the head of the file
                list in the current directory */
                cur = filesystem->cur_dir->file_list;

                /* Find location to insert node within file_list */
                while (cur != NULL && strcmp(name, cur->name) >= 0)
                {
                    /* If there is a file of the same name,
                    increment timestamp and return 1*/
                    if (strcmp(name, cur->name) == 0)
                    {
                        cur->timestamp += 1;
                        return result;
                    }

                    prev = cur;
                    cur = cur->next_file;
                }

                /* Insert new File node */
                new_file = malloc(sizeof(*new_file));
                if (new_file != NULL)
                {
                    new_name = malloc(sizeof(char) * (strlen(name) + 1));
                    if (new_name != NULL)
                    {
                        /* Copies name to the allocated string */
                        strcpy(new_name, name);

                        /* Initializes new file structure members */
                        new_file->name = new_name;
                        new_file->timestamp = 1;
                        new_file->next_file = cur;

                        /* Case: File is inserted at the head */
                        if (prev == NULL)
                        {
                            filesystem->cur_dir->file_list = new_file;
                        }
                        /* Case: File is inserted elsewhere */
                        else
                        {
                            prev->next_file = new_file;
                        }
                    }
                }
            }
        }
    }

    return result;
}

/*
 * Creates a subdirectory with the specified name in the file system's
 * current directory.
 * - If a subdirectory/file with the same name already exists, or if name
 *   is invalid, then it will return 0.
 */
int mkdir(FileSystem *const filesystem, const char name[])
{
    Dir_node *cur, *prev = NULL, *new_dir;
    char *new_name;
    char *new_path;
    int result = 0;

    /* Checks if parameters are valid, and also whether name is an illegal
    special case in this context or it consists of a forward-slash character */
    if (filesystem != NULL && name != NULL && strlen(name) != 0 
        && strcmp(name, ".") != 0 && strcmp(name, "..") != 0 
        && strcmp(name, "/") != 0 && strchr(name, '/') == NULL)
    {
        /* Searches files with the same name.
        If there is no file with the same name, continue. */
        if (search_subdir(filesystem->cur_dir, name) == NULL)
        {

            result = 1;

            /* Set cur to the head of the
            subdirectory list of the current directory */
            cur = filesystem->cur_dir->subdir_list;

            /* Find location to insert node within subdir_list */
            while (cur != NULL && strcmp(name, cur->name) >= 0)
            {
                /* If there is a subdirectory of the same name,
                do nothing and return 1 */
                if (strcmp(name, cur->name) == 0)
                {
                    return result;
                }
                prev = cur;
                cur = cur->next_dir;
            }

            /* Insert new Subdirectory node */
            new_dir = malloc(sizeof(*new_dir));
            if (new_dir != NULL)
            {
                new_name = malloc(sizeof(char) * (strlen(name) + 1));
                if (new_name != NULL)
                {
                    new_path = malloc(sizeof(char) * (strlen(name) 
                                + strlen(filesystem->cur_dir->path) + 2));

                    if (new_path != NULL)
                    {
                        /* Copies name to the allocated name string */
                        strcpy(new_name, name);
                        /* Creates path name to the allocated path string */
                        strcpy(new_path, filesystem->cur_dir->path);
                        strcat(new_path, "/");
                        strcat(new_path, name);

                        /* Initializes new directory structure members */
                        new_dir->name = new_name;
                        new_dir->path = new_path;
                        new_dir->file_list = NULL;
                        new_dir->subdir_list = NULL;
                        new_dir->next_dir = cur;
                        new_dir->par_dir = filesystem->cur_dir;

                        /* Case: Directory is inserted at the head */
                        if (prev == NULL)
                        {
                            filesystem->cur_dir->subdir_list = new_dir;
                        }
                        /* Case: Directory is inserted elsewhere */
                        else
                        {
                            prev->next_dir = new_dir;
                        }
                    }
                }
            }
        }
    }

    return result;
}

/*
 * Moves the current directory of filesystem.
 * The directory to be moved to is specified by name using the parameter name.
 * - If name is a single period (.), there will be no effect since this tells
 *   the function to "move to the current directory."
 *   However, it is not an error case.
 * - If name is a double adjacent period (..), it will move the current
 *   directory to its parent directory.
 * - If name is solely a forward-slash (/), it will move the current
 *   directory to the root directory.
 * - If name is a valid name of an existing subdirectory, then it will move
 *   the current directory there.
 * - Other cases would be errors, including consists a forward-slash, but
 *   is not solely a forward-slash.
 */
int cd(FileSystem *const filesystem, const char name[])
{
    Dir_node *dir;
    int result = 0;

    /* Checks if parameters are valid */
    if (filesystem != NULL && name != NULL && strlen(name) != 0)
    {
        /* Move to current directory (no effect) */
        if (strcmp(".", name) == 0)
        {
            result = 1;
        }
        /* Move to parent directory */
        else if (strcmp("..", name) == 0)
        {
            /* If the current directory is the root directory, then its
            parent directory would be NULL, and therefore, should be
            prevented from crashing the program */
            if (filesystem->cur_dir->par_dir != NULL)
            {
                filesystem->cur_dir = filesystem->cur_dir->par_dir;
            }
            result = 1;
        }
        /* Move to root directory */
        else if (strcmp("/", name) == 0)
        {
            filesystem->cur_dir = filesystem->root;
            result = 1;
        }
        /* Move to an existing subdirectory */
        else
        {
            /* If name consists of a forward-slash, it is invalid */
            if (strchr(name, '/') == NULL)
            {
                /* Searches for subdirectories with the specified name,
                and sets the current directory to be inside it. If not
                found, then 0 will be returned. */
                dir = search_subdir(filesystem->cur_dir, name);
                if (dir != NULL)
                {
                    filesystem->cur_dir = dir;
                    result = 1;
                }
            }
        }
    }

    return result;
}

/*
 * Displays a subdirectory, a file, or a certain directory.
 * - If name is an existing file in the current directory, then it will print
 *   its name followed by a timestamp.
 * - If name is an existing subdirectory in the current directory, then it
 *   will print all the files and and subdirectories within it in
 *   lexicographic order.
 * - If name is a single period (.), or name is an empty string, it will print
 *   out the current directory.
 * - If name is a double adjacent period (..), it will print out the parent
 *   directory of the current directory.
 * - If name is a forward-slash (/), it will print out the root directory.
 * - Other cases would be errors, including consists a forward-slash, but
 *   is not solely a forward-slash.
 */
int ls(FileSystem *const filesystem, const char name[])
{
    Dir_node *dir;
    File_node *file;
    int result = 0;

    /* Checks if parameters are valid */
    if (filesystem != NULL && name != NULL)
    {

        /* Prints out current directory */
        if (strcmp(".", name) == 0 || strlen(name) == 0)
        {
            print_whole_dir(filesystem->cur_dir);
            result = 1;
        }
        /* Prints out parent directory */
        else if (strcmp("..", name) == 0)
        {
            /* If the current directory is the root directory, then its
            parent directory would be NULL, and therefore, should be
            prevented from crashing the program */
            if (filesystem->cur_dir->par_dir != NULL)
            {
                print_whole_dir(filesystem->cur_dir->par_dir);
            }
            result = 1;
        }
        /* Prints out root directory */
        else if (strcmp("/", name) == 0)
        {
            print_whole_dir(filesystem->root);
            result = 1;
        }
        /* Prints out an existing file or subdirectory */
        else
        {
            /* If name consists of a forward-slash, it is invalid */
            if (strchr(name, '/') == NULL)
            {
                /* Searches for a subdirectory with the specified name and
                prints it out. */
                dir = search_subdir(filesystem->cur_dir, name);
                if (dir != NULL)
                {
                    print_whole_dir(dir);
                    result = 1;
                }
                /* If a subdirectory is not found, then it will try searching
                for files with the specified name */
                else
                {
                    file = search_file(filesystem->cur_dir, name);
                    if (file != NULL)
                    {
                        printf("%s %d\n", file->name, file->timestamp);
                        result = 1;
                    }
                }
                /* If both are not found, then function will return 0 */
            }
        }
    }

    return result;
}

/*
 * Prints out the current directories full path, all the way from the root,
 * with each directory in between separated by forward-slashes.
 */
void pwd(FileSystem *const filesystem)
{
    /* Check if parameter is valid */
    if (filesystem != NULL)
    {
        /* If the current directory is the root directory, then print out
        a forward-slash. The root is initialized such that its path would
        be an empty string, to simplify the process of keeping track of
        other directory paths */
        if (strcmp(filesystem->cur_dir->path, "") == 0)
        {
            printf("/\n");
        }
        else
        {
            printf("%s\n", filesystem->cur_dir->path);
        }
    }
}

/*
 * Deallocates all dynamically-allocated memory that is being used by the
 * Ournix file system. As a result, all allocated memory of subdirectories
 * and files inside the root, and even the root itself, will be freed and
 * returned to the memory pool for future use. This also means that
 * whatever data the file system is holding will be removed.
 */
void rmfs(FileSystem *const filesystem)
{
    /* Checks if paramter is valid */
    if (filesystem != NULL)
    {
        remove_dir(filesystem->root);
    }
}

/*
 * Removes a file or subdirectory within the current directory. This also
 * means that whatever content is stored within them is also removed,
 * as any allocated memory being used within it freed and returned to the
 * memory pool.
 */
int rm(FileSystem *const filesystem, const char name[])
{
    int result = 0;

    /* Checks if parameters are valid, and also whether name is an illegal
    special case in this context or it consists of a forward-slash character */
    if (filesystem != NULL && name != NULL && strlen(name) != 0 
        && strcmp(name, ".") != 0 && strcmp(name, "..") != 0 
        && strcmp(name, "/") != 0 && strchr(name, '/') == NULL)
    {
        /* Tries removing a directory with the specified name */
        result = search_and_remove_dir(filesystem->cur_dir, name);

        /* If directory is not found, it will try
        removing a file with the specified name */
        if (!result)
        {
            result = search_and_remove_file(filesystem->cur_dir, name);
        }

        /* If both a directory or file is not
        found, then result would stay 0 */
    }

    return result;
}

/*
 * A helper function to search for a file with the specified name within
 * the specified directory.
 */
static File_node *search_file(Dir_node *const dir, const char name[])
{
    File_node *cur;

    /* Set cur to the head of the file
    list in the specified directory */
    cur = dir->file_list;

    /* Traverses the file list to search for the desired file */
    while (cur != NULL)
    {
        if (strcmp(cur->name, name) == 0)
        {
            return cur;
        }
        cur = cur->next_file;
    }

    /* If a file with the specified name is not found,
    then it will return NULL */
    return NULL;
}

/*
 * A helper function to search for a directory with the specified name within
 * the specified directory.
 */
static Dir_node *search_subdir(Dir_node *const dir, const char name[])
{
    Dir_node *cur;

    /* Set cur to the head of the subdirectory
    list in the specified directory */
    cur = dir->subdir_list;

    /* Traverses the subdirectory list to search for the
    desired subdirectory */
    while (cur != NULL)
    {
        if (strcmp(cur->name, name) == 0)
        {
            return cur;
        }
        cur = cur->next_dir;
    }

    /* If a subdirectory with the specified name is not found,
    then it will return NULL */
    return NULL;
}

/*
 * A helper function to print the contents of the whole specified directory.
 * To do this, the function takes two steps:
 * 1. It will create an ordered linked list (arranged in lexicographic order)
 *    storing the names of all files and subdirectories (if any) within the
 *    specified directory.
 * 2. Prints all the names being stored in the ordered linked list while
 *    deallocating any memory being used by it. We can safely do this since
 *    the linked list will be of no use for us in the future.
 */
static void print_whole_dir(Dir_node *const dir)
{
    Name_node *name_list = NULL, *printed_name;
    Dir_node *cur_dir;
    File_node *cur_file;

    /* Make an ordered Linked List of File/Directory names. */

    /* Traverse directory's files. */
    cur_file = dir->file_list;

    while (cur_file != NULL)
    {
        name_list = insert_name(name_list, cur_file->name, 0);
        cur_file = cur_file->next_file;
    }

    /* Traverse directory's subdirectories. */
    cur_dir = dir->subdir_list;

    while (cur_dir != NULL)
    {
        name_list = insert_name(name_list, cur_dir->name, 1);
        cur_dir = cur_dir->next_dir;
    }

    /* Start printing names. If the name_list is NULL, it indicates that
    the directory is empty, and therefore, nothing is printed. */
    while (name_list != NULL)
    {
        printf("%s\n", name_list->name);

        /* Set printed_name to the name that has just been printed
        and move name_list to the next node. */
        printed_name = name_list;
        name_list = name_list->next_name;

        /* Free allocated memory being used by printed_name. */
        free(printed_name->name);
        free(printed_name);
    }
}

/*
 * A helper function to assist the print_whole_dir() function in creating
 * an ordered linked list of names.
 * - The head parameter represents the head of the name linked list.
 * - The name parameter represents the name to be stored within the node.
 * - The is_dir parameter indicates whether the node being inserted will
 *   store the name of a directory. This distinction is important since
 *   directories are printed with a preceding forward-slash.
 */
static Name_node *insert_name(Name_node *head, char *name, int is_dir)
{
    Name_node *cur, *prev = NULL, *new_node;
    char *new_name;

    /* The boolean value flag is used to indicate whether an error has
    occured in the process of allocating memory, to prevent the function
    from executing more statements and causing the program to crash. */
    int flag = 1;

    /* Initialize new_name node. */
    new_node = malloc(sizeof(*new_node));

    if (new_node != NULL)
    {
        new_name = malloc(sizeof(char) * (strlen(name) + 2));
        if (new_name != NULL)
        {
            strcpy(new_name, name);

            /* If the name being inserted is the name of a directory,
            concatenate a forward-slash to the end of the string. */
            if (is_dir)
            {
                strcat(new_name, "/");
            }

            /* Initialize values of new_name node */
            new_node->name = new_name;
            new_node->next_name = NULL;
        }
        else
        {
            /* Malloc Error */
            flag = 0;
        }
    }
    else
    {
        /* Malloc Error */
        flag = 0;
    }

    /* Insert the new_name node into the linked list. */
    if (flag)
    {
        /* If head is NULL, it means that the node to be
        inserted is the first node in the linked list. */
        if (head == NULL)
        {
            return new_node;
        }

        cur = head;

        /* Find a location to insert the node */
        while (cur != NULL && strcmp(name, cur->name) >= 0)
        {
            prev = cur;
            cur = cur->next_name;
        }

        /* Initializes the pointer to the next node. */
        new_node->next_name = cur;

        /* Case: File is inserted at the head */
        if (prev == NULL)
        {
            head = new_node;
        }

        /* Case: File is inserted elsewhere */
        else
        {
            prev->next_name = new_node;
        }
    }

    return head;
}

/*
 * A helper method to search for a directory and remove it from the list
 * of subdirectories. This function is distinct from simply searching for
 * a directory using the search_dir() helper function and removing it using
 * the remove_dir() helper function as it also modifies the links within
 * the subdirectory linked list.
 */
static int search_and_remove_dir(Dir_node *const cur_dir, const char name[])
{
    Dir_node *cur, *prev = NULL;
    int result = 0;

    /* Traverses the subdirectory list to look for the desired subdirectory */
    cur = cur_dir->subdir_list;

    while (cur != NULL && strcmp(cur->name, name) != 0)
    {
        prev = cur;
        cur = cur->next_dir;
    }

    /* If cur is NULL, it would indicate that the subdirectory with the
    specified name is not found, and result would stay 0. */
    if (cur != NULL)
    {
        result = 1;

        /* Case: The subdirectory to be removed is the head of the list */
        if (prev == NULL)
        {
            cur_dir->subdir_list = cur->next_dir;
        }

        /* Case: The subdirectory to be removed is between the list */
        else
        {
            prev->next_dir = cur->next_dir;
        }

        /* Remove all subdirectory contents and free all
        allocated memory being used by it */
        remove_dir(cur);
    }

    return result;
}

/*
 * A recursive helper function used to remove all the contents within a
 * directory, which includes all files and subdirectories within it, and
 * frees all allocated memory being used by any of the content.
 */
static void remove_dir(Dir_node *dir)
{
    File_node *cur_file, *file_to_be_removed;
    Dir_node *cur_dir, *dir_to_be_removed;

    if (dir != NULL)
    {
        /* Remove all files within this directory */
        cur_file = dir->file_list;

        /* If cur_file is NULL, this would indicate
        that the directory has no files */
        while (cur_file != NULL)
        {
            file_to_be_removed = cur_file;
            cur_file = cur_file->next_file;

            remove_file(file_to_be_removed);
        }

        /* Remove all subdirectories within this directory */
        cur_dir = dir->subdir_list;

        /* If cur_dir is NULL, this would indicate
        that the directory has no subdirectories */
        while (cur_dir != NULL)
        {
            dir_to_be_removed = cur_dir;
            cur_dir = cur_dir->next_dir;

            remove_dir(dir_to_be_removed); /* Recursive call */
        }

        /* Free allocated memory being used
        by other directory struct members */
        if (strcmp("root", dir->name) != 0)
        {
            free(dir->name);
        }

        if (strlen(dir->path) != 0)
        {
            free(dir->path);
        }

        /* Free allocated memory being used by the directory itself */
        free(dir);
    }
}

/*
 * A helper method to search for a file and remove it from the list of files.
 * This function is distinct from simply searching for a file using the
 * search_file() helper function and removing it using the remove_file()
 * helper function as it also modifies the links within the file linked list.
 */
static int search_and_remove_file(Dir_node *const cur_dir, const char name[])
{
    File_node *cur, *prev = NULL;
    int result = 0;

    /* Traverses the file list to look for the desired file */
    cur = cur_dir->file_list;

    while (cur != NULL && strcmp(cur->name, name))
    {
        prev = cur;
        cur = cur->next_file;
    }

    /* If cur is NULL, it would indicate that the file with the
    specified name is not found, and result would stay 0. */
    if (cur != NULL)
    {
        result = 1;

        /* Case: The file to be removed is the head of the list */
        if (prev == NULL)
        {
            cur_dir->file_list = cur->next_file;
        }

        /* Case: The file to be removed is between the list */
        else
        {
            prev->next_file = cur->next_file;
        }

        /* Remove all file contents and free all
        allocated memory being used by it */
        remove_file(cur);
    }

    return result;
}

/*
 * A helper function used to remove all the contents within a
 * file, namely its name and timestamp.
 */
static void remove_file(File_node *file)
{
    if (file != NULL)
    {

        free(file->name);
        free(file);
    }
}