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

#ifndef LIBFS__h
#define LIBFS__h

#ifdef __cplusplus
extern "C"
{
#endif

/** Major version of libfs. */
#define LIBFS_VERSION_MAJOR 0
/** Minor version of libfs. */
#define LIBFS_VERSION_MINOR 2
/** Patch version of libfs. */
#define LIBFS_VERSION_PATCH 3

/* Define to 1 if you build with Doxygen. */
#ifndef LIBFS_DOXYGEN
/* #undef LIBFS_DOXYGEN */
#endif

#if !defined(LIBFS_MALLOC) && !defined(LIBFS_FREE)
/**
 * Defines the malloc function used by libfs at compile time.
 *
 * @code
 * void* my_malloc(size_t size)
 * {
 *     // do something
 * }
 *
 * #define LIBFS_MALLOC my_malloc
 * @endcode
 */
#define LIBFS_MALLOC malloc
/**
 * Defines the free function used by libfs at compile time.
 *
 * @code
 * void my_free(void* ptr)
 * {
 *     // do something
 * }
 *
 * #define LIBFS_FREE my_free
 * @endcode
 */
#define LIBFS_FREE free
#endif


#ifdef _WIN32
#define LIBFS_WINDOWS
#define LIBFS_MAX_PATH 260
#elif defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#define LIBFS_POSIX
#define LIBFS_MAX_PATH 4096

#else
#error "unsupported platform"
#endif

#include <stddef.h>    // for size_t
#include <stdbool.h>   // for bool
#include <sys/types.h> // for off_t


/**
    * Composes an absolute path.
    *
    * @code{.c}
    * char buf[MAX_PATH];
    * if (!fs_absolute("./relative", buf, MAX_PATH))
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
    * @param[out] buf Buffer for storing the result path
    * @param[in] size Buffer size
    * @return A pointer to buf if there is no error, NULL otherwise.
    */
char *
fs_absolute(const char *path, char *buf, size_t size);

/**
    * Gets a pointer to the rightmost path separator.
    *
    * @code{.c}
    * const char* c = fs_rsplit("./path/to/foo.txt");
    * @endcode
    *
    * @param[in] path Some null-terminated path
    * @return A pointer to the rightmost separator.
    */
const char*
fs_rsplit(const char* path);

/**
    * Gets the directory name of path.
    *
    * @code{.c}
    * char dirname[256];
    * fs_dirname("./foo.txt", dirname, 256);
    * @endcode
    *
    * @param[in] path Some null-terminated path
    * @param[out] buf Buffer for storing the result path
    * @param[in] size Buffer size
    * @return The number of bytes that would have been written if
    * buf was large enough (excluding the null-terminating character).
    */
size_t
fs_dirname(const char* path, char *buf, size_t size);

/**
    * Gets the base name of path.
    *
    * @code{.c}
    * const char* basename = fs_basename("./foo.txt");
    * @endcode
    *
    * @param[in] path Some null-terminated path
    * @return A pointer to the base name.
    */
const char*
fs_basename(const char* path);

/**
    * Copies files or directories.
    *
    * @code{.c}
    * fs_copy("foo.txt", "bar.txt");
    * @endcode
    *
    * @param[in] from Some null-terminated path to the source file, directory, or symlink
    * @param[in] to Some null-terminated path to the destination file, directory, or symlink
    */
void
fs_copy(const char *from, const char *to);

/**
    * Get the current working directory.
    *
    * @code{.c}
    * char buf[MAX_PATH];
    * if (!fs_current_dir(buf, MAX_PATH))
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
    * Concatenates two paths together with the platform specific separator.
    *
    * @code{.c}
    * char cwd[MAX_PATH];
    * if (!fs_current_dir(cwd, MAX_PATH))
    * {
    *     print("fs_current_dir failed");
    *     return;
    * }
    *
    * char buf[MAX_PATH];
    * fs_join_path(buf, MAX_PATH, cwd, "foo.txt");
    * printf("%s", buf);
    * @endcode
    *
    * @param[out] buf Buffer for storing the result path
    * @param[in] size Buffer size
    * @param[in] left Left part null-terminated path
    * @param[in] right Right part null-terminated path
    * @return The number of bytes written to buf, < 0 if there is an error, or >= size if the result was truncated.
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
    * @return The size of the file, in bytes
    */
off_t
fs_file_size(const char *path);

/**
    * Checks if a path corresponds to a directory.
    *
    * @code{.c}
    * if (!fs_is_directory("./somedirectory"))
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
fs_is_directory(const char *path);

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

#ifdef LIBFS_IMPLEMENTATION

#ifdef LIBFS_WINDOWS
#include <direct.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winioctl.h> // for FSCTL_GET_REPARSE_POINT
#include <strsafe.h>

#ifndef S_ISDIR
#define S_ISDIR(x) ((x & _S_IFDIR) == _S_IFDIR)
#endif
#ifndef S_ISREG
#define S_ISREG(x) ((x & _S_IFREG) == _S_IFREG)
#endif

#elif LIBFS_POSIX
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/sendfile.h>
#endif

#if defined(LIBFS_POSIX) || defined(LIBFS_WINDOWS)
#include <sys/stat.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#define LIBFS_MKDIR_PERMISSIONS 0700
#define LIBFS_UNUSED(x) (void)(x)

typedef struct fs_directory_iterator fs_directory_iterator;

const char *
fs_rsplit(const char *path)
{
	char *c1;
	char *c2;

	c1 = strrchr(path, '/');
	if (!c1)
	{
		return strrchr(path, '\\');
	}

	c2 = strrchr(c1, '\\');
	return c2 ? c2 : c1;
}

size_t
fs_dirname(const char *path, char *buf, size_t size)
{
	const char *c = fs_rsplit(path);
	if (!c)
	{
		snprintf(buf, size, "%s", "");
		return 0;
	}

	return snprintf(buf, size, "%.*s", (int)(c - path), path);
}

const char *
fs_basename(const char *path)
{
	const char *c = fs_rsplit(path);
	if (!c)
	{
		return path;
	}

	return c + 1;
}

#ifdef LIBFS_WINDOWS
char *
fs_absolute(const char *path, char *buf, size_t size)
{
	if (!GetFullPathName(path, (DWORD)size, buf, NULL))
	{
		return NULL;
	}

	return buf;
}

void
fs_copy(const char *from, const char *to)
{
	CopyFile(from, to, 0);
}
#elif LIBFS_POSIX
char *
fs_absolute(const char *path, char *buf, size_t size)
{
	LIBFS_UNUSED(size);
	if (!realpath(path, buf))
	{
		return NULL;
	}

	return buf;
}

void
fs_copy(const char *from, const char *to)
{
	FILE *ffrom = fopen(from, "rb");
	if (!ffrom)
	{
		return;
	}

	FILE *fto = fopen(to, "wb");
	if (!fto)
	{
		return;
	}

	fseek(ffrom, 0, SEEK_END);
	long size = ftell(ffrom);
	fseek(ffrom, 0, 0);

	sendfile(ffrom, fto, 0, size);

	fclose(ffrom);
	fclose(fto);
}
#endif

#ifdef LIBFS_WINDOWS
char *
fs_current_dir(char *buf, size_t size)
{
	if (GetCurrentDirectory((DWORD)size, buf))
	{
		return buf;
	}

	return NULL;
}
#else
char *
fs_current_dir(char *buf, size_t size)
{
	return getcwd(buf, size);
}
#endif

bool
fs_exist(const char *path)
{
	struct stat s;
	return stat(path, &s) == 0;
}

bool
fs_is_directory(const char *path)
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

bool
fs_is_symlink(const char *path)
{
#ifdef LIBFS_WINDOWS
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

    BYTE buffer[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
    DWORD bytes;

    BOOL ok = DeviceIoControl(
        h,
        FSCTL_GET_REPARSE_POINT,
        NULL,
        0,
        buffer,
        sizeof(buffer),
        &bytes,
        NULL
    );

    CloseHandle(h);

    if (!ok)
        return false;

    typedef struct {
        DWORD ReparseTag;
        WORD  ReparseDataLength;
        WORD  Reserved;
    } REPARSE_DATA_BUFFER_HEADER;

    REPARSE_DATA_BUFFER_HEADER *hdr =
        (REPARSE_DATA_BUFFER_HEADER *)buffer;

    return hdr->ReparseTag == IO_REPARSE_TAG_SYMLINK;
#elif defined(LIBFS_POSIX)
    struct stat s;
	return (lstat(path, &s) == 0) && S_ISLNK(s.st_mode);
#endif
}

off_t
fs_file_size(const char *path)
{
	struct stat s;
	FILE *file = fopen(path, "rb");
	if (!file)
	{
		return -1L;
	}

	if (fstat(fileno(file), &s) == -1)
	{
		return -1L;
	}

	fclose(file);
	return s.st_size;
}

static void *
fs_read_file_internal(const char *path, void *buf, size_t size, size_t *readen)
{
	void *data;
	size_t file_size;
	size_t read_size;
	FILE *file = fopen(path, "rb");
	if (!file)
	{
		return NULL;
	}

	/* File size */
	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	data = buf;
	read_size = size;
	if (!data)
	{
		read_size = file_size;
		size = file_size + 1;

		/* Create a buffer large enough */
		data = LIBFS_MALLOC(size);
		if (!data)
		{
			fclose(file);
			return NULL;
		}
	}

	if (size > 0)
	{
		read_size = fread(data, 1, read_size, file);

		/* Append \0 */
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

	fwrite(buf, size, 1, file);
	fclose(file);
	return true;
}

typedef struct fs_file_iterator
{
	FILE *file;
} fs_file_iterator;

fs_file_iterator *
fs_iter_file(const char *path)
{
	fs_file_iterator *it;
	FILE *f = fopen(path, "r");
	if (!f)
	{
		return NULL;
	}

	it = (fs_file_iterator *)LIBFS_MALLOC(sizeof(fs_file_iterator));
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
	LIBFS_FREE(it);
}

int
fs_join_path(char *buf, size_t size, const char *left, const char *right)
{
#ifdef LIBFS_WINDOWS
	return snprintf(buf, size, "%s\\%s", left, right);
#else
    return snprintf(buf, size, "%s/%s", left, right);
#endif
}

#ifdef LIBFS_WINDOWS
char *
fs_temp_dir(char *buf, size_t size)
{
	if (!GetTempPath((DWORD)size, buf))
	{
		return NULL;
	}

	return buf;
}

bool
fs_delete_dir(const char *path)
{
	if (RemoveDirectory(path) == 0)
	{
		return ERROR_FILE_NOT_FOUND == GetLastError();
	}

	return true;
}

bool
fs_delete_file(const char *path)
{
	if (DeleteFile(path) == 0)
	{
		return ERROR_FILE_NOT_FOUND == GetLastError();
	}

	return true;
}

bool
fs_make_dir(const char *path)
{
	if (CreateDirectory(path, NULL) == 0)
	{
		return ERROR_ALREADY_EXISTS == GetLastError();
	}

	return true;
}
#elif defined(LIBFS_POSIX)
char *
fs_temp_dir(char *buf, size_t size)
{
	const char *path = getenv("TMPDIR");
	if (!path)
	{
		return NULL;
	}

	snprintf(buf, size, "%s", path);
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
    return (mkdir(path, LIBFS_MKDIR_PERMISSIONS) == 0) || (EEXIST == errno);
}
#endif

#ifdef LIBFS_WINDOWS
typedef struct fs_win_directory_iterator
{
	fs_directory_iterator base;
	WIN32_FIND_DATA fdFile;
	HANDLE hFind;
	bool started;
} fs_win_directory_iterator;

fs_directory_iterator *
fs_open_dir(const char *path)
{
	fs_win_directory_iterator *it;
	TCHAR szDir[MAX_PATH];
	WIN32_FIND_DATA fdFile;
	HANDLE hFind;
	StringCchCopy(szDir, MAX_PATH, path);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));
	hFind = FindFirstFile(szDir, &fdFile);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	it = (fs_win_directory_iterator *)LIBFS_MALLOC(sizeof(fs_win_directory_iterator));
	memset(it, 0, sizeof(fs_win_directory_iterator));
	it->fdFile = fdFile;
	it->hFind = hFind;
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
	else if (!FindNextFile(_it->hFind, &_it->fdFile))
	{
		return NULL;
	}

	_it->base.path = _it->fdFile.cFileName;
	return it;
}

void
fs_close_dir(fs_directory_iterator *it)
{
	fs_win_directory_iterator *_it = (fs_win_directory_iterator *)it;
	FindClose(_it->hFind);
	LIBFS_FREE(_it);
}
#elif defined(LIBFS_POSIX)
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

	it = (fs_posix_directory_iterator *)LIBFS_MALLOC(sizeof(fs_posix_directory_iterator));
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
	LIBFS_FREE(_it);
}
#endif

#endif

#endif
