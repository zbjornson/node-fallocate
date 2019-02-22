#include "nan.h"
#include "stdint.h"

#if defined(__unix__)
#include <fcntl.h>
int _fallocate(int fd, int64_t len) {
  return posix_fallocate(fd, 0, len);
}

#elif defined(_MSC_VER)
#include "Windows.h"

int _fallocate(int fd, int64_t len) {
  // This call prevents us from being able to use n-api,
  // https://github.com/nodejs/abi-stable-node/issues/318
  HANDLE hFile = uv_get_osfhandle(fd);

  FILE_END_OF_FILE_INFO finfo;
  finfo.EndOfFile.QuadPart = len;

  bool success = SetFileInformationByHandle(hFile, FileEndOfFileInfo, &finfo, sizeof finfo);

  if (!success) {
    return GetLastError();
  } else {
    return 0;
  }
}

#endif

NAN_METHOD(fallocateSync) {
  // TODO consider doing the simple checks that appear in 
  // https://github.com/bminor/glibc/blob/7628a1b05adb1e4c6857b87c6f8b71a1d0b1d72c/sysdeps/posix/posix_fallocate.c
  // here so we have more portable error messages for common failures.

	// For portability, posix_fallocate's offset param is not supported.
  // TODO but apparently it's important to start from the file's current size?
  // https://jira.percona.com/browse/PS-1482
	int fd = Nan::To<int>(info[0]).FromJust();
	int64_t len = Nan::To<int64_t>(info[1]).FromJust();
	int result = _fallocate(fd, len);
	if (result) {
		Nan::ThrowError(Nan::ErrnoException(result, "fallocate")); // TODO this is a posix errno
	}
}

NAN_MODULE_INIT(Init) {
	NAN_EXPORT(target, fallocateSync);
}

NODE_MODULE(fallocate, Init);
