#include "fileutils.h"

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <error.h>
#include <string.h>

int tlog_is_absolute_path(const char *file_name)
{
    return IS_DIR_SEPARATOR(file_name[0]?1:0); 
}

char *tlog_path_skip_root(const char *file_name)
{
    /* Skip initial slashes */
    if (IS_DIR_SEPARATOR (file_name[0])) {
        while (IS_DIR_SEPARATOR (file_name[0])) {
            file_name++;
        }
        return (char *)file_name;
    }

    return NULL;
}

int tlog_file_test (const char *filename, TFileTest test)
{
    if((test & FILE_TEST_EXISTS)
        && (access(filename,F_OK)) == 0)
        return 1;

    if((test & FILE_TEST_IS_EXECUTABLE)
        && (access(filename,X_OK)) == 0)
    {
        if(getuid() != 0)
            return 1;
        else
            test &= ~FILE_TEST_IS_EXECUTABLE;
    }

    if(test & FILE_TEST_IS_SYMLINK)
    {
        struct stat s;
        if((lstat(filename,&s) == 0) && S_ISLNK(s.st_mode))
            return 1;
    }

    if(test & (FILE_TEST_IS_REGULAR |
               FILE_TEST_IS_DIR     |
               FILE_TEST_IS_EXECUTABLE))
    {
        struct stat s;
        
        if(stat(filename,&s) == 0)
        {
            if((test & FILE_TEST_IS_REGULAR) && S_ISREG(s.st_mode))
                return 1;

            if((test & FILE_TEST_IS_DIR) && S_ISDIR(s.st_mode))
                return 1;

            if((test & FILE_TEST_IS_EXECUTABLE) &&
                    ((s.st_mode & S_IXOTH) || 
                     (s.st_mode & S_IXUSR) ||
                     (s.st_mode & S_IXGRP)))
                return 1;
        
        }
    }
    return 0;
}

int tlog_mkdir_with_parents(const char *pathname,int mode)
{
    char *fn,*p;
    if(pathname == NULL || *pathname == '\0')
    {
        errno = EINVAL;
        return -1;
    }

    fn = strdup(pathname);

    if(tlog_is_absolute_path(fn))
        p = (char *)tlog_path_skip_root(fn);
    else
        p = fn;
    do{
        while(*p && !IS_DIR_SEPARATOR(*p)) p++;

        if(!*p)
            p = NULL;
        else
            *p = '\0';

        if(!tlog_file_test(fn,FILE_TEST_EXISTS))
        {
            if(mkdir(fn,mode) == -1 && errno != EEXIST)
            {
                int errno_save = errno;
                free(fn);
                errno = errno_save;
                return -1;
            }
        }else if (!tlog_file_test(fn,FILE_TEST_IS_DIR))
        {
            free(fn);
            errno = ENOTDIR;
            return -1;
        }

        if(p)
        {
            *p++ = DIR_SEPARATOR;
            while( *p && IS_DIR_SEPARATOR(*p)) p++;
        }
    }while(p);

    free(fn);
    return 0;
}
