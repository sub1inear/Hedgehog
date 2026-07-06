/*
The MIT License (MIT)

Copyright (c) 2021, Nauja

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
*/

#ifndef FS_H
#define FS_H

#ifdef __cplusplus
extern "C"
{
#endif

/** Major version of fs. */
#define FS_VERSION_MAJOR 0
/** Minor version of fs. */
#define FS_VERSION_MINOR 2
/** Patch version of fs. */
#define FS_VERSION_PATCH 3

#if !defined(FS_MALLOC) && !defined(FS_FREE)
/**
 * Defines the malloc function used by fs at compile time.
 *
 * @code
 * void* my_malloc(size_t size)
 * {
 *     // do something
 * }
 *
 * #define FS_MALLOC my_malloc
 * @endcode
 */
#define FS_MALLOC malloc
/**
 * Defines the free function used by fs at compile time.
 *
 * @code
 * void my_free(void* ptr)
 * {
 *     // do something
 * }
 *
 * #define FS_FREE my_free
 * @endcode
 */
#define FS_FREE free
#endif

#ifdef _WIN32
#define FS_WINDOWS
#define FS_MAX_PATH 260
#elif defined(__linux__)
#define FS_LINUX
#define FS_POSIX
#define FS_MAX_PATH 4096
#elif defined(__APPLE__) && defined(__MACH__)
#define FS_MACOS
#define FS_POSIX
#define FS_MAX_PATH 1024
#else
#error "unsupported OS"
#endif


#include <stddef.h>    // for size_t
#include <stdbool.h>   // for bool
#include <sys/types.h> // for off_t


/**
    * Composes an absolute path, resolving symlinks.
    * Due to `realpath` limitations, `buf` must be at least `FS_MAX_PATH` bytes long.
    *
    * @code{.c}
    * char buf[FS_MAX_PATH];
    * if (!fs_absolute("./relative", buf))
    * {
    *     print("fs_absolute failed");
    * }
    * else
    * {
    *     printf("%s", buf);
    * }
    * @endcode
    *
    * @param[in] path Some null-terminated path
    * @param[out] buf Buffer for storing the result path. Must be at least `FS_MAX_PATH` bytes long
    * @return A pointer to buf if there is no error, NULL otherwise.
    */
char *
fs_absolute(const char *path, char buf[FS_MAX_PATH]);

/**
    * Gets a pointer to the rightmost path separator.
    *
    * @code{.c}
    * char* c = fs_rsplit("./path/to/foo.txt");
    * @endcode
    *
    * @param[in] path Some null-terminated path
    * @return A pointer to the rightmost separator.
    */
char *
fs_rsplit(const char* path);

/**
    * Gets the directory name of path.
    *
    * @code{.c}
    * char dirname[FS_MAX_PATH];
    * fs_dirname("./foo.txt", dirname, sizeof(dirname));
    * @endcode
    *
    * @param[in] path Some null-terminated path
    * @param[out] buf Buffer for storing the result path
    * @param[in] size Buffer size
    * @return The number of bytes written to buf, < 0 if there was an error, or >= size if the result was truncated.
    */
size_t
fs_dirname(const char *path, char *buf, size_t size);

/**
    * Gets the base name of path.
    *
    * @code{.c}
    * char *basename = fs_basename("./foo.txt");
    * @endcode
    *
    * @param[in] path Some null-terminated path
    * @return A pointer to the base name.
    */
char *
fs_basename(const char* path);

/**
    * Gets the extension of path.
    *
    * @code{.c}
    * char *ext = fs_extention("./foo.txt");
    * @endcode
    *
    * @param[in] path Some null-terminated path
    * @return A pointer to the extension, or NULL if there is no extension.
    */
char *
fs_extention(const char *path);

/**
    * Copies files, preserving permissions.
    *
    * @code{.c}
    * fs_copy("foo.txt", "bar.txt");
    * @endcode
    *
    * @param[in] from Some null-terminated path to the source file. Must exist
    * @param[in] to Some null-terminated path to the destination file. Can not exist (will be created)
    * @return true if the copy was successful, false otherwise.
    */
bool
fs_copy(const char *from, const char *to);

/**
    * Get the current working directory.
    *
    * @code{.c}
    * char buf[FS_MAX_PATH];
    * if (!fs_current_dir(buf, sizeof(buf)))
    * {
    *     print("fs_current_dir failed");
    * }
    * else
    * {
    *     printf("%s", buf);
    * }
    * @endcode
    *
    * @param[out] buf Buffer for storing the result path
    * @param[in] size Buffer size
    * @return A pointer to buf if there is no error, NULL otherwise.
    */
char *
fs_current_dir(char *buf, size_t size);

/**
    * Get the path of the currently running executable, following symlinks if necessary.
    * Due to `realpath` limitations, `buf` must be at least `FS_MAX_PATH` bytes long.
    *
    * @code{.c}
    * char buf[FS_MAX_PATH];
    * if (!fs_exec_path(buf))
    * {
    *     print("fs_exec_path failed");
    * }
    * else
    * {
    *     printf("%s", buf);
    * }
    * @endcode
    *
    * @param[out] buf Buffer for storing the result path. Must be at least `FS_MAX_PATH` bytes long
    * @return A pointer to buf if there is no error, NULL otherwise.
    */
char *
fs_exec_path(char buf[FS_MAX_PATH]);

/**
    * Concatenates two paths together with the platform specific separator.
    *
    * @code{.c}
    * char cwd[FS_MAX_PATH];
    * if (!fs_current_dir(cwd, sizeof(buf)))
    * {
    *     print("fs_current_dir failed");
    *     return;
    * }
    *
    * char buf[FS_MAX_PATH];
    * fs_join_path(buf, sizeof(buf), cwd, "foo.txt");
    * printf("%s", buf);
    * @endcode
    *
    * @param[out] buf Buffer for storing the result path
    * @param[in] size Buffer size
    * @param[in] left Left part null-terminated path
    * @param[in] right Right part null-terminated path
    * @return The number of bytes written to buf, < 0 if there was an error, or >= size if the result was truncated.
    */
int
fs_join_path(char *buf, size_t size, const char *left, const char *right);

/**
    * Checks if a path corresponds to an existing file or directory.
    *
    * @code{.c}
    * if (!fs_exist("./foo.txt"))
    * {
    *     print("foo.txt not found");
    * }
    * else
    * {
    *     printf("foo.txt found");
    * }
    * @endcode
    *
    * @param[in] path Some null-terminated path
    * @return If the file or directory exists.
    */
bool
fs_exist(const char *path);

/**
    * Gets the size of an existing file.
    *
    * @code{.c}
    * off_t size = fs_file_size("foo.txt")
    * printf("file size: %d", size);
    * @endcode
    *
    * @param[in] path Some null-terminated path
    * @return The size of the file, in bytes, or -1 for an error.
    */
off_t
fs_file_size(const char *path);

/**
    * Checks if a path corresponds to a directory.
    *
    * @code{.c}
    * if (!fs_is_dir("./somedirectory"))
    * {
    *     print("path is not a directory");
    * }
    * else
    * {
    *     print("path is a directory");
    * }
    * @endcode
    *
    * @param[in] path Some null-terminated path
    * @return If path points to an existing directory.
    */
bool
fs_is_dir(const char *path);

/**
    * Checks if a path corresponds to a file.
    *
    * @code{.c}
    * if (!fs_is_file("./foo.txt"))
    * {
    *     print("path is not a file");
    * }
    * else
    * {
    *     print("path is a file");
    * }
    * @endcode
    *
    * @param[in] path Some null-terminated path
    * @return If path points to an existing file.
    */
bool
fs_is_file(const char *path);

/**
    * Checks if a path corresponds to a symbolic link.
    *
    * @code{.c}
    * if (!fs_is_symlink("./somesymlink"))
    * {
    *     print("path is not a symbolic link");
    * }
    * else
    * {
    *     print("path is a symbolic link");
    * }
    * @endcode
    *
    * @param[in] path Some null-terminated path
    * @return If path points to an existing symbolic link.
    */
bool
fs_is_symlink(const char *path);

/**
    * Writes file content to buffer.
    *
    * @code{.c}
    * void buf[1024];
    * fs_read_file("foo.txt", buf, 1024);
    * @endcode
    *
    * @param[in] path Some null-terminated path to existing file
    * @param[in] buf Some memory buffer
    * @param[in] size Buffer size
    * @return The number of bytes that would have been readen if
    * buf was large enough (excluding the null-terminating character).
    */
size_t
fs_read_file_buffer(const char *path, void *buf, size_t size);

/**
    * Reads a whole file content.
    *
    * @code{.c}
    * int size;
    * void* buf;
    * if (!(buf = fs_read_file("foo.txt", &size)))
    * {
    *     printf("fs_read_file failed");
    * }
    * else
    * {
    *     printf("file size: %d", size);
    * }
    * @endcode
    *
    * @param[in] path Some null-terminated path to existing file
    * @param[out] size Number of bytes read
    * @return A pointer to read bytes if there is no error, NULL otherwise.
    */
void *
fs_read_file(const char *path, size_t *size);

/**
    * Writes content to file.
    *
    * @code{.c}
    * const char* buf = "hello";
    * if (!fs_write_file("foo.txt", buf, 5))
    * {
    *     printf("fs_write_file failed");
    * }
    * @endcode
    *
    * @param[in] path Some null-terminated path
    * @param[in] buf Some memory buffer
    * @param[in] size Buffer size
    * @return If the file was written.
    */
bool
fs_write_file(const char *path, const void *buf, size_t size);

/**
    * @struct fs_file_iterator
    * Struct used to iterate over a file.
    *
    * @code{.c}
    * struct fs_file_iterator* it = fs_iter_file("foo.txt");
    * char c;
    *
    * while(fs_next_char(it, &c))
    * {
    *     printf("%c", c);
    * }
    *
    * fs_close_file(it);
    * @endcode
    */
struct fs_file_iterator;

/**
    * Opens a file to iterate over its content.
    *
    * @code{.c}
    * struct fs_file_iterator* it = fs_iter_file("foo.txt");
    *
    * // iterate file
    *
    * fs_close_file(it);
    * @endcode
    *
    * @param[in] path Some null-terminated path
    * @return A pointer for iterating over the file if there is no error,
    * NULL otherwise.
    */
struct fs_file_iterator *
fs_iter_file(const char *path);

/**
    * Iterates over the next char of a file.
    *
    * @code{.c}
    * char c;
    * while(fs_next_char(it, &c))
    * {
    *     printf("%c", c);
    * }
    * @endcode
    *
    * @param[in] it Some opened file iterator
    * @param[out] c Character read
    * @return The same it pointer or NULL if an error occurred or there
    * is no more entry to iterate over.
    */
struct fs_file_iterator *
fs_next_char(struct fs_file_iterator *it, char *c);

/**
    * Closes and frees an opened file iterator.
    *
    * @code{.c}
    * struct fs_file_iterator* it = fs_iter_file("foo.txt");
    *
    * // iterate file
    *
    * fs_close_file(it);
    * @endcode
    *
    * @param[in] it Some opened file iterator
    */
void
fs_close_file(struct fs_file_iterator *it);

/**
    * Gets the absolute path to the platform specific temporary directory.
    *
    * @code{.c}
    * char buf[MAX_PATH];
    * if (!fs_temp_dir(buf, MAX_PATH))
    * {
    *     printf("fs_temp_dir failed");
    * }
    * else
    * {
    *     printf("%s", buf);
    * }
    * @endcode
    *
    * @param[in] buf Buffer for storing the result path
    * @param[in] size Buffer size
    * @return A pointer to buf if there is no error, NULL otherwise.
    */
char *
fs_temp_dir(char *buf, size_t size);

/**
    * Deletes an empty directory if it exists.
    *
    * @code
    * if (!fs_delete_dir("foo"))
    * {
    *     printf("fs_delete_dir failed");
    * }
    * @endcode
    * @param[in] path Some null-terminated path
    * @return If the directory was deleted.
    */
bool
fs_delete_dir(const char *path);

/**
    * Deletes a file if it exists.
    *
    * @code
    * if (!fs_delete_file("foo.txt"))
    * {
    *     printf("fs_delete_file failed");
    * }
    * @endcode
    * @param[in] path Some null-terminated path
    * @return If the file was deleted.
    */
bool
fs_delete_file(const char *path);

/**
    * Creates a directory if it doesn't exist.
    *
    * The parent directory must exist as it will not
    * be created recursively.
    *
    * @code
    * if (!fs_make_dir("foo"))
    * {
    *     printf("fs_make_dir failed");
    * }
    * @endcode
    * @param[in] path Some null-terminated path
    * @return If the directory was created.
    */
bool
fs_make_dir(const char *path);

/**
    * Struct used to iterate over a directory.
    *
    * @code{.c}
    * struct fs_directory_iterator* it = fs_open_dir("./somedir");
    *
    * while(fs_read_dir(it))
    * {
    *     printf("%s", it->path);
    * }
    *
    * fs_close_dir(it);
    * @endcode
    */
struct fs_directory_iterator
{
    /** Path to file. */
    const char *path;
};

/**
    * Gets an iterator over entries of a directory.
    *
    * @code{.c}
    * struct fs_directory_iterator* it = fs_open_dir("./somedir");
    *
    * while(fs_read_dir(it))
    * {
    *     printf("%s", it->path);
    * }
    *
    * fs_close_dir(it);
    * @endcode
    *
    * @param[in] path Some null-terminated path
    * @return A pointer for iterating over the directory if there is no error, NULL otherwise.
    */
struct fs_directory_iterator *
fs_open_dir(const char *path);

/**
    * Iterates over the next entry of a directory.
    *
    * @code{.c}
    * struct fs_directory_iterator* it = fs_open_dir("./somedir");
    *
    * while(fs_read_dir(it))
    * {
    *     printf("%s", it->path);
    * }
    *
    * fs_close_dir(it);
    * @endcode
    *
    * @param[in] it Some opened directory iterator
    * @return The same it pointer or NULL if an error occurred or there is no more entry to iterate over.
    */
struct fs_directory_iterator *
fs_read_dir(struct fs_directory_iterator *it);

/**
    * Closes and frees an opened directory iterator.
    *
    * @code{.c}
    * struct fs_directory_iterator* it = fs_open_dir("./somedir");
    *
    * while(fs_read_dir(it))
    * {
    *     printf("%s", it->path);
    * }
    *
    * fs_close_dir(it);
    * @endcode
    *
    * @param[in] it Some opened directory iterator
    */
void
fs_close_dir(struct fs_directory_iterator *it);

#ifdef __cplusplus
}
#endif

#ifdef FS_IMPLEMENTATION

#ifdef FS_WINDOWS
#include <direct.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winioctl.h> // for FSCTL_GET_REPARSE_POINT
#include <aclapi.h> // for GetNamedSecurityInfo and SetNamedSecurityInfo
#include <strsafe.h>

#ifndef S_ISDIR
#define S_ISDIR(x) ((x & _S_IFDIR) == _S_IFDIR)
#endif
#ifndef S_ISREG
#define S_ISREG(x) ((x & _S_IFREG) == _S_IFREG)
#endif

#elif defined(FS_POSIX)
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#ifdef FS_MACOS
#include <mach-o/dyld.h>
#include <copyfile.h>
#endif

#endif

#if defined(FS_WINDOWS) || defined(FS_POSIX)
#include <sys/stat.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FS_MKDIR_PERMISSIONS 0700

typedef struct fs_directory_iterator fs_directory_iterator;

char *
fs_rsplit(const char *path)
{
	char *c1 = strrchr(path, '/');
	if (!c1)
	{
		return strrchr(path, '\\');
	}

	char *c2 = strrchr(c1, '\\');
	return c2 ? c2 : c1;
}

size_t
fs_dirname(const char *path, char *buf, size_t size)
{
	char *c = fs_rsplit(path);
	if (!c)
	{
		snprintf(buf, size, "%s", "");
		return 0;
	}

	return snprintf(buf, size, "%.*s", (int)(c - path), path);
}

char *
fs_basename(const char *path)
{
	char *c = fs_rsplit(path);
	if (!c)
	{
		return (char *)path;
	}

	return c + 1;
}

char *
fs_extention(const char *path)
{
    char *c = strrchr(path, '.');
    if (!c)
    {
        return NULL;
    }
    return c;
}

#ifdef FS_WINDOWS
char *
fs_absolute(const char *path, char *buf)
{
    HANDLE h = CreateFileA(
        path,
        0,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );
    if (h == INVALID_HANDLE_VALUE)
    {
        return NULL;
    }
    DWORD len = GetFinalPathNameByHandleA(h, buf, FS_MAX_PATH, FILE_NAME_NORMALIZED | VOLUME_NAME_DOS);
    CloseHandle(h);
    if (len == 0 || len >= FS_MAX_PATH)
    {
        return NULL;
    }
    // remove "\\?\" prefix
    memmove(buf, buf + 4, len - 3);
    return buf;
}
#elif defined(FS_POSIX)
char *
fs_absolute(const char *path, char *buf)
{
	if (!realpath(path, buf))
	{
		return NULL;
	}

	return buf;
}
#endif

#ifdef FS_WINDOWS
bool
fs_copy(const char *from, const char *to)
{
    if (!CopyFileA(from, to, false))
    {
        return false;
    }

    // copy permissions
    PACL dacl = NULL;
    PSECURITY_DESCRIPTOR sd = NULL;
    DWORD err = GetNamedSecurityInfoA(
        from,
        SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION,
        NULL,
        NULL,
        &dacl,
        NULL,
        &sd
    );

    if (err != ERROR_SUCCESS)
    {
        return false;
    }

    err = SetNamedSecurityInfoA(
        (LPSTR)to,
        SE_FILE_OBJECT,
        DACL_SECURITY_INFORMATION,
        NULL,
        NULL,
        dacl,
        NULL
    );

    LocalFree(sd);
    return err == ERROR_SUCCESS;
}
#elif defined(FS_LINUX)
bool
fs_copy(const char *from, const char *to)
{
    int ffrom = open(from, O_RDONLY);
    if (ffrom == -1)
    {
        return false;
    }

    struct stat s;
    if (fstat(ffrom, &s) == -1)
    {
        close(ffrom);
        return false;
    }

    int fto = open(to, O_CREAT | O_WRONLY | O_TRUNC, s.st_mode);
    if (fto == -1)
    {
        close(ffrom);
        return false;
    }

    off_t offset = 0;
    while (offset < s.st_size)
    {
        ssize_t result = sendfile(fto, ffrom, &offset, s.st_size - offset);
        if (result <= 0)
        {
            close(fto);
            close(ffrom);
            return false;
        }
    }
    close(fto);
    close(ffrom);
    return true;
}
#elif defined(FS_MACOS)
bool
fs_copy(const char *from, const char *to)
{
    return !copyfile(from, to, NULL, COPYFILE_ALL);
}
#endif

#ifdef FS_WINDOWS
char *
fs_current_dir(char *buf, size_t size)
{
    DWORD len = GetCurrentDirectoryA((DWORD)size, buf);
	if (len == 0 || len >= (DWORD)size)
	{
		return NULL;
	}

	return buf;
}
#elif defined(FS_POSIX)
char *
fs_current_dir(char *buf, size_t size)
{
	return getcwd(buf, size);
}
#endif

/*
The Linux version `readlink("/proc/self/exe")` follows symlinks,
so Windows and MacOS versions must do the same manually to be consistent.
*/
#ifdef FS_WINDOWS
char *
fs_exec_path(char *buf)
{
    char tmp_buf[FS_MAX_PATH];
    if (!GetModuleFileNameA(NULL, tmp_buf, (DWORD)sizeof(tmp_buf)))
    {
        return NULL;
    }
    return fs_absolute(tmp_buf, buf);
}
#elif defined(FS_LINUX)
char *
fs_exec_path(char *buf)
{
    ssize_t len = readlink("/proc/self/exe", buf, FS_MAX_PATH - 1);
    if (len == -1)
    {
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}
#elif defined(FS_MACOS)
char *
fs_exec_path(char *buf)
{
    char tmp_buf[FS_MAX_PATH];
    uint32_t tmp_buf_size = (uint32_t)sizeof(tmp_buf);
    if (_NSGetExecutablePath(tmp_buf, &tmp_buf_size) != 0)
    {
        return NULL;
    }
    return fs_absolute(tmp_buf, buf);
}
#endif

bool
fs_exist(const char *path)
{
	struct stat s;
	return stat(path, &s) == 0;
}

bool
fs_is_dir(const char *path)
{
	struct stat s;
	return (stat(path, &s) == 0) && S_ISDIR(s.st_mode);
}

bool
fs_is_file(const char *path)
{
	struct stat s;
	return (stat(path, &s) == 0) && S_ISREG(s.st_mode);
}


#ifdef FS_WINDOWS
typedef struct reparse_data_buffer_header {
    DWORD reparse_tag;
    WORD  reparse_data_length;
    WORD  reserved;
} reparse_data_buffer_header;
bool
fs_is_symlink(const char *path)
{
    DWORD attrs = GetFileAttributesA(path);
    if (attrs == INVALID_FILE_ATTRIBUTES)
        return false;

    if (!(attrs & FILE_ATTRIBUTE_REPARSE_POINT))
        return false;

    HANDLE h = CreateFileA(
        path,
        0,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );

    if (h == INVALID_HANDLE_VALUE)
        return false;

    BYTE buf[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
    DWORD bytes;

    BOOL result = DeviceIoControl(
        h,
        FSCTL_GET_REPARSE_POINT,
        NULL,
        0,
        buf,
        sizeof(buf),
        &bytes,
        NULL
    );

    CloseHandle(h);

    if (!result)
        return false;

    reparse_data_buffer_header *hdr =
        (reparse_data_buffer_header *)buf;
    return hdr->reparse_tag == IO_REPARSE_TAG_SYMLINK;
}
#elif defined(FS_POSIX)
bool
fs_is_symlink(const char *path)
{
    struct stat s;
	return (lstat(path, &s) == 0) && S_ISLNK(s.st_mode);
}
#endif

off_t
fs_file_size(const char *path)
{
	struct stat s;
    return (stat(path, &s) == 0) ? s.st_size : -1;
}

static void *
fs_read_file_internal(const char *path, void *buf, size_t size, size_t *readen)
{
	FILE *file = fopen(path, "rb");
	if (!file)
	{
		return NULL;
	}

	// file size
    off_t file_size = fs_file_size(path);

    if (file_size == -1)
    {
        fclose(file);
        return NULL;
    }

	void *data = buf;
	size_t read_size = size;
	if (!data)
	{
		read_size = file_size;
		size = file_size + 1;

		// create a buffer large enough
		data = FS_MALLOC(size);
		if (!data)
		{
			fclose(file);
			return NULL;
		}
	}

	if (size > 0)
	{
		read_size = fread(data, 1, read_size, file);

		// append '\0'
		if (read_size < size)
		{
			((char *)data)[read_size] = '\0';
		}
		else
		{
			((char *)data)[size - 1] = '\0';
		}
	}

	fclose(file);

	*readen = file_size;
	return data;
}

size_t
fs_read_file_buffer(const char *path, void *buf, size_t size)
{
	size_t readen = 0;
	fs_read_file_internal(path, buf, size, &readen);
	return readen;
}

void *
fs_read_file(const char *path, size_t *size)
{
	return fs_read_file_internal(path, NULL, 0, size);
}

bool
fs_write_file(const char *path, const void *buf, size_t size)
{
	FILE *file = fopen(path, "wb");
	if (!file)
	{
		return false;
	}

	size_t result = fwrite(buf, sizeof(char), size, file);
	fclose(file);
	return result == size;
}

typedef struct fs_file_iterator
{
	FILE *file;
} fs_file_iterator;

fs_file_iterator *
fs_iter_file(const char *path)
{
	fs_file_iterator *it;
	FILE *f = fopen(path, "rb");
	if (!f)
	{
		return NULL;
	}

	it = (fs_file_iterator *)FS_MALLOC(sizeof(fs_file_iterator));
    if (!it)
    {
        fclose(f);
        return NULL;
    }
	memset(it, 0, sizeof(fs_file_iterator));
	it->file = f;
	return it;
}

fs_file_iterator *
fs_next_char(fs_file_iterator *it, char *c)
{
	if (!fread(c, 1, 1, it->file))
	{
		return NULL;
	}

	return it;
}

void
fs_close_file(fs_file_iterator *it)
{
	fclose(it->file);
	FS_FREE(it);
}

int
fs_join_path(char *buf, size_t size, const char *left, const char *right)
{
#ifdef FS_WINDOWS
	return snprintf(buf, size, "%s\\%s", left, right);
#elif defined(FS_POSIX)
    return snprintf(buf, size, "%s/%s", left, right);
#endif
}

#ifdef FS_WINDOWS
char *
fs_temp_dir(char *buf, size_t size)
{
    DWORD len = GetTempPathA((DWORD)size, buf);
	if (len == 0 || len >= (DWORD)size)
	{
		return NULL;
	}

	return buf;
}

bool
fs_delete_dir(const char *path)
{
	if (RemoveDirectoryA(path) == 0)
	{
        DWORD err = GetLastError();
		return err == ERROR_FILE_NOT_FOUND ||
               err == ERROR_PATH_NOT_FOUND;
	}

	return true;
}

bool
fs_delete_file(const char *path)
{
	if (DeleteFileA(path) == 0)
	{
		DWORD err = GetLastError();
		return err == ERROR_FILE_NOT_FOUND ||
               err == ERROR_PATH_NOT_FOUND;
	}

	return true;
}

bool
fs_make_dir(const char *path)
{
	if (CreateDirectoryA(path, NULL) == 0)
	{
		return GetLastError() == ERROR_ALREADY_EXISTS;
	}

	return true;
}
#elif defined(FS_POSIX)
char *
fs_temp_dir(char *buf, size_t size)
{
	const char *path = getenv("TMPDIR");
	if (!path)
	{
        path = "/tmp";
	}

	if (snprintf(buf, size, "%s", path) >= (int)size)
    {
        return NULL;
    }
    return buf;
}

bool
fs_delete_dir(const char *path)
{
	return (rmdir(path) == 0) || (ENOENT == errno);
}

bool
fs_delete_file(const char *path)
{
	return (remove(path) == 0) || (ENOENT == errno);
}
bool
fs_make_dir(const char *path)
{
    return (mkdir(path, FS_MKDIR_PERMISSIONS) == 0) || (EEXIST == errno);
}
#endif

#ifdef FS_WINDOWS
typedef struct fs_win_directory_iterator
{
	fs_directory_iterator base;
	WIN32_FIND_DATA file;
	HANDLE h;
	bool started;
} fs_win_directory_iterator;

fs_directory_iterator *
fs_open_dir(const char *path)
{
    char dir[FS_MAX_PATH];
    StringCchCopyA(dir, FS_MAX_PATH, path);
	StringCchCatA(dir, FS_MAX_PATH, "\\*");

    WIN32_FIND_DATA file;
    HANDLE h = FindFirstFileA(dir, &file);
	if (h == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	fs_win_directory_iterator *it =
        (fs_win_directory_iterator *)FS_MALLOC(sizeof(fs_win_directory_iterator));
    if (!it) {
        FindClose(h);
        return NULL;
    }
    memset(it, 0, sizeof(fs_win_directory_iterator));
	it->file = file;
	it->h = h;
	return (fs_directory_iterator *)it;
}

fs_directory_iterator *
fs_read_dir(fs_directory_iterator *it)
{
	fs_win_directory_iterator *_it = (fs_win_directory_iterator *)it;

	if (!_it->started)
	{
		_it->started = true;
	}
	else if (!FindNextFileA(_it->h, &_it->file))
	{
		return NULL;
	}

	_it->base.path = _it->file.cFileName;
	return it;
}

void
fs_close_dir(fs_directory_iterator *it)
{
	fs_win_directory_iterator *_it = (fs_win_directory_iterator *)it;
	FindClose(_it->h);
	FS_FREE(_it);
}
#elif defined(FS_POSIX)
typedef struct fs_posix_directory_iterator
{
	fs_directory_iterator base;
	DIR *dir;
	struct dirent *ent;
} fs_posix_directory_iterator;

fs_directory_iterator *
fs_open_dir(const char *path)
{
	fs_posix_directory_iterator *it;
	DIR *d = opendir(path);
	if (!d)
	{
		return NULL;
	}

	it = (fs_posix_directory_iterator *)FS_MALLOC(sizeof(fs_posix_directory_iterator));
    if (!it) {
        closedir(d);
        return NULL;
    }
    memset(it, 0, sizeof(fs_posix_directory_iterator));
	it->dir = d;
	return (fs_directory_iterator *)it;
}

fs_directory_iterator *
fs_read_dir(fs_directory_iterator *it)
{
	fs_posix_directory_iterator *_it = (fs_posix_directory_iterator *)it;
	if (!(_it->ent = readdir(_it->dir)))
	{
		return NULL;
	}

	_it->base.path = &_it->ent->d_name[0];
	return it;
}

void
fs_close_dir(fs_directory_iterator *it)
{
	fs_posix_directory_iterator *_it = (fs_posix_directory_iterator *)it;
	closedir(_it->dir);
	FS_FREE(_it);
}
#endif

#endif

#endif
