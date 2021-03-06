/*
 * @filename 		WrapperCreateFile.h
 * @version			1.0, December 2013
 * @author			R.J. Rodríguez (rjrodriguez@fi.upm.es), I. Rodríguez-Gastón (irodriguez@virtualminds.es)
 * @description		Concrete class to wrap CreateFileA/CreateFileW Windows APIs
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PinWrapperWinAPI.h"

class WrapperCreateFile: public PinWrapperWinAPI
{
	private:
		/**
		 Wrapper of CreateFileA/CreateFileW Windows APIs. Spoofs the return if needed
				before returning to the caller 
		 @param lpFileName the name of the file or device to be created or opened
		 @param dwDesiredAccess the requested access to the file or device
		 @param dwShareMode the requested sharing mode of the file or device
		 @param lpSecurityAttributes a pointer to a SECURITY_ATTRIBUTES structure that contains 
		    two separate but related data members: an optional security descriptor, and a Boolean value that determines
			whether the returned handle can be inherited by child processes
		 @param dwCreationDisposition an action to take on a file or device that exists or does not exist
		 @param dwFlagsAndAttributes the file or device attributes and flags, FILE_ATTRIBUTE_NORMAL being the most common default value for files
		 @param hTemplateFile q valid handle to a template file with the GENERIC_READ access right
		 @return an open handle to the specified file, the same value that original that returns Windows API CreateFileA/CreateFileW
				For more information, check http://msdn.microsoft.com/en-us/library/windows/desktop/aa363858(v=vs.85).aspx
		 */
		static WINDOWS::HANDLE myCreateFile(AFUNPTR orig, 
										WINDOWS::LPCTSTR lpFileName, WINDOWS::DWORD dwDesiredAccess,
										WINDOWS::DWORD dwShareMode, WINDOWS::LPSECURITY_ATTRIBUTES lpSecurityAttributes,
										WINDOWS::DWORD dwCreationDisposition, WINDOWS::DWORD dwFlagsAndAttributes,
										WINDOWS::HANDLE hTemplateFile, CONTEXT *ctx,
										bool isUnicode)
		{
			WINDOWS::HANDLE retVal;
			bool detectionVMs = checkDetection(ctx, "CreateFile", (char *)lpFileName, isUnicode);

			if(detectionVMs)
			{
				printMessage("\t-> [CF] Asked for a dubious filename, spoofing return\n");
				return (WINDOWS::HANDLE)-1;
			}else
				PIN_CallApplicationFunction(ctx, PIN_ThreadId(),
									CALLINGSTD_STDCALL, orig,
									PIN_PARG(WINDOWS::HANDLE), &retVal,
									PIN_PARG(WINDOWS::LPCTSTR), lpFileName,
									PIN_PARG(WINDOWS::DWORD), dwDesiredAccess,
									PIN_PARG(WINDOWS::DWORD), dwShareMode,
									PIN_PARG(WINDOWS::LPSECURITY_ATTRIBUTES), lpSecurityAttributes,
									PIN_PARG(WINDOWS::DWORD), dwCreationDisposition,
									PIN_PARG(WINDOWS::DWORD), dwFlagsAndAttributes,
									PIN_PARG(WINDOWS::HANDLE), hTemplateFile,
									PIN_PARG_END()
									);

			return retVal;
		}
	protected:
		PROTO GetPrototypeFunction(char *funcName)
		{
			return PROTO_Allocate( 
					PIN_PARG(WINDOWS::LONG), 
					CALLINGSTD_STDCALL, funcName,
					PIN_PARG(WINDOWS::LPCTSTR),
					PIN_PARG(WINDOWS::DWORD),
					PIN_PARG(WINDOWS::DWORD),
					PIN_PARG(WINDOWS::LPSECURITY_ATTRIBUTES),
					PIN_PARG(WINDOWS::DWORD),
					PIN_PARG(WINDOWS::DWORD),
					PIN_PARG(WINDOWS::HANDLE), 
					PIN_PARG_END()
					);
		}
		
		void ReplaceFunctionSignature(RTN rtn, PROTO proto, bool isUnicode)
		{
			RTN_ReplaceSignature(rtn, (AFUNPTR) WrapperCreateFile::myCreateFile,
					IARG_PROTOTYPE, proto,
					IARG_ORIG_FUNCPTR,
					IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
					IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
					IARG_FUNCARG_ENTRYPOINT_VALUE, 2,
					IARG_FUNCARG_ENTRYPOINT_VALUE, 3,
					IARG_FUNCARG_ENTRYPOINT_VALUE, 4,
					IARG_FUNCARG_ENTRYPOINT_VALUE, 5,
					IARG_FUNCARG_ENTRYPOINT_VALUE, 6,
					IARG_CONTEXT,
					IARG_BOOL, isUnicode,
					IARG_END
					);
		}

	public:
		WrapperCreateFile():PinWrapperWinAPI()
		{
			strcpy(this -> funcName, "CreateFile");
		}

		~WrapperCreateFile(){};
};