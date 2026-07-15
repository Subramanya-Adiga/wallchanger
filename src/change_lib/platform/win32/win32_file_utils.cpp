#include "win32_file_utils.hpp"

#include <ntstatus.h>
#include <winternl.h>

using pNtCreateFile = NTSTATUS(__stdcall *)(
    PHANDLE FileHandle, ULONG DesiredAccess, PVOID ObjectAttributes,
    PVOID IoStatusBlock, PLARGE_INTEGER AllocationSize, ULONG FileAttributes,
    ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions,
    PVOID EaBuffer, ULONG EaLength);

using pNtReadFile = ULONG(__stdcall *)(
    IN HANDLE FileHandle, IN HANDLE Event OPTIONAL,
    IN PVOID ApcRoutine OPTIONAL, IN PVOID ApcContext OPTIONAL,
    OUT PVOID IoStatusBlock, OUT PVOID Buffer, IN ULONG Length,
    IN PLARGE_INTEGER ByteOffset OPTIONAL, IN PULONG Key OPTIONAL);

std::expected<HANDLE, std::error_code>
win32_open_file(std::string_view filename) {

  auto create_file = (pNtCreateFile)GetProcAddress(
      GetModuleHandle(L"ntdll.dll"), "NtCreateFile");
  if (create_file == nullptr) {
    return std::unexpected{make_win32_error_code()};
  }

  if ((filename == ".") || (filename == "..")) {
    return std::unexpected{
        make_error_code(win32_error_code{ERROR_INVALID_NAME})};
  }

  auto name = to_wtf8(filename.data(), filename.size());
  HANDLE ret = nullptr;

  UNICODE_STRING file_name = {.Length = static_cast<USHORT>(name.length() << 1),
                              .MaximumLength =
                                  static_cast<USHORT>(sizeof(PWSTR) + 1),
                              .Buffer = name.data()};

  OBJECT_ATTRIBUTES oa = {.Length = sizeof(OBJECT_ATTRIBUTES),
                          .RootDirectory = nullptr,
                          .ObjectName = &file_name,
                          .Attributes = OBJ_CASE_INSENSITIVE,
                          .SecurityDescriptor = nullptr,
                          .SecurityQualityOfService = nullptr};
  IO_STATUS_BLOCK io = {};

  auto res =
      create_file(&ret, GENERIC_ALL, &oa, &io, nullptr, FILE_ATTRIBUTE_NORMAL,
                  FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_CREATE,
                  FILE_NON_DIRECTORY_FILE, nullptr, 0);

  if (res != STATUS_SUCCESS) {
    return std::unexpected{make_HRESULT_error_code(res)};
  }

  return ret;
}
