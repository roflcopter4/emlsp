#pragma once
#ifndef HGUARD__UTIL__ERRNO_HH_
#define HGUARD__UTIL__ERRNO_HH_  //NOLINT

//#include "Common.hh"

inline namespace emlsp {
namespace util {
/****************************************************************************************/


#define X(w, e)    \
 {                 \
  ERROR_##w, #w, e \
 }

static constexpr struct {
      DWORD       w; /* windows version of error */
      std::string s; /* text of windows version */
      int         e; /* errno version of error */
} errmap[] = {
  /* FIXME: Some of these choices are arbitrary! */
    {ERROR_ACCESS_DENIED,             "ACCESS_DENIED",             EACCES      },
    {ERROR_ACTIVE_CONNECTIONS,        "ACTIVE_CONNECTIONS",        EAGAIN      },
    {ERROR_ALREADY_EXISTS,            "ALREADY_EXISTS",            EEXIST      },
    {ERROR_BAD_DEVICE,                "BAD_DEVICE",                ENODEV      },
    {ERROR_BAD_EXE_FORMAT,            "BAD_EXE_FORMAT",            ENOEXEC     },
    {ERROR_BAD_NETPATH,               "BAD_NETPATH",               ENOENT      },
    {ERROR_BAD_NET_NAME,              "BAD_NET_NAME",              ENOENT      },
    {ERROR_BAD_NET_RESP,              "BAD_NET_RESP",              ENOSYS      },
    {ERROR_BAD_PATHNAME,              "BAD_PATHNAME",              ENOENT      },
    {ERROR_BAD_PIPE,                  "BAD_PIPE",                  EINVAL      },
    {ERROR_BAD_UNIT,                  "BAD_UNIT",                  ENODEV      },
    {ERROR_BAD_USERNAME,              "BAD_USERNAME",              EINVAL      },
    {ERROR_BEGINNING_OF_MEDIA,        "BEGINNING_OF_MEDIA",        EIO         },
    {ERROR_BROKEN_PIPE,               "BROKEN_PIPE",               EPIPE       },
    {ERROR_BUSY,                      "BUSY",                      EBUSY       },
    {ERROR_BUS_RESET,                 "BUS_RESET",                 EIO         },
    {ERROR_CALL_NOT_IMPLEMENTED,      "CALL_NOT_IMPLEMENTED",      ENOSYS      },
    {ERROR_CANCELLED,                 "CANCELLED",                 EINTR       },
    {ERROR_CANNOT_MAKE,               "CANNOT_MAKE",               EPERM       },
    {ERROR_CHILD_NOT_COMPLETE,        "CHILD_NOT_COMPLETE",        EBUSY       },
    {ERROR_COMMITMENT_LIMIT,          "COMMITMENT_LIMIT",          EAGAIN      },
    {ERROR_CONNECTION_REFUSED,        "CONNECTION_REFUSED",        ECONNREFUSED},
    {ERROR_CRC,                       "CRC",                       EIO         },
    {ERROR_DEVICE_DOOR_OPEN,          "DEVICE_DOOR_OPEN",          EIO         },
    {ERROR_DEVICE_IN_USE,             "DEVICE_IN_USE",             EAGAIN      },
    {ERROR_DEVICE_REQUIRES_CLEANING,  "DEVICE_REQUIRES_CLEANING",  EIO         },
    {ERROR_DEV_NOT_EXIST,             "DEV_NOT_EXIST",             ENOENT      },
    {ERROR_DIRECTORY,                 "DIRECTORY",                 ENOTDIR     },
    {ERROR_DIR_NOT_EMPTY,             "DIR_NOT_EMPTY",             ENOTEMPTY   },
    {ERROR_DISK_CORRUPT,              "DISK_CORRUPT",              EIO         },
    {ERROR_DISK_FULL,                 "DISK_FULL",                 ENOSPC      },
    {ERROR_DS_GENERIC_ERROR,          "DS_GENERIC_ERROR",          EIO         },
    {ERROR_DUP_NAME,                  "DUP_NAME",                  ENOTUNIQ    },
    {ERROR_EAS_DIDNT_FIT,             "EAS_DIDNT_FIT",             ENOSPC      },
    {ERROR_EAS_NOT_SUPPORTED,         "EAS_NOT_SUPPORTED",         ENOTSUP     },
    {ERROR_EA_LIST_INCONSISTENT,      "EA_LIST_INCONSISTENT",      EINVAL      },
    {ERROR_EA_TABLE_FULL,             "EA_TABLE_FULL",             ENOSPC      },
    {ERROR_END_OF_MEDIA,              "END_OF_MEDIA",              ENOSPC      },
    {ERROR_EOM_OVERFLOW,              "EOM_OVERFLOW",              EIO         },
    {ERROR_EXE_MACHINE_TYPE_MISMATCH, "EXE_MACHINE_TYPE_MISMATCH", ENOEXEC     },
    {ERROR_EXE_MARKED_INVALID,        "EXE_MARKED_INVALID",        ENOEXEC     },
    {ERROR_FILEMARK_DETECTED,         "FILEMARK_DETECTED",         EIO         },
    {ERROR_FILENAME_EXCED_RANGE,      "FILENAME_EXCED_RANGE",      ENAMETOOLONG},
    {ERROR_FILE_CORRUPT,              "FILE_CORRUPT",              EEXIST      },
    {ERROR_FILE_EXISTS,               "FILE_EXISTS",               EEXIST      },
    {ERROR_FILE_INVALID,              "FILE_INVALID",              ENXIO       },
    {ERROR_FILE_NOT_FOUND,            "FILE_NOT_FOUND",            ENOENT      },
    {ERROR_HANDLE_DISK_FULL,          "HANDLE_DISK_FULL",          ENOSPC      },
    {ERROR_HANDLE_EOF,                "HANDLE_EOF",                ENODATA     },
    {ERROR_INVALID_ADDRESS,           "INVALID_ADDRESS",           EINVAL      },
    {ERROR_INVALID_AT_INTERRUPT_TIME, "INVALID_AT_INTERRUPT_TIME", EINTR       },
    {ERROR_INVALID_BLOCK_LENGTH,      "INVALID_BLOCK_LENGTH",      EIO         },
    {ERROR_INVALID_DATA,              "INVALID_DATA",              EINVAL      },
    {ERROR_INVALID_DRIVE,             "INVALID_DRIVE",             ENODEV      },
    {ERROR_INVALID_EA_NAME,           "INVALID_EA_NAME",           EINVAL      },
    {ERROR_INVALID_EXE_SIGNATURE,     "INVALID_EXE_SIGNATURE",     ENOEXEC     },
    {ERROR_INVALID_FUNCTION,          "INVALID_FUNCTION",          EBADRQC     },
    {ERROR_INVALID_HANDLE,            "INVALID_HANDLE",            EBADF       },
    {ERROR_INVALID_NAME,              "INVALID_NAME",              ENOENT      },
    {ERROR_INVALID_PARAMETER,         "INVALID_PARAMETER",         EINVAL      },
    {ERROR_INVALID_SIGNAL_NUMBER,     "INVALID_SIGNAL_NUMBER",     EINVAL      },
    {ERROR_IOPL_NOT_ENABLED,          "IOPL_NOT_ENABLED",          ENOEXEC     },
    {ERROR_IO_DEVICE,                 "IO_DEVICE",                 EIO         },
    {ERROR_IO_INCOMPLETE,             "IO_INCOMPLETE",             EAGAIN      },
    {ERROR_IO_PENDING,                "IO_PENDING",                EAGAIN      },
    {ERROR_LOCK_VIOLATION,            "LOCK_VIOLATION",            EBUSY       },
    {ERROR_MAX_THRDS_REACHED,         "MAX_THRDS_REACHED",         EAGAIN      },
    {ERROR_META_EXPANSION_TOO_LONG,   "META_EXPANSION_TOO_LONG",   EINVAL      },
    {ERROR_MOD_NOT_FOUND,             "MOD_NOT_FOUND",             ENOENT      },
    {ERROR_MORE_DATA,                 "MORE_DATA",                 EMSGSIZE    },
    {ERROR_NEGATIVE_SEEK,             "NEGATIVE_SEEK",             EINVAL      },
    {ERROR_NETNAME_DELETED,           "NETNAME_DELETED",           ENOENT      },
    {ERROR_NOACCESS,                  "NOACCESS",                  EFAULT      },
    {ERROR_NONE_MAPPED,               "NONE_MAPPED",               EINVAL      },
    {ERROR_NONPAGED_SYSTEM_RESOURCES, "NONPAGED_SYSTEM_RESOURCES", EAGAIN      },
    {ERROR_NOT_CONNECTED,             "NOT_CONNECTED",             ENOLINK     },
    {ERROR_NOT_ENOUGH_MEMORY,         "NOT_ENOUGH_MEMORY",         ENOMEM      },
    {ERROR_NOT_ENOUGH_QUOTA,          "NOT_ENOUGH_QUOTA",          EIO         },
    {ERROR_NOT_OWNER,                 "NOT_OWNER",                 EPERM       },
    {ERROR_NOT_READY,                 "NOT_READY",                 ENOMEDIUM   },
    {ERROR_NOT_SAME_DEVICE,           "NOT_SAME_DEVICE",           EXDEV       },
    {ERROR_NOT_SUPPORTED,             "NOT_SUPPORTED",             ENOSYS      },
    {ERROR_NO_DATA,                   "NO_DATA",                   EPIPE       },
    {ERROR_NO_DATA_DETECTED,          "NO_DATA_DETECTED",          EIO         },
    {ERROR_NO_MEDIA_IN_DRIVE,         "NO_MEDIA_IN_DRIVE",         ENOMEDIUM   },
    {ERROR_NO_MORE_FILES,             "NO_MORE_FILES",             ENMFILE     },
    {ERROR_NO_MORE_ITEMS,             "NO_MORE_ITEMS",             ENMFILE     },
    {ERROR_NO_MORE_SEARCH_HANDLES,    "NO_MORE_SEARCH_HANDLES",    ENFILE      },
    {ERROR_NO_PROC_SLOTS,             "NO_PROC_SLOTS",             EAGAIN      },
    {ERROR_NO_SIGNAL_SENT,            "NO_SIGNAL_SENT",            EIO         },
    {ERROR_NO_SYSTEM_RESOURCES,       "NO_SYSTEM_RESOURCES",       EFBIG       },
    {ERROR_NO_TOKEN,                  "NO_TOKEN",                  EINVAL      },
    {ERROR_OPEN_FAILED,               "OPEN_FAILED",               EIO         },
    {ERROR_OPEN_FILES,                "OPEN_FILES",                EAGAIN      },
    {ERROR_OUTOFMEMORY,               "OUTOFMEMORY",               ENOMEM      },
    {ERROR_PAGED_SYSTEM_RESOURCES,    "PAGED_SYSTEM_RESOURCES",    EAGAIN      },
    {ERROR_PAGEFILE_QUOTA,            "PAGEFILE_QUOTA",            EAGAIN      },
    {ERROR_PATH_NOT_FOUND,            "PATH_NOT_FOUND",            ENOENT      },
    {ERROR_PIPE_BUSY,                 "PIPE_BUSY",                 EBUSY       },
    {ERROR_PIPE_CONNECTED,            "PIPE_CONNECTED",            EBUSY       },
    {ERROR_PIPE_LISTENING,            "PIPE_LISTENING",            ECOMM       },
    {ERROR_PIPE_NOT_CONNECTED,        "PIPE_NOT_CONNECTED",        ECOMM       },
    {ERROR_POSSIBLE_DEADLOCK,         "POSSIBLE_DEADLOCK",         EDEADLOCK   },
    {ERROR_PRIVILEGE_NOT_HELD,        "PRIVILEGE_NOT_HELD",        EPERM       },
    {ERROR_PROCESS_ABORTED,           "PROCESS_ABORTED",           EFAULT      },
    {ERROR_PROC_NOT_FOUND,            "PROC_NOT_FOUND",            ESRCH       },
    {ERROR_REM_NOT_LIST,              "REM_NOT_LIST",              ENONET      },
    {ERROR_SECTOR_NOT_FOUND,          "SECTOR_NOT_FOUND",          EINVAL      },
    {ERROR_SEEK,                      "SEEK",                      EINVAL      },
    {ERROR_SERVICE_REQUEST_TIMEOUT,   "SERVICE_REQUEST_TIMEOUT",   EBUSY       },
    {ERROR_SETMARK_DETECTED,          "SETMARK_DETECTED",          EIO         },
    {ERROR_SHARING_BUFFER_EXCEEDED,   "SHARING_BUFFER_EXCEEDED",   ENOLCK      },
    {ERROR_SHARING_VIOLATION,         "SHARING_VIOLATION",         EBUSY       },
    {ERROR_SIGNAL_PENDING,            "SIGNAL_PENDING",            EBUSY       },
    {ERROR_SIGNAL_REFUSED,            "SIGNAL_REFUSED",            EIO         },
    {ERROR_SXS_CANT_GEN_ACTCTX,       "SXS_CANT_GEN_ACTCTX",       ELIBBAD     },
    {ERROR_THREAD_1_INACTIVE,         "THREAD_1_INACTIVE",         EINVAL      },
    {ERROR_TIMEOUT,                   "TIMEOUT",                   EBUSY       },
    {ERROR_TOO_MANY_LINKS,            "TOO_MANY_LINKS",            EMLINK      },
    {ERROR_TOO_MANY_OPEN_FILES,       "TOO_MANY_OPEN_FILES",       EMFILE      },
    {ERROR_UNEXP_NET_ERR,             "UNEXP_NET_ERR",             EIO         },
    {ERROR_WAIT_NO_CHILDREN,          "WAIT_NO_CHILDREN",          ECHILD      },
    {ERROR_WORKING_SET_QUOTA,         "WORKING_SET_QUOTA",         EAGAIN      },
    {ERROR_WRITE_PROTECT,             "WRITE_PROTECT",             EROFS       },
    {0,                               nullptr,                     0           }
};

extern "C" {
const char *_sys_errlist[] = {
    /* NOERROR 0 */ "No error",
    /* EPERM 1 */ "Operation not permitted",
    /* ENOENT 2 */ "No such file or directory",
    /* ESRCH 3 */ "No such process",
    /* EINTR 4 */ "Interrupted system call",
    /* EIO 5 */ "Input/output error",
    /* ENXIO 6 */ "No such device or address",
    /* E2BIG 7 */ "Argument list too long",
    /* ENOEXEC 8 */ "Exec format error",
    /* EBADF 9 */ "Bad file descriptor",
    /* ECHILD 10 */ "No child processes",
    /* EAGAIN 11 */ "Resource temporarily unavailable",
    /* ENOMEM 12 */ "Cannot allocate memory",
    /* EACCES 13 */ "Permission denied",
    /* EFAULT 14 */ "Bad address",
    /* ENOTBLK 15 */ "Block device required",
    /* EBUSY 16 */ "Device or resource busy",
    /* EEXIST 17 */ "File exists",
    /* EXDEV 18 */ "Invalid cross-device link",
    /* ENODEV 19 */ "No such device",
    /* ENOTDIR 20 */ "Not a directory",
    /* EISDIR 21 */ "Is a directory",
    /* EINVAL 22 */ "Invalid argument",
    /* ENFILE 23 */ "Too many open files in system",
    /* EMFILE 24 */ "Too many open files",
    /* ENOTTY 25 */ "Inappropriate ioctl for device",
    /* ETXTBSY 26 */ "Text file busy",
    /* EFBIG 27 */ "File too large",
    /* ENOSPC 28 */ "No space left on device",
    /* ESPIPE 29 */ "Illegal seek",
    /* EROFS 30 */ "Read-only file system",
    /* EMLINK 31 */ "Too many links",
    /* EPIPE 32 */ "Broken pipe",
    /* EDOM 33 */ "Numerical argument out of domain",
    /* ERANGE 34 */ "Numerical result out of range",
    /* ENOMSG 35 */ "No message of desired type",
    /* EIDRM 36 */ "Identifier removed",
    /* ECHRNG 37 */ "Channel number out of range",
    /* EL2NSYNC 38 */ "Level 2 not synchronized",
    /* EL3HLT 39 */ "Level 3 halted",
    /* EL3RST 40 */ "Level 3 reset",
    /* ELNRNG 41 */ "Link number out of range",
    /* EUNATCH 42 */ "Protocol driver not attached",
    /* ENOCSI 43 */ "No CSI structure available",
    /* EL2HLT 44 */ "Level 2 halted",
    /* EDEADLK 45 */ "Resource deadlock avoided",
    /* ENOLCK 46 */ "No locks available",
    NULL,
    NULL,
    NULL,
    /* EBADE 50 */ "Invalid exchange",
    /* EBADR 51 */ "Invalid request descriptor",
    /* EXFULL 52 */ "Exchange full",
    /* ENOANO 53 */ "No anode",
    /* EBADRQC 54 */ "Invalid request code",
    /* EBADSLT 55 */ "Invalid slot",
    /* EDEADLOCK 56 */ "File locking deadlock error",
    /* EBFONT 57 */ "Bad font file format",
    NULL,
    NULL,
    /* ENOSTR 60 */ "Device not a stream",
    /* ENODATA 61 */ "No data available",
    /* ETIME 62 */ "Timer expired",
    /* ENOSR 63 */ "Out of streams resources",
    /* ENONET 64 */ "Machine is not on the network",
    /* ENOPKG 65 */ "Package not installed",
    /* EREMOTE 66 */ "Object is remote",
    /* ENOLINK 67 */ "Link has been severed",
    /* EADV 68 */ "Advertise error",
    /* ESRMNT 69 */ "Srmount error",
    /* ECOMM 70 */ "Communication error on send",
    /* EPROTO 71 */ "Protocol error",
    NULL,
    NULL,
    /* EMULTIHOP 74 */ "Multihop attempted",
    /* ELBIN 75 */ "Inode is remote (not really error)",
    /* EDOTDOT 76 */ "RFS specific error",
    /* EBADMSG 77 */ "Bad message",
    NULL,
    /* EFTYPE 79 */ "Inappropriate file type or format",
    /* ENOTUNIQ 80 */ "Name not unique on network",
    /* EBADFD 81 */ "File descriptor in bad state",
    /* EREMCHG 82 */ "Remote address changed",
    /* ELIBACC 83 */ "Can not access a needed shared library",
    /* ELIBBAD 84 */ "Accessing a corrupted shared library",
    /* ELIBSCN 85 */ ".lib section in a.out corrupted",
    /* ELIBMAX 86 */ "Attempting to link in too many shared libraries",
    /* ELIBEXEC 87 */ "Cannot exec a shared library directly",
    /* ENOSYS 88 */ "Function not implemented",
    /* ENMFILE 89 */ "No more files",
    /* ENOTEMPTY 90	*/ "Directory not empty",
    /* ENAMETOOLONG 91 */ "File name too long",
    /* ELOOP 92 */ "Too many levels of symbolic links",
    NULL,
    NULL,
    /* EOPNOTSUPP 95 */ "Operation not supported",
    /* EPFNOSUPPORT 96 */ "Protocol family not supported",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    /* ECONNRESET 104 */ "Connection reset by peer",
    /* ENOBUFS 105 */ "No buffer space available",
    /* EAFNOSUPPORT 106 */ "Address family not supported by protocol",
    /* EPROTOTYPE 107 */ "Protocol wrong type for socket",
    /* ENOTSOCK 108 */ "Socket operation on non-socket",
    /* ENOPROTOOPT 109 */ "Protocol not available",
    /* ESHUTDOWN 110 */ "Cannot send after transport endpoint shutdown",
    /* ECONNREFUSED 111 */ "Connection refused",
    /* EADDRINUSE 112 */ "Address already in use",
    /* ECONNABORTED 113 */ "Software caused connection abort",
    /* ENETUNREACH 114 */ "Network is unreachable",
    /* ENETDOWN 115 */ "Network is down",
    /* ETIMEDOUT 116 */ "Connection timed out",
    /* EHOSTDOWN 117 */ "Host is down",
    /* EHOSTUNREACH 118 */ "No route to host",
    /* EINPROGRESS 119 */ "Operation now in progress",
    /* EALREADY 120 */ "Operation already in progress",
    /* EDESTADDRREQ 121 */ "Destination address required",
    /* EMSGSIZE 122 */ "Message too long",
    /* EPROTONOSUPPORT 123 */ "Protocol not supported",
    /* ESOCKTNOSUPPORT 124 */ "Socket type not supported",
    /* EADDRNOTAVAIL 125 */ "Cannot assign requested address",
    /* ENETRESET 126 */ "Network dropped connection on reset",
    /* EISCONN 127 */ "Transport endpoint is already connected",
    /* ENOTCONN 128 */ "Transport endpoint is not connected",
    /* ETOOMANYREFS 129 */ "Too many references: cannot splice",
    /* EPROCLIM 130 */ "Too many processes",
    /* EUSERS 131 */ "Too many users",
    /* EDQUOT 132 */ "Disk quota exceeded",
    /* ESTALE 133 */ "Stale NFS file handle",
    /* ENOTSUP 134 */ "Not supported",
    /* ENOMEDIUM 135 */ "No medium found",
    /* ENOSHARE 136 */ "No such host or network path",
    /* ECASECLASH 137 */ "Filename exists with different case",
    /* EILSEQ 138 */ "Invalid or incomplete multibyte or wide character",
    /* EOVERFLOW 139 */ "Value too large for defined data type",
    /* ECANCELED 140 */ "Operation canceled",
    /* ENOTRECOVERABLE 141 */ "State not recoverable",
    /* EOWNERDEAD 142 */ "Previous owner died",
    /* ESTRPIPE 143 */ "Streams pipe error"};


/****************************************************************************************/
} // namespace util
} // namespace emlsp
#endif
